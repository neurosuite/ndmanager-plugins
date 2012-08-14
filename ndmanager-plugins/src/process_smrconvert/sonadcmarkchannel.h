/***************************************************************************
 *   Copyright (C) 2004-2008 by Michael Zugaro                             *
 *   michael.zugaro@college-de-france.fr                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program. If not, see <http://www.gnu.org/licenses/>.  *
 *                                                                         *
 ***************************************************************************/
#ifndef SONADCMARKCHANNEL_H
#define SONADCMARKCHANNEL_H

#include "sonchannel.h"
#include "sonerror.h"
/**
@author Michael Zugaro
*/
class SONADCMarkChannel : public SONChannel
{
	public:
		/**
			Constructor
			@param	file		input file
			@param	channel		channel ID
		*/
		SONADCMarkChannel(SONFile *file,uint16_t channel);
		/**
			Destructor
		*/
		virtual ~SONADCMarkChannel();
		/**
			Read ADC data
		*/
		virtual void read() throw (SONError);
		/**
			Write ADC data
			@param	times	timestamp file
			@param	waveforms	waveform file
			@param	reverse	reverse values?
		*/
		virtual void write(ostream& times,ostream& waveforms,bool reverse = false) const throw (SONError);

	private:

		uint16_t		channel;
		int32_t		nWaveforms;
		uint16_t		nSamplesPerWaveform;
		int16_t		*waveform;
		int32_t		*time;
};

#endif
