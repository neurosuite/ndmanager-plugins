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

#include "sonfile.h"
#include "sonadcchannels.h"
#include "sonadcmarkchannel.h"
#include "soneventchannel.h"
#include "sonerror.h"

extern bool verbose;

SONFile::SONFile(string filename) :
filename(filename),
fileHeader(0),
channelInfo(0),
blockHeaders(0),
nADCChannels(0),
nADCMarkChannels(0),
nEventChannels(0)
{
}

SONFile::~SONFile()
{
	delete fileHeader;
	delete[] channelInfo;
	delete[] blockHeaders;
}

void SONFile::open() throw(SONError)
{
	ifstream::open(filename.c_str(),ios::in);
	if ( !good() ) throw SONError(SONError::READ_ERROR,"'" + filename + "' not found. Aborting.");
}

void SONFile::init() throw(SONError)
{
	fileHeader = new SONFileHeader(this);
	fileHeader->read();

	uint16_t nChannels = fileHeader->getNChannels();

	channelInfo = new SONChannelInfo[nChannels];
	for ( uint16_t i = 0 ; i < nChannels ; ++i )
	{
		channelInfo[i].init(this,i);
		channelInfo[i].read();
	}

	blockHeaders = new SONBlockHeaders[nChannels];
	for ( uint16_t i = 0 ; i < nChannels ; ++i ) blockHeaders[i].init(this,i);
}
