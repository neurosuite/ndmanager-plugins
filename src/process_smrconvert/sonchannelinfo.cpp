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

#include "sonchannelinfo.h"
#include "sonfile.h"

const int SONChannelInfo::fileHeaderSize = 512;
const int SONChannelInfo::channelHeaderSize = 140;
const int SONChannelInfo::diskBlock = 512;

SONChannelInfo::SONChannelInfo() :
file(0)
{
}

SONChannelInfo::SONChannelInfo(SONFile *file,uint16_t channel) :
file(file),
channel(channel)
{
}

SONChannelInfo::~SONChannelInfo()
{
}

void SONChannelInfo::read()
{
	char	nBytes;

	// Skip file and channel headers
	file->seekg(fileHeaderSize+channelHeaderSize*channel,ios_base::beg);

	file->read((char *)&delSize,sizeof(delSize));
	file->read((char *)&nextDelBlock,sizeof(nextDelBlock));
	file->read((char *)&fBlockOffset,sizeof(fBlockOffset));
	file->read((char *)&lBlockOffset,sizeof(lBlockOffset));
	file->read((char *)&nBlocksLSW,sizeof(nBlocksLSW));
	file->read((char *)&nExtra,sizeof(nExtra));
	file->read((char *)&preTrig,sizeof(preTrig));
	file->read((char *)&nBlocksMSW,sizeof(nBlocksMSW));
	file->read((char *)&phySz,sizeof(phySz));
	file->read((char *)&maxData,sizeof(maxData));
	file->read(&nBytes,1);
	file->read((char *)&comment,sizeof(comment)-1);
	comment[nBytes] = '\0';
	file->read((char *)&maxChanTime,sizeof(maxChanTime));
	file->read((char *)&lChanDvd,sizeof(lChanDvd));
	file->read((char *)&phyChan,sizeof(phyChan));
	file->read(&nBytes,sizeof(nBytes));
	file->read((char *)&title,sizeof(title)-1);
	title[nBytes] = '\0';
	file->read((char *)&idealRate,sizeof(idealRate));
	file->read((char *)&kind,sizeof(kind));
	file->read((char *)&pad,sizeof(pad));
	switch ( kind )
	{
		case ADC:
		case ADCMark:
			file->read((char *)&v.adc.scale,sizeof(v.adc.scale));
			file->read((char *)&v.adc.offset,sizeof(v.adc.offset));
			file->read(&nBytes,1);
			file->read((char *)&v.adc.units,sizeof(v.adc.units)-1);
			v.adc.units[nBytes] = '\0';
			file->read((char *)&v.adc.divide,sizeof(v.adc.divide));
			break;

		case EventBoth:
			file->read((char *)&v.event.initLow,sizeof(v.event.initLow));
			file->read((char *)&v.event.nextLow,sizeof(v.event.nextLow));
			break;

		case RealWave:
		case RealMark:
			file->read((char *)&v.real.min,sizeof(v.real.min));
			file->read((char *)&v.real.max,sizeof(v.real.max));
			file->read(&nBytes,sizeof(nBytes));
			file->read((char *)&v.real.units,sizeof(v.real.units)-1);
			v.real.units[nBytes] = '\0';
			break;
	}
	// Deal with different file format versions
	if ( file->getVersion() < 9 )
	{
		firstBlockOffset = fBlockOffset;
		lastBlockOffset = lBlockOffset;
		nBlocks = nBlocksLSW;
	}
	else
	{
		firstBlockOffset = (int64_t)fBlockOffset*diskBlock;
		lastBlockOffset = (int64_t)lBlockOffset*diskBlock;
		nBlocks = nBlocksLSW+65536*nBlocksMSW;
	}
}

ostream& operator<<(ostream& out,const SONChannelInfo& channelInfo)
{
	out << "Channel #                             " << channelInfo.channel << endl;
	out << "Channel kind                          ";
	switch ( channelInfo.kind )
	{
		case SONChannelInfo::ADC:
			out << "ADC" << endl;
			break;

		case SONChannelInfo::EventFall:
			out << "EventFall" << endl;
			break;

		case SONChannelInfo::EventRise:
			out << "EventRise" << endl;
			break;

		case SONChannelInfo::EventBoth:
			out << "EventBoth" << endl;
			break;

		case SONChannelInfo::Marker:
			out << "Marker" << endl;
			break;

		case SONChannelInfo::ADCMark:
			out << "ADCMark" << endl;
			out << "Number of Samples per Waveform        " << channelInfo.getNSamplesPerWaveform() << endl;
			break;

		case SONChannelInfo::RealMark:
			out << "RealMark" << endl;
			break;

		case SONChannelInfo::TextMark:
			out << "TextMark" << endl;
			break;

		case SONChannelInfo::RealWave:
			out << "RealWave" << endl;
			break;
	}
	out << "Number of blocks in deleted chain     " << channelInfo.delSize << endl;
	out << "First block in chain pointer          " << channelInfo.nextDelBlock << endl;
	out << "First block in file                   " << channelInfo.firstBlockOffset << endl;
	out << "Last block in file                    " << channelInfo.lastBlockOffset << endl;
	out << "Number of blocks                      " << channelInfo.nBlocks << endl;
	out << "Number of extra bytes                 " << channelInfo.nExtra << endl;
	if ( channelInfo.kind == SONChannelInfo::ADCMark )
		out << "Pre-trigger points                    " << channelInfo.preTrig << endl;
	out << "Physical block size (=n*512)          " << channelInfo.phySz << endl;
	out << "Maximum number of data per block      " << channelInfo.maxData << endl;
	out << "Comment                               " << channelInfo.comment << endl;
	out << "End time                              " << channelInfo.maxChanTime << endl;
	out << "Waveform divide from usPerTime        " << channelInfo.lChanDvd << endl;
	out << "Physical channel ID                   " << channelInfo.phyChan << endl;
	out << "Channel name                          " << channelInfo.title << endl;
	out << "Ideal sampling rate                   " << channelInfo.idealRate << endl;
	out << "Channel data type                     " << (int) channelInfo.kind << endl;

	switch ( channelInfo.kind )
	{
		case SONChannelInfo::ADC:
		case SONChannelInfo::ADCMark:
			out << "Scale                                 " << channelInfo.v.adc.scale << endl;
			out << "Offset                                " << channelInfo.v.adc.offset << endl;
			out << "Units                                 " << channelInfo.v.adc.units << endl;
			out << "Divide                                " << channelInfo.v.adc.divide << endl;
			break;

		case SONChannelInfo::EventBoth:
			out << "Initial event state                   " << channelInfo.v.event.initLow << endl;
			out << "Expected state of next write          " << channelInfo.v.event.nextLow << endl;
			break;

		case SONChannelInfo::RealWave:
		case SONChannelInfo::RealMark:
			out << "Expected min value                    " << channelInfo.v.real.min << endl;
			out << "Expected max value                    " << channelInfo.v.real.max << endl;
			out << "Units                                 " << channelInfo.v.real.units << endl;
			break;
	}
}

