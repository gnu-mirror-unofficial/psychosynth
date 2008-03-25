/***************************************************************************
 *                                                                         *
 *   PSYCHOSYNTH                                                           *
 *   ===========                                                           *
 *                                                                         *
 *   Copyright (C) Juan Pedro Bolivar Puente 2007                          *
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation, either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 *                                                                         *
 ***************************************************************************/

#include "common/Deleter.h"
#include "object/Object.h"

#include <cmath>

#define ENV_RISE_SECONDS  0.05f
#define ENV_FALL_SECONDS  0.05f
#define ENV_DELTA(srate, sec)  (1/((sec)*(srate)))

using namespace std;

namespace psynth
{

Object::Object(const AudioInfo& info, int type,
	       const std::string& name,
	       int n_in_audio, int n_in_control,
	       int n_out_audio, int n_out_control,
	       bool single_update) :
    m_audioinfo(info),
    m_outdata_audio(n_out_audio, AudioBuffer(info)),
    m_outdata_control(n_out_control, ControlBuffer(info.block_size)),
    m_nparam(0),
    m_id(OBJ_NULL_ID),
    m_type(type),
    m_name(name),
    m_param_position(0,0),
    m_param_radious(5.0f),
    m_param_mute(false),
    m_updated(false),
    m_single_update(single_update)
{   
    addParam("position", ObjParam::VECTOR2F, &m_param_position);
    addParam("radious", ObjParam::FLOAT, &m_param_radious);
    addParam("mute", ObjParam::INT, &m_param_mute);
    
    m_out_sockets[LINK_AUDIO].resize(n_out_audio, OutSocket(LINK_AUDIO));
    m_out_sockets[LINK_CONTROL].resize(n_out_control, OutSocket(LINK_CONTROL));
    m_in_sockets[LINK_AUDIO].resize(n_in_audio, InSocketManual(LINK_AUDIO));
    m_in_sockets[LINK_CONTROL].resize(n_in_control, InSocketManual(LINK_CONTROL));
    m_in_envelope[LINK_AUDIO].resize(n_in_audio);
    m_in_envelope[LINK_CONTROL].resize(n_in_control);
    
    setEnvelopesDeltas();
}

Object::~Object()
{
    for_each(m_params.begin(), m_params.end(), Deleter<ObjParam*>());
//    cout << "Deleting object.\n";
}

void Object::setEnvelopesDeltas()
{
    int i;
    
    float rise_dt = ENV_DELTA(m_audioinfo.sample_rate, ENV_RISE_SECONDS);
    float fall_dt = -ENV_DELTA(m_audioinfo.sample_rate, ENV_FALL_SECONDS);

    for (i = 0; i < LINK_TYPES; ++i)
	for (vector<EnvelopeSimple>::iterator it = m_in_envelope[i].begin();
	     it != m_in_envelope[i].end();
	     ++it) {
	    it->setDeltas(rise_dt, fall_dt);
	}

    m_out_envelope.setDeltas(rise_dt, fall_dt);
    m_out_envelope.set(1.0f);
}

void Object::addParam(const std::string& name, int type, void* val)
{
    m_params.push_back(new ObjParam);
    m_params[m_nparam]->configure(m_nparam, name, type, val);
    m_nparam++;   
}

void Object::addParam(const std::string& name, int type, void* val, ObjParam::Event ev)
{
    m_params.push_back(new ObjParam);
    m_params[m_nparam]->configure(m_nparam, name, type, val, ev);
    m_nparam++;
}

void Object::delParam(int index)
{
    
    m_nparam--;
}

ObjParam& Object::param(const std::string& name)
{
    for (vector<ObjParam*>::iterator it = m_params.begin();
	 it != m_params.end();
	 ++it)
	if (name == (*it)->getName())
	    return **it;
    
    return m_null_param;
}

const ObjParam& Object::param(const std::string& name) const
{
    for (vector<ObjParam*>::const_iterator it = m_params.begin();
	 it != m_params.end();
	 ++it)
	if (name == (*it)->getName())
	    return **it;
    
    return m_null_param;
}

void Object::clearConnections()
{
    int i, j;

    for (i = 0; i < LINK_TYPES; ++i)
	for (j = 0; j < m_in_sockets[i].size(); ++j)
	    if (!m_in_sockets[i][j].isEmpty())
		connectIn(i, j, NULL, 0);    
    
    for (i = 0; i < LINK_TYPES; ++i)
	for (j = 0; j < m_out_sockets[i].size(); ++j)
	    if (!m_out_sockets[i][j].isEmpty())
		m_out_sockets[i][j].clearReferences();
}

bool Object::hasConnections()
{
    int i, j;

    for (i = 0; i < LINK_TYPES; ++i)
	for (j = 0; j < m_in_sockets[i].size(); ++j)
	    if (!m_in_sockets[i][j].isEmpty())
		return true;
    
    for (i = 0; i < LINK_TYPES; ++i)
	for (j = 0; j < m_out_sockets[i].size(); ++j)
	    if (!m_out_sockets[i][j].isEmpty())
		return true;

    return false;
}

void Object::connectIn(int type, int in_socket, Object* src, int out_socket)
{
    m_in_sockets[type][in_socket].src_obj = src;
    m_in_sockets[type][in_socket].src_sock = out_socket;

    if (!m_in_envelope[type][in_socket].finished()) {
	m_in_sockets[type][in_socket].must_update = true;
	m_in_envelope[type][in_socket].release();
    } else {
	m_in_sockets[type][in_socket].must_update = false;
	forceConnectIn(type, in_socket, src, out_socket);
    }
}

void Object::forceConnectIn(int type, int in_socket, Object* src, int out_socket)
{
    m_in_envelope[type][in_socket].press();
    
    if (m_in_sockets[type][in_socket].m_srcobj)
	m_in_sockets[type][in_socket].m_srcobj->
	    m_out_sockets[type][out_socket].removeReference(this, in_socket);
    
    m_in_sockets[type][in_socket].set(src, out_socket);
    
    if (src)
	src->m_out_sockets[type][out_socket].addReference(this, in_socket);    
}

inline bool Object::canUpdate(const Object* caller, int caller_port_type,
			      int caller_port)
{
    bool ret;

    if (m_single_update || !caller)
	ret = !m_updated;
    else
	ret =
	    m_updated_links[caller_port_type].insert(make_pair(caller->getID(),
							       caller_port)).second;
    m_updated = true;
    
    return ret;
}

void Object::InSocket::updateInput(const Object* caller, int caller_port_type, int caller_port)
{
    if (m_srcobj) {
	m_srcobj->update(caller, caller_port_type, caller_port);

	if (m_type == LINK_AUDIO) {
	    const AudioBuffer* buf = m_srcobj->getOutput<AudioBuffer>(m_type, m_srcport);
	    if (buf)
		for (list<Watch*>::iterator it = m_watchs.begin(); it != m_watchs.end(); ++it)
		    (*it)->update(*buf);
	} else {
	    const ControlBuffer* buf = m_srcobj->getOutput<ControlBuffer>(m_type, m_srcport);
	    if (buf)
		for (list<Watch*>::iterator it = m_watchs.begin(); it != m_watchs.end(); ++it) {
		    (*it)->update(*buf);
		}
	}
    }
}

void Object::updateEnvelopes()
{
    int i, j;
    
    for (i = 0; i < LINK_TYPES; ++i)
	for (vector<EnvelopeSimple>::iterator it = m_in_envelope[i].begin();
	     it != m_in_envelope[i].end();
	     ++it)
	    it->update(m_audioinfo.block_size);

    /* Apply envelopes to output (for soft muting) */
    for (i = 0; i < m_outdata_audio.size(); ++i)
	for (j = 0; j < m_audioinfo.num_channels; ++j) {
	    EnvelopeSimple env = m_out_envelope;
	    env.update(m_outdata_audio[i][j], m_audioinfo.block_size);
	}
    for (i = 0; i < m_outdata_control.size(); ++i) {
	EnvelopeSimple env = m_out_envelope;
	env.update(m_outdata_control[i].getData(), m_audioinfo.block_size);
    }

    m_out_envelope.update(m_audioinfo.block_size);
}

void Object::updateInSockets()
{
    int i, j;

    /* TODO: Thread synch! */
    for (j = 0; j < LINK_TYPES; ++j)
	for (i = 0; i < m_in_envelope[j].size(); ++i) {
	    if (m_in_sockets[j][i].must_update &&
		m_in_envelope[j][i].finished()) {
		forceConnectIn(j, i,
			       m_in_sockets[j][i].src_obj,
			       m_in_sockets[j][i].src_sock);
		m_in_sockets[j][i].must_update = false;
	    }
	}
}

void Object::updateParams()
{
    size_t j;
    
    for (vector<ObjParam*>::iterator i = m_params.begin(); i != m_params.end(); ++i)
	(*i)->update();

    if (m_param_mute)
	m_out_envelope.release();
    else
	m_out_envelope.press();
}

void Object::updateInputs()
{
    size_t j, i;

    for (i = 0; i < LINK_TYPES; ++i)
	for (j = 0; j < m_in_sockets[i].size(); ++j)
	    m_in_sockets[i][j].updateInput(this, i, j);
}

void Object::update(const Object* caller, int caller_port_type, int caller_port)
{
    if (canUpdate(caller, caller_port_type, caller_port)) {
	updateParams();

	if (!m_param_mute || !m_out_envelope.finished()) {
	    updateInputs();
	    doUpdate(caller, caller_port_type, caller_port); 
	}

	updateEnvelopes();
	updateInSockets();
    }
}

void Object::setInfo(const AudioInfo& info)
{
    int i;
    
    for (i = 0; i < m_outdata_audio.size(); ++i)
	m_outdata_audio[i].setInfo(info);

    if (m_audioinfo.block_size != info.block_size)
	for (i = 0; i < m_outdata_control.size(); ++i)
	    m_outdata_control[i].resize(info.block_size);

    m_audioinfo = info;

    setEnvelopesDeltas();
}   

} /* namespace psynth */
