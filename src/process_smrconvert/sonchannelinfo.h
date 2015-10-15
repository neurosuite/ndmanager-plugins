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
#ifndef SONCHANNELINFO_H
#define SONCHANNELINFO_H

#include <iostream>
#include <fstream>
#include <stdint.h>

using namespace std;

class SONFile;

/**
@author Michael Zugaro
*/
class SONChannelInfo
{
	friend class SONExtractor;
	friend ostream& operator<<(ostream& out,const SONChannelInfo& channelInfo);

	public:

		/**
			File header size (in bytes)
		*/
		static const int fileHeaderSize;
		/**
			Channel header size (in bytes)
		*/
		static const int channelHeaderSize;
		/**
			file offset coeff (file format version >= 9)
		*/
		static const int diskBlock;

		enum
		{
			Off=0,		// the channel is OFF
			ADC, 			// 16-bit waveform
			EventFall,	// events (falling edges)
			EventRise,	// events (rising edges)
			EventBoth,	// events (either)
			Marker,		// events + four 8-bit keys
			ADCMark,		// markers + ADC data
			RealMark,	// markers + float numbers
			TextMark,	// markers + text
			RealWave		// float waveform
		};

	public:
		/**
			Default constructor (necessary to create arrays of SONChannelInfo)
			@see init
		*/
   	SONChannelInfo();
		/**
			Constructor
			@param	file	input file
			@param	channel	channel number
		*/
   	SONChannelInfo(SONFile *file,uint16_t channel);
		/**
			Destructor
		*/
		virtual ~SONChannelInfo();
		/**
			Initialization (necessary to create arrays of SONChannelInfo)
			@param	file	input file
			@param	channel	channel number
		*/
   	virtual inline void init(SONFile *file,uint16_t channel);
		/**
			Read SON file header
		*/
		virtual void read();
		/**
			Test whether the channel is empty
		*/
		virtual inline bool isEmpty() const;
		/**
			Get number of blocks in the channel
			@return	number of blocks
		*/
		virtual inline uint32_t getNBlocks() const;
		/**
			Get file offset of first block of data for the channel
			@return	first block
		*/
		virtual inline int64_t getFirstBlockOffset() const;
		/**
			Get number of samples per waveform
			@return	number of samples per waveform
		*/
		virtual inline uint16_t getNSamplesPerWaveform() const;
		/**
			Get lChanDvd field (used to compute timestamps)
			@return	lChanDvd
		*/
		virtual inline int32_t getLChanDvd() const;
		/**
			Get sampling rate
			@return	sampling rate
		*/
		virtual inline float getRate() const;
		/**
			Get divide
			@return	divide
		*/
		virtual inline uint16_t getDivide() const;
		/**
			Is this an ADC channel?
			@return	true if this is an ADC channel
		*/
		virtual inline bool isADCChannel() const;
		/**
			Is this an ADC mark channel?
			@return	true if this is an ADC mark channel
		*/
		virtual inline bool isADCMarkChannel() const;
		/**
			Is this an event channel (EventRise, EventFall, EventBoth, Marker, RealMark or TextMark)?
			@return	true if this is an event channel
		*/
		virtual inline bool isEventChannel() const;
		/**
			Is this a simple Event channel (EventRise, EventFall or EventBoth)?
			@return	true if this is a simple Event channel
		*/
		virtual inline bool isSimpleEventChannel() const;
		/**
			Is this an Marker channel?
			@return	true if this is an Marker channel
		*/
		virtual inline bool isMarkerChannel() const;
		/**
			Is this an TextMark channel?
			@return	true if this is an TextMark channel
		*/
		virtual inline bool isTextMarkChannel() const;

	private:

		SONFile			*file;
		int				channel;
		uint16_t			delSize;
		int32_t			nextDelBlock;
		int32_t			fBlockOffset;
		int64_t			firstBlockOffset;
		int32_t			lBlockOffset;
		int64_t			lastBlockOffset;
		uint16_t			nBlocksLSW;
		int16_t			nBlocksMSW;
		uint32_t			nBlocks;
		uint16_t			nExtra;
		int16_t			preTrig;
		uint16_t			phySz;
		uint16_t			maxData;
		char				comment[71+1];
		int32_t			maxChanTime;
		int32_t			lChanDvd;
		int16_t			phyChan;
		char				title[9+1];
		float				idealRate;
		char				kind;
		char				pad;
		union
		{
			struct
			{
				float			scale;
				float			offset;
				char			units[5+1];
				uint16_t		divide;
			} adc;
			struct
			{
				bool			initLow;
				bool			nextLow;
			} event;
			struct
			{
				float			min;
				float			max;
				char			units[5+1];
			} real;
		} v;
};

void SONChannelInfo::init(SONFile *file,uint16_t channel)
{
	this->file = file;
	this->channel = channel;
}

bool SONChannelInfo::isEmpty() const
{
	return firstBlockOffset == -1;
}

uint32_t SONChannelInfo::getNBlocks() const
{
	// This takes into account differences between file format versions (pre vs post 9)
	return nBlocks;
}

int64_t SONChannelInfo::getFirstBlockOffset() const
{
	// This takes into account differences between file format versions (pre vs post 9)
	return firstBlockOffset;
}

int32_t SONChannelInfo::getLChanDvd() const
{
	return lChanDvd;
}

float SONChannelInfo::getRate() const
{
	return idealRate;
}

uint16_t SONChannelInfo::getDivide() const
{
	return v.adc.divide;
}

bool SONChannelInfo::isADCChannel() const
{
	return kind == ADC;
}

bool SONChannelInfo::isADCMarkChannel() const
{
	return kind == ADCMark;
}

bool SONChannelInfo::isEventChannel() const
{
	return kind == EventFall || kind == EventRise || kind == EventBoth || kind == Marker || kind == TextMark;
}

bool SONChannelInfo::isSimpleEventChannel() const
{
	return kind == EventFall || kind == EventRise || kind == EventBoth;
}

bool SONChannelInfo::isMarkerChannel() const
{
	return kind == Marker;
}

bool SONChannelInfo::isTextMarkChannel() const
{
	return kind == TextMark;
}

uint16_t SONChannelInfo::getNSamplesPerWaveform() const
{
	return nExtra/sizeof(uint16_t);
}

#endif
