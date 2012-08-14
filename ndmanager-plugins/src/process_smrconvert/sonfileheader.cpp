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

#include "sonfileheader.h"
#include "sonfile.h"

SONFileHeader::SONFileHeader(SONFile *file) :
file(file)
{
	for ( int i = 0 ; i  < 5 ; ++i ) fileComment[i] = 0;
}

SONFileHeader::~SONFileHeader()
{
	for ( int i = 0 ; i  < 5 ; ++i ) delete[] fileComment[i];
}

void SONFileHeader::read()
{
	streampos		pointer;
	unsigned char	nBytes;

	file->seekg(0,ios::beg);

	file->read((char *)&systemID,sizeof(systemID));
	file->read(copyright,sizeof(copyright)-1);
	copyright[sizeof(copyright)-1] = '\0';
	file->read(creator,sizeof(creator)-1);
	creator[sizeof(creator)-1] = '\0';
	file->read((char *)&usPerTime,sizeof(usPerTime));
	file->read((char *)&timePerADC,sizeof(timePerADC));
	file->read((char *)&fileState,sizeof(fileState));
	file->read((char *)&firstData,sizeof(firstData));
	file->read((char *)&nChannels,sizeof(nChannels));
	file->read((char *)&channelSize,sizeof(channelSize));
	file->read((char *)&extraData,sizeof(extraData));
	file->read((char *)&bufferSize,sizeof(bufferSize));
	file->read((char *)&osFormat,sizeof(osFormat));
	file->read((char *)&maxFTime,sizeof(maxFTime));
	file->read((char *)&timeBase,sizeof(timeBase));
	file->read(timeDate.detail,sizeof(timeDate.detail));
	file->read((char *)&timeDate.year,sizeof(timeDate.year));
	file->read(pad,sizeof(pad));

	pointer = file->tellg();
	for ( int i = 0 ; i  < 5 ; ++i )
	{
		file->read((char *)&nBytes,sizeof(nBytes));
		fileComment[i] = new char[nBytes+1];
		file->read(fileComment[i],nBytes);
		fileComment[i][nBytes] = '\0';
		pointer += 80;
		file->seekg(pointer,ios_base::beg);
	}
}

ostream& operator<<(ostream& out,const SONFileHeader& fileHeader)
{
	out << "System ID        " << fileHeader.systemID << endl;
	out << "Copyright        " << fileHeader.copyright << endl;
	out << "Creator          " << fileHeader.creator << endl;
	out << "Us per time      " << fileHeader.usPerTime << endl;
	out << "Time per ADC     " << fileHeader.timePerADC << endl;
	out << "File state       " << fileHeader.fileState << endl;
	out << "First data       " << fileHeader.firstData << endl;
	out << "Channels         " << fileHeader.nChannels << endl;
	out << "Channel size     " << fileHeader.channelSize << endl;
	out << "Extra data       " << fileHeader.extraData << endl;
	out << "Buffer size      " << fileHeader.bufferSize << endl;
	out << "OS format        " << fileHeader.osFormat << endl;
	out << "Max F time       " << fileHeader.maxFTime << " (" << fileHeader.maxFTime*fileHeader.getSecondsPerTick() << " s)" << endl;
	out << "D time base      " << fileHeader.timeBase << endl;
	out << "Date-detail     ";
	for ( int i = 0 ; i  < 6 ; ++i ) out << " " << (unsigned char) fileHeader.timeDate.detail[i];
	out << endl;
	out << "Date-year        " << fileHeader.timeDate.year << endl;
	out << "COMMENTS         [1] " << fileHeader.fileComment[0] << endl;
	for ( int i = 1 ; i  < 5 ; ++i ) out << "                 [" << i+1 << "] " << fileHeader.fileComment[i] << endl;
}

