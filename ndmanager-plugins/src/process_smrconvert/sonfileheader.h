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
#ifndef SONFILEHEADER_H
#define SONFILEHEADER_H

#include <iostream>
#include <fstream>
#include <stdint.h>

class SONFile;

using namespace std;

/**
@author Michael Zugaro
*/
class SONFileHeader
{
	friend class SONExtractor;
	friend ostream& operator<<(ostream& out,const SONFileHeader& fileHeader);

	public:
		/**
			Constructor
			@param	file		input file stream
		*/
   	SONFileHeader(SONFile *file);
		/**
			Destructor
		*/
		virtual ~SONFileHeader();
		/**
			Read SON file header
		*/
		virtual void read();
		/**
			Get number of channels in SON file
			@return	number of channels
		*/
		virtual inline int16_t getNChannels() const;
		/**
			Get system ID (Spike 2 version)
			@return	system ID
		*/
		virtual inline int16_t getSystemID() const;
		/**
			Conversion from ticks to seconds
			@return	clock tick duration (in s)
		*/
		virtual inline double getSecondsPerTick() const;
		/**
			Get time units per ADC interrupt
			@return	time per ADC
		*/
		virtual inline uint16_t getTimePerADC() const;

	private:

		SONFile		*file;
		int16_t		systemID;
		char			copyright[10+1];
		char			creator[8+1];
		uint16_t		usPerTime;
		uint16_t		timePerADC;
		int16_t		fileState;
		int32_t		firstData;
		int16_t		nChannels;
		uint16_t		channelSize;
		uint16_t		extraData;
		uint16_t		bufferSize;
		uint16_t		osFormat;
		int32_t		maxFTime;
		double		timeBase;
		struct
		{
			char			detail[6];
			uint16_t		year;
		} timeDate;
		char			pad[52];
		char			*fileComment[5];
};

int16_t SONFileHeader::getNChannels() const
{
	return nChannels;
}

int16_t SONFileHeader::getSystemID() const
{
	return systemID;
}

// See note on time units in sonfile.h
double SONFileHeader::getSecondsPerTick() const
{
	return usPerTime * timeBase;
}

// See note on time units in sonfile.h
uint16_t SONFileHeader::getTimePerADC() const
{
	return timePerADC;
}

#endif
