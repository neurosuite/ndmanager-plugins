/***************************************************************************
 *   Copyright (C) 2004-2011 by Michael Zugaro                             *
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
#ifndef SONFILE_H
#define SONFILE_H

#include <iostream>
#include <fstream>
#include <string>
#include <stdint.h>

#include "sonerror.h"
#include "sonfileheader.h"
#include "sonchannelinfo.h"
#include "sonblockheaders.h"

using namespace std;

// Note on time units (stay focused, this is crazy)
//
// There are several time units in SON files:
//
//   * time base units (>= version 6, fixed value of 1.0e-6 in earlier versions)
//   * clock ticks
//   * channel sampling intervals
//
// The time base is given by timeBase (defined in the file header) and is typically one microsecond (1.0e-6).
// The number of time base units per clock tick is given by usPerTime (also defined in the file header):
//
//        clock tick duration (in s) = timeBase * usPerTime               (>= version 6)
//    or  clock tick duration (in s) = 1.0e-6 * usPerTime                 (< version 6)
//
// For instance, if timeBase = 1.0e-6 and usPerTime = 2, then a clock tick lasts 2 µs. Finally, the channel
// sampling intervals vary with the kind of channel. For an ADC channel it is given by:
//
//       sampling interval (in ticks) = lChanDvd                (>= version 6)
//   or  sampling interval (in ticks) = divide * timePerADC     (< version 6)
//
// (lChanDvd and divide are defined in the channel info, and timePerADC is defined in the file header).
// As a consequence:
//
//       sampling interval (in s) = lChanDvd * usPerTime * timeBase                (>= version 6)
//   or  sampling interval (in s) = divide * timePerADC * usPerTime * 1.0e-6       (< version 6)

/**
@author Michael Zugaro
*/
class SONFile : public ifstream
{
	public:
		/**
			Constructor

			@param	filename	smr file name
		*/
		SONFile(string filename);
		/**
			Destructor
		*/
		virtual ~SONFile();
		/**
			Open file
		*/
		virtual void open() throw(SONError);
		/**
			Initialization: read file header and channel information
		*/
		virtual void init() throw(SONError);
		/**
			Read block headers for a given channel. This is not done on initialization
			because it can take a lot of time and we do not want to block the program
			unless this is absolutely necessary.
		*/
		virtual inline void readBlockHeaders(uint16_t channel);
		/**
			Get file name
			@return	file name
		*/
		virtual inline string getName() const;
		/**
			Get number of channels
			@return	number of channels
		*/
		virtual inline uint16_t getNChannels() const;
		/**
			Get file format version
			@return	file format version
		*/
		virtual inline const int16_t getVersion() const;
		/**
			Get file header
			@return	file header
		*/
		virtual inline const SONFileHeader *getFileHeader() const;
		/**
			Get channel information
			@param	channel channel ID
			@return	channel info
		*/
		virtual inline const SONChannelInfo *getChannelInfo(uint16_t channel) const;
		/**
			Get block headers for a given channel
			@param	channel channel ID
			@return	block headers
		*/
		virtual inline const SONBlockHeaders *getBlockHeaders(uint16_t channel) const;
		/**
			Get sampling frequency (in Hz) for a given channel
			@param	channel channel ID
			@return	sampling frequency
		*/
		virtual inline double getSamplingFrequency(uint16_t channel) const;
		/**
			Get clock tick duration (in s)
			@return	clock tick duration
		*/
		virtual inline double getSecondsPerTick() const;
		/**
			Get sampling interval (in ticks) for a given channel
			@param	channel channel ID
			@return	sampling interval
		*/
		virtual inline uint16_t getTicksPerSI(uint16_t channel) const;
		/**
			Get number of blocks for a given channel
			@param	channel channel ID
			@return	number of blocks
		*/
		virtual inline uint32_t getNBlocks(uint16_t channel) const;
		/**
			Get number of samples in a given block for a given channel
			@param	channel channel ID
			@param	block	block number
			@return	number of samples actually recorded
		*/
		virtual inline uint16_t getNSamplesInBlock(uint16_t channel,uint32_t block) const;
		/**
			Get number of samples for a given channel
			@param	channel channel ID
			@return	total number of samples
		*/
		virtual inline int64_t getNSamples(uint16_t channel) const;
		/**
			Get number of samples per waveform for a given channel
			@param	channel channel ID
			@return	number of samples per waveform
		*/
		virtual inline uint16_t getNSamplesPerWaveform(uint16_t channel) const;
		/**
			Get file offset of first block of data for a given channel
			@param	channel	channel ID
			@return	offset
		*/
		virtual inline int64_t getFirstBlockOffset(uint16_t channel) const;
		/**
			Get file offset of a given block of data for a given channel
			@param	channel channel ID
			@param	block	block number
			@return	offset
		*/
		virtual inline int64_t getBlockOffset(uint16_t channel,uint32_t block) const;
		/**
			Test if a given channel contains ADC data
			@param	channel channel ID
			@return	test result
		*/
		virtual inline bool isADCChannel(uint16_t channel) const;
		/**
			Test if a given channel contains ADC Mark data
			@param	channel channel ID
			@return	test result
		*/
		virtual inline bool isADCMarkChannel(uint16_t channel) const;
		/**
			Test if a given channel contains Event data
			@param	channel channel ID
			@return	test result
		*/
		virtual inline bool isEventChannel(uint16_t channel) const;
		/**
			Test if a given channel contains simple Event data
			@param	channel channel ID
			@return	test result
		*/
		virtual inline bool isSimpleEventChannel(uint16_t channel) const;
		/**
			Test if a given channel contains Marker data
			@param	channel channel ID
			@return	test result
		*/
		virtual inline bool isMarkerChannel(uint16_t channel) const;
		/**
			Test if a given channel contains Text Marker data
			@param	channel channel ID
			@return	test result
		*/
		virtual inline bool isTextMarkChannel(uint16_t channel) const;

	private:

		string					filename;
		SONFileHeader			*fileHeader;
		SONChannelInfo			*channelInfo;
		SONBlockHeaders		*blockHeaders;
		uint16_t					nADCChannels;
		uint16_t					ADCChannels[1000];
		uint16_t					nADCMarkChannels;
		uint16_t					ADCMarkChannels[1000];
		uint16_t					nEventChannels;
		uint16_t					eventChannels[1000];
};

string SONFile::getName() const
{
	return filename;
}

const SONFileHeader *SONFile::getFileHeader() const
{
	return fileHeader;
}

const int16_t SONFile::getVersion() const
{
	return fileHeader->getSystemID();
}

const SONChannelInfo *SONFile::getChannelInfo(uint16_t channel) const
{
	return &channelInfo[channel];
}

const SONBlockHeaders *SONFile::getBlockHeaders(uint16_t channel) const
{
	return &blockHeaders[channel];
}

uint16_t SONFile::getNChannels() const
{
	return fileHeader->getNChannels();
}

uint16_t SONFile::getNSamplesPerWaveform(uint16_t channel) const
{
	return channelInfo[channel].getNSamplesPerWaveform();
}

// See note on time units at the beginning of this file
double SONFile::getSamplingFrequency(uint16_t channel) const
{
	return 1.0L/(getSecondsPerTick()*getTicksPerSI(channel));
}

// See note on time units at the beginning of this file
double SONFile::getSecondsPerTick() const
{
	return fileHeader->getSecondsPerTick();
}

// See note on time units at the beginning of this file
uint16_t SONFile::getTicksPerSI(uint16_t channel) const
{
	if ( fileHeader->getSystemID() < 6 )
	{
		return channelInfo[channel].getDivide() * fileHeader->getTimePerADC();
	}
	else
	{
		return channelInfo[channel].getLChanDvd();
	}
}

uint32_t SONFile::getNBlocks(uint16_t channel) const
{
	return channelInfo[channel].getNBlocks();
}

uint16_t SONFile::getNSamplesInBlock(uint16_t channel,uint32_t block) const
{
	return blockHeaders[channel].getNSamplesInBlock(block);
}

int64_t SONFile::getNSamples(uint16_t channel) const
{
	return blockHeaders[channel].getNSamples();
}

int64_t SONFile::getFirstBlockOffset(uint16_t channel) const
{
	return channelInfo[channel].getFirstBlockOffset();
}

int64_t SONFile::getBlockOffset(uint16_t channel,uint32_t block) const
{
	return blockHeaders[channel].getBlockOffset(block);
}

bool SONFile::isADCChannel(uint16_t channel) const
{
	return channelInfo[channel].isADCChannel();
}

bool SONFile::isADCMarkChannel(uint16_t channel) const
{
	return channelInfo[channel].isADCMarkChannel();
}

bool SONFile::isEventChannel(uint16_t channel) const
{
	return channelInfo[channel].isEventChannel();
}

bool SONFile::isSimpleEventChannel(uint16_t channel) const
{
	return channelInfo[channel].isSimpleEventChannel();
}

bool SONFile::isMarkerChannel(uint16_t channel) const
{
	return channelInfo[channel].isMarkerChannel();
}

bool SONFile::isTextMarkChannel(uint16_t channel) const
{
	return channelInfo[channel].isTextMarkChannel();
}

void SONFile::readBlockHeaders(uint16_t channel)
{
	blockHeaders[channel].read();
}


#endif
