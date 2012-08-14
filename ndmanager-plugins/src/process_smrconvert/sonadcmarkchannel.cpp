/***************************************************************************
 *                                                                         *
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

#include <sstream>

#include "sonfileheader.h"
#include "sonadcmarkchannel.h"
#include "sonblockheaders.h"
#include "sonfile.h"

SONADCMarkChannel::SONADCMarkChannel(SONFile *file,uint16_t channel) :
SONChannel(file,channel),
nWaveforms(0),
nSamplesPerWaveform(0),
waveform(0),
time(0)
{
}

SONADCMarkChannel::~SONADCMarkChannel()
{
	delete[] waveform;
	delete[] time;
}

void SONADCMarkChannel::read() throw (SONError)
{
	if ( !file->isADCMarkChannel(channel) )
	{
		stringstream ss;
		ss << this->channel;
		string channelStr;
		ss >> channelStr;
		string	error = "Channel " + channelStr + " is not an ADC Mark channel";
		throw SONError(SONError::WRONG_CHANNEL_TYPE,error);
	}

	// Allocate memory

	nWaveforms = file->getNSamples(channel);
	nSamplesPerWaveform = file->getNSamplesPerWaveform(channel);

	delete[] waveform;
	delete[] time;
	waveform = new int16_t[nWaveforms*nSamplesPerWaveform];
	file->readBlockHeaders(channel);

	uint16_t ticksPerSI = file->getTicksPerSI(channel);

	time = new int32_t[nWaveforms];

	uint16_t		currentWaveform = 0;
	uint32_t		nBlocks = file->getNBlocks(channel);

	// Read data
	for ( uint32_t currentBlock = 0 ; currentBlock < nBlocks ; ++currentBlock )
	{
		// How many waveforms should be read from this block?
		uint16_t	nBlockWaveforms = file->getNSamplesInBlock(channel,currentBlock);

		// Read and update variables
		file->seekg(file->getBlockOffset(channel,currentBlock)+blockHeaderSize,ios_base::beg);
		for ( uint16_t j = 0 ; j < nBlockWaveforms ; ++j )
		{
			file->read((char *)&time[currentWaveform],sizeof(time[0]));
			time[currentWaveform] = time[currentWaveform]/ticksPerSI;
			file->seekg(markerSize,ios_base::cur);		// skip markers
			file->read((char *)&waveform[currentWaveform*nSamplesPerWaveform],nSamplesPerWaveform*sizeof(waveform[0]));
			currentWaveform++;
		}
	}
}

void SONADCMarkChannel::write(ostream& times,ostream& waveforms,bool reverse) const throw (SONError)
{
	if ( reverse ) for ( int32_t i = 0 ; i < nWaveforms*nSamplesPerWaveform ; ++i ) waveform[i] = -waveform[i];

	waveforms.write((char *)waveform,nWaveforms*nSamplesPerWaveform*sizeof(waveform[0]));
	for ( int32_t i = 0 ; i < nWaveforms ; ++i ) times << time[i] << '\n';
}
