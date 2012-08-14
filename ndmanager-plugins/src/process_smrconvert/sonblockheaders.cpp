/***************************************************************************
 *                                                                         *
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
#include <sstream>

#include "sonblockheaders.h"
#include "sonfile.h"

const int SONBlockHeaders::diskBlock = 512;

extern bool verbose;

SONBlockHeaders::SONBlockHeaders() :
file(0),
channel(0),
nBlocks(0),
currentBlock(0),
blockOffset(0),
nextBlockOffset(0),
start(0),
stop(0),
nSamplesInBlock(0),
nSamples(0)
{
}

SONBlockHeaders::SONBlockHeaders(SONFile *file,uint16_t channel) :
file(file),
channel(channel),
nBlocks(0),
currentBlock(0),
blockOffset(0),
nextBlockOffset(0),
start(0),
stop(0),
nSamplesInBlock(0),
nSamples(0)
{
}

SONBlockHeaders::~SONBlockHeaders()
{
	delete[] blockOffset;
	delete[] nextBlockOffset;
	delete[] start;
	delete[] stop;
	delete[] nSamplesInBlock;
}

void SONBlockHeaders::readNext() throw (SONError)
{
	if ( currentBlock == 0 )
	{
		delete[] blockOffset;
		delete[] nextBlockOffset;
		delete[] start;
		delete[] stop;
		delete[] nSamplesInBlock;
		blockOffset = new int64_t[nBlocks];
		nextBlockOffset = new int64_t[nBlocks];
		start = new int32_t[nBlocks];
		stop = new int32_t[nBlocks];
		nSamplesInBlock = new uint16_t[nBlocks];
		nBlocks = file->getNBlocks(channel);
		blockOffset[0] = file->getFirstBlockOffset(channel);
		nSamples = 0;
	}
	else
	{
		blockOffset[currentBlock] = nextBlockOffset[currentBlock-1];
	}

	int32_t		pBlockOffset;
	uint16_t		channel;

	file->seekg(blockOffset[currentBlock],ios_base::beg);
	file->read((char *)&pBlockOffset,sizeof(pBlockOffset));	// unused
	file->read((char *)&nBlockOffset,sizeof(nBlockOffset));
	// Deal with different file format versions
	if ( file->getVersion() < 9 )
	{
		nextBlockOffset[currentBlock] = nBlockOffset;
	}
	else
	{
		nextBlockOffset[currentBlock] = ((int64_t)nBlockOffset)*((int64_t)diskBlock);
	}
	file->read((char *)&start[currentBlock],sizeof(start[0]));
	file->read((char *)&stop[currentBlock],sizeof(stop[0]));
	file->read((char *)&channel,sizeof(channel));
	file->read((char *)&nSamplesInBlock[currentBlock],sizeof(nSamplesInBlock[currentBlock]));
	nSamples += nSamplesInBlock[currentBlock];
	if ( channel-1 != this->channel )
	{
		stringstream ss;
		ss << this->channel;
		string channelStr;
		ss >> channelStr;
		string message = "Error while reading channel " + channelStr;
		throw SONError(SONError::READ_ERROR,message);
	}

	if ( verbose ) cout << "INFO [" << this->channel << "]  \t# block[" << currentBlock << "] @ " << blockOffset[currentBlock] << " (" << nSamplesInBlock[currentBlock] << " samples), next " << nextBlockOffset[currentBlock] << endl;

	currentBlock++;
}

void SONBlockHeaders::read() throw (SONError)
{
	nBlocks = file->getNBlocks(channel);
	currentBlock = 0;
	for ( uint32_t block = 0 ; block < nBlocks ; ++block ) readNext();
}
