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

#include "sonfile.h"
#include "sonadcchannels.h"
#include "sonblockheaders.h"

extern bool verbose;

const int SONADCChannels::blockHeaderSize = 20;

SONADCChannels::SONADCChannels(SONFile *file,uint16_t nChannels,uint16_t *channels,int32_t nBufferSamples) :
file(file),
nChannels(nChannels),
channels(0),
bof(true),
samplingInterval(0),
currentBlock(0),
currentSampleInBlock(0),
lastBlock(0),
lastSampleInLastBlock(0),
currentSampleInBuffer(0),
nBufferSamples(nBufferSamples),
nSamplesRead(0),
buffer(0),
multiplexed(0)
{
	this->channels = new uint16_t[nChannels];
	memcpy(this->channels,channels,nChannels*sizeof(channels[0]));
	currentBlock = new uint32_t[nChannels];
	currentSampleInBlock = new uint16_t[nChannels];
	lastBlock = new uint32_t[nChannels];
	lastSampleInLastBlock = new uint16_t[nChannels];
	buffer = new int16_t[nBufferSamples*nChannels];
	memset(buffer,0,nBufferSamples*nChannels*sizeof(buffer[0]));
	if ( nChannels > 1 ) multiplexed = new int16_t[nBufferSamples*nChannels];
	nSamplesRead = new int32_t[nChannels];
}

SONADCChannels::~SONADCChannels()
{
	delete[] channels;
	delete[] currentBlock;
	delete[] currentSampleInBlock;
	delete[] lastBlock;
	delete[] lastSampleInLastBlock;
	delete[] nSamplesRead;
	delete[] buffer;
	delete[] multiplexed;
}

bool SONADCChannels::done() const
{
	if ( bof ) return false;
	for ( uint16_t i = 0 ; i < nChannels ; ++i ) if ( currentBlock[i] < lastBlock[i] || (currentBlock[i] == lastBlock[i] && currentSampleInBlock[i] < lastSampleInLastBlock[i]) ) return false;
	return true;
}

void SONADCChannels::read() throw (SONError)
{
	// Make sure all channels are ADC channels and have the same sampling rate, and initialize all pointers
	if ( bof )
	{
		for ( uint16_t i = 0 ; i < nChannels ; ++i )
		{
			if ( !file->isADCChannel(channels[i]) )
			{
				stringstream ss;
				ss << channels[i];
				string channelStr;
				ss >> channelStr;
				string message = "Channel " + channelStr + " is not an ADC channel";
				throw SONError(SONError::WRONG_CHANNEL_TYPE,message);
			}
			if ( !verbose ) cout << channels[i] << " " << flush;
			uint16_t si = file->getTicksPerSI(channels[i]);

			if ( verbose ) cout << "DATA [" << channels[i] << "]\tsampling interval = " << si << endl;
			if ( i == 0 ) samplingInterval = si;
			else if ( si != samplingInterval )
			{
				string error = "";
				stringstream ss;
				string siStr,channelStr,samplingIntervalStr;
				ss << si;
				ss >> siStr;
				ss << channels[i];
				ss >> channelStr;
				ss << samplingInterval;
				ss >> samplingIntervalStr;
				error += "Incompatible sampling intervals: " + siStr + " for channel " + channelStr + " (should be " + samplingIntervalStr + ")";
				throw SONError(SONError::READ_ERROR,error);
			}

			file->readBlockHeaders(channels[i]);
			lastBlock[i] = file->getNBlocks(channels[i])-1;
			lastSampleInLastBlock[i] = file->getNSamplesInBlock(channels[i],lastBlock[i])-1;
			currentBlock[i] = 0;
			currentSampleInBlock[i] = 0;
			if ( verbose ) cout << "  \tblock size = " << file->getNSamplesInBlock(channels[i],0) << endl;
			if ( verbose ) cout << "  \t# samples = " << file->getNSamples(channels[i]) << "  \t# blocks = " << lastBlock[i]+1 << "  \tblock size = " << file->getNSamplesInBlock(channels[i],0) << "  \t# samples in last block = " << lastSampleInLastBlock[i]+1 << endl;
		}
		bof = false;
	}

	// Clear buffer
	currentSampleInBuffer = 0;
	memset(buffer,0,nChannels*nBufferSamples*sizeof(buffer[0]));
	memset(nSamplesRead,0,nChannels*sizeof(nSamplesRead[0]));

	// Read one block of data
	for ( uint16_t i = 0 ; i < nChannels ; ++i )
	{
		int32_t		nSamplesLeft = nBufferSamples;
		int32_t		start = currentSampleInBuffer;
		while ( nSamplesLeft > 0 )
		{
			// Are we at EOF?
			if ( currentBlock[i] > lastBlock[i] ) break;

			// How many samples should be read from this block?
			uint16_t		nBlockSamples = file->getNSamplesInBlock(channels[i],currentBlock[i]);
			uint16_t		nSamplesLeftInBlock = nBlockSamples - currentSampleInBlock[i];
			uint16_t		nSamplesToRead;
			if ( nSamplesLeftInBlock < nSamplesLeft ) nSamplesToRead = nSamplesLeftInBlock;
			else nSamplesToRead = nSamplesLeft;

			if ( verbose ) cout << "DATA [" << channels[i] << "]  \t# samples left = " << nSamplesLeft << "  \t# samples in block[" << currentBlock[i] << "] = " << nSamplesToRead << "/" << nBlockSamples << "  \tstarting @ " << currentSampleInBuffer << endl;

			// Read those samples and update variables
			file->seekg(file->getBlockOffset(channels[i],currentBlock[i])+blockHeaderSize+currentSampleInBlock[i]*sizeof(buffer[0]),ios_base::beg);
			file->read((char *)&buffer[currentSampleInBuffer],nSamplesToRead*sizeof(buffer[0]));
			currentSampleInBlock[i] += nSamplesToRead;
			currentSampleInBuffer += nSamplesToRead;
			nSamplesLeftInBlock -= nSamplesToRead;
			nSamplesLeft -= nSamplesToRead;
			if ( nSamplesLeftInBlock == 0 )
			{
				currentBlock[i]++;
				currentSampleInBlock[i] = 0;
			}
		}
		nSamplesRead[i] = currentSampleInBuffer-start;

		if ( verbose ) cout << "DATA [" << channels[i] << "]  \t# samples read = " << nSamplesRead[i] << endl;
	}
// cout << "---" << endl;
}

void SONADCChannels::write(ostream& out,bool reverse) const throw (SONError)
{
	int sign = 1;

	if ( reverse ) sign = -1;

	if ( nChannels > 1 )
	{
		int32_t padLength = 0;

		// Rearrange the channels (sequentially -> multiplexed)

		int32_t cumulativeNSamplesRead[nChannels],maxNSamplesRead = 0;
		cumulativeNSamplesRead[0] = 0;
		for ( uint16_t i = 1 ; i < nChannels ; ++i ) cumulativeNSamplesRead[i] = cumulativeNSamplesRead[i-1]+nSamplesRead[i-1];
		if ( verbose ) cout << "WRITE # samples = " << currentSampleInBuffer << endl;

		// At the very end, this will be required to pad with zeros if necessary
		if ( done() ) for ( uint16_t i = 0 ; i < nChannels ; ++i ) if ( nSamplesRead[i] > maxNSamplesRead ) maxNSamplesRead = nSamplesRead[i];

		for ( uint16_t i = 0 ; i < nChannels ; ++i )
		{
			// Write samples100
			for ( uint16_t j = 0 ; j < nSamplesRead[i] ; ++j ) multiplexed[j*nChannels+i] = sign*buffer[cumulativeNSamplesRead[i]+j];

			// At the very end, pad with zeros if necessary
			if ( done() )
			{
				int32_t nZeros = maxNSamplesRead-nSamplesRead[i];
				if ( nZeros != 0 )
				{
					if ( verbose ) cout << "WRITE Padding [" << channels[i] << "] with " << nZeros << " zeros." << endl;
					for ( int32_t j = nSamplesRead[i] ; j < maxNSamplesRead ; ++j )
					{
						multiplexed[j*nChannels+i] = 0;
						++padLength;
					}
				}
			}
		}

		// Write to stream
		out.write((char *)multiplexed,(currentSampleInBuffer+padLength)*sizeof(multiplexed[0]));
	}
	else
	{
		if ( verbose ) cout << "WRITE [" << channels[0] << "] # samples " << currentSampleInBuffer << endl;
		out.write((char *)buffer,currentSampleInBuffer*sizeof(buffer[0]));
	}
}
