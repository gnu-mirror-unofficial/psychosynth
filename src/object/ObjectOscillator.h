/***************************************************************************
 *                                                                         *
 *   PSYCHOSYNTH                                                           *
 *   ===========                                                           *
 *                                                                         *
 *   Copyright (C) 2007 Juan Pedro Bolivar Puente                          *
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

#ifndef OBJECTOSCILLATOR_H
#define OBJECTOSCILLATOR_H

#include <iostream>
#include <cmath>

#include "object/Object.h"
#include "object/KnownObjects.h"
#include "object/Oscillator.h"

class ObjectOscillator : public Object
{		
public:	
    enum InAudioSocketID {
	N_IN_A_SOCKETS
    };
	
    enum InControlSocketID {
	IN_C_FREQUENCY,
	IN_C_AMPLITUDE,
	N_IN_C_SOCKETS
    };

    enum WaveType {
	OSC_SINE     = Oscillator::SINE,
	OSC_SQUARE   = Oscillator::SQUARE,
	OSC_TRIANGLE = Oscillator::TRIANGLE,
	OSC_SAWTOOTH = Oscillator::SAWTOOTH,
	OSC_MOOGSAW  = Oscillator::MOOGSAW,
	OSC_EXP      = Oscillator::EXP,
	N_OSC_TYPES
    };

    enum ModType {
	MOD_FM = Oscillator::FM,
	MOD_AM = Oscillator::AM,
	MOD_PM = Oscillator::PM,
	N_MOD_TYPES
    };
    
    enum ParamID {
	PARAM_WAVE,
	PARAM_MODULATOR,
	PARAM_FREQUENCY,
	PARAM_AMPLITUDE,
	N_PARAM
    };

    static const float DEFAULT_FREQ = 220.0f;
    static const float DEFAULT_AMPL = 0.5f;
    
protected:
    Oscillator m_oscillator;

    void updateOscParams();
    
private:
    int   m_param_wave;
    int   m_param_mod;
    float m_param_freq;
    float m_param_ampl;
    
public:
    ObjectOscillator(const AudioInfo& prop,
		     int obj_type,
		     int n_audio_out,
		     int n_control_out);
    
    ~ObjectOscillator();
};

#endif /* OBJECTOSCILLATOR_H */
