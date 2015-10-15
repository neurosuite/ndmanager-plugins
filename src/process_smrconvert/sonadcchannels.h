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
#ifndef SONADCCHANNEL_H
#define SONADCCHANNEL_H

#include "sonerror.h"
#include <inttypes.h>
#include <cstring>

/**
@author Michael Zugaro
*/
class SONADCChannels
{
	static const int blockHeaderSize;

	public:
		/**
			Constructor

			@param	file	input file
			@param	nChannels	number of channels to process
			@param	channels	list of channel IDs
			@param	nBufferSamples	read/write block size (in samples per channel)
		*/
		SONADCChannels(SONFile *file,uint16_t nChannels,uint16_t *channels,int32_t nBufferSamples = 8192);
		/**
			Destructor
		*/
		virtual ~SONADCChannels();
		/**
			Restart from sample 0
		*/
		virtual inline void reset();
		/**
			Test if the SON file contains more data for the selected channels

			@return	true if there is no data left
		*/
		virtual bool done() const;
		/**
			Read ADC data (one buffer at a time)
		*/
		virtual void read() throw (SONError);
		/**
			Write ADC data (one buffer at a time)

			@param	out	output stream (.dat file)
			@param	reverse	should data values be reversed?
		*/
		virtual void write(ostream& out,bool reverse = false) const throw (SONError);
		/**
			Return percentage of input file read
		*/
		virtual inline float percentDone() const;

	private:

		SONFile					*file;
		uint16_t					nChannels;
		uint16_t					*channels;
		uint16_t					samplingInterval;
		bool						bof;
		uint32_t					*currentBlock;
		uint16_t					*currentSampleInBlock;
		uint32_t					*lastBlock;
		uint16_t					*lastSampleInLastBlock;
		int32_t					currentSampleInBuffer;
		int32_t					nBufferSamples;
		int32_t					*nSamplesRead;
		int16_t					*buffer;
		int16_t					*multiplexed;
		int32_t					maxNSamples;
};

void SONADCChannels::reset()
{
	bof = true;
}

inline float SONADCChannels::percentDone() const
{
	return ((float)currentBlock[0])/((float)lastBlock[0]);
}


#endif
