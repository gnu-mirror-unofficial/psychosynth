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

#include <algorithm>
#include "node/node_audio_oscillator.h"

using namespace std;

namespace psynth
{

PSYNTH_DEFINE_NODE_FACTORY (node_audio_oscillator);

void node_audio_oscillator::do_update (const node* caller, int caller_port_type, int caller_port)
{
    audio_buffer* buf = get_output<audio_buffer> (LINK_AUDIO, OUT_A_OUTPUT);
    sample* out = buf->get_channel(0);

    update_osc(out);
	
    /* Copy on the other channels. */
    for (size_t i = 1; i < (size_t) get_info().num_channels; i++)
	memcpy((*buf)[i], (*buf)[0], sizeof(sample) * get_info().block_size);
}

} /* namespace psynth */
