/***************************************************************************
 *                                                                         *
 *   Copyright (C) 2005-2008 by Michael Zugaro                             *
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

#include <sstream>

#include "soneventchannel.h"
#include "sonblockheaders.h"
#include "sonfileheader.h"
#include "sonfile.h"

SONEventChannel::SONEventChannel(SONFile *file,uint16_t channel) :
SONChannel(file,channel),
markers(0),
time(0),
nEvents(0)
{
}

SONEventChannel::~SONEventChannel()
{
}

void SONEventChannel::read() throw (SONError)
{
	// Make sure this is an Event channel
	if ( !file->isEventChannel(channel) )
	{
		stringstream ss;
		ss << channel;
		string channelStr;
		ss >> channelStr;
		string message = "Channel " + channelStr + " is not an Event channel";
		throw SONError(SONError::WRONG_CHANNEL_TYPE,message);
	}

	file->readBlockHeaders(channel);

	nEvents = file->getNSamples(channel);
	if ( nEvents == 0 ) throw SONError(SONError::EMPTY_CHANNEL,"");

	delete[] markers;
	delete[] time;
	time = new int32_t[nEvents];
	markers = new char[nEvents];

	uint16_t		currentEvent = 0;
	uint32_t		nBlocks = file->getNBlocks(channel);

	// Read data
	for ( uint32_t currentBlock = 0 ; currentBlock < nBlocks ; ++currentBlock )
	{
		// How many events should be read from this block?
		uint16_t	nBlockEvents = file->getNSamplesInBlock(channel,currentBlock);

		// Read and update variables
		file->seekg(file->getBlockOffset(channel,currentBlock)+blockHeaderSize,ios_base::beg);
		for ( uint16_t j = 0 ; j < nBlockEvents ; ++j )
		{
			file->read((char *)&time[currentEvent],sizeof(time[0]));
			if ( ! file->isSimpleEventChannel(channel) )
			{
				file->read((char *)&markers[currentEvent],sizeof(markers[0])); // read first marker
				file->seekg(markerSize-1,ios_base::cur);		// skip remaining markers
			}
			currentEvent++;
		}
	}
}

void SONEventChannel::write(ostream& out) const throw (SONError)
{
	// Read info to convert from ticks to seconds
	double millisecondsPerTick = file->getSecondsPerTick() * 1000;

	if ( file->isSimpleEventChannel(channel) )
		for ( uint32_t i = 0 ; i < nEvents ; ++i ) out << time[i] * millisecondsPerTick << '\t' << 0 << '\n';
	else
		for ( uint32_t i = 0 ; i < nEvents ; ++i ) out << time[i] * millisecondsPerTick << '\t' << (int) markers[i] << '\n';
}
