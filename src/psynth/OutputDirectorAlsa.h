/***************************************************************************
 *                                                                         *
 *   PSYCHOSYNTH                                                           *
 *   ===========                                                           *
 *                                                                         *
 *   Copyright (C) Juan Pedro Bolivar Puente 2007, 2008                    *
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

#ifndef PSYNTH_OUTPUT_DIRECTOR_ALSA_H
#define PSYNTH_OUTPUT_DIRECTOR_ALSA_H

#include "psynth/DefaultsAlsa.h"
#include "psynth/OutputDirector.h"
#include "output/OutputAlsa.h"

class OutputDirectorAlsa : public OutputDirector
{
    OutputAlsa* m_output;

    bool onDeviceChange(const ConfNode& conf) {
	std::string device;
	Output::State old_state;
	
	conf.get(device);
	
	old_state = m_output->getState();
	m_output->gotoState(Output::NOTINIT);
	m_output->setDevice(device);
	m_output->gotoState(old_state);

	return false;
    }
    
    virtual Output* doStart(ConfNode& conf) {
	std::string device;
	
	conf.getChild("out_device").def(DEFAULT_ALSA_OUT_DEVICE);
	conf.getChild("out_device").get(device);
	conf.getChild("out_device").addChangeEvent(MakeEvent(this, &OutputDirectorAlsa::onDeviceChange));

	m_output = new OutputAlsa;
	m_output->setDevice(device);

	return m_output;
    };

    virtual void doStop(ConfNode& conf) {
	conf.getChild("out_device").deleteChangeEvent(MakeEvent(this, &OutputDirectorAlsa::onDeviceChange));
	if (m_output) {
	    delete m_output;
	    m_output = NULL;
	}
    }

public:
    OutputDirectorAlsa() :
	m_output(NULL) {}
};

class OutputDirectorAlsaFactory : public OutputDirectorFactory
{
public:
    virtual const char* getName() {
	return DEFAULT_ALSA_NAME;
    }
    
    virtual OutputDirector* createOutputDirector() {
	return new OutputDirectorAlsa;
    }
};

#endif /* PSYNTH_OUTPUT_DIRECTOR_ALSA_H */
