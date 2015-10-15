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

#include "nevfile.h"

#include <iostream>
#include <cmath>
#include <cstring>

using namespace std;

/**
	Open file
*/
void NEVFile::open() throw(NLXError)
{
	ifstream::open(filename.c_str(),ifstream::in);
	if ( !good() ) throw NLXError(NLXError::READ_ERROR,"could not open '" + filename + "'.");
}

/**
	Read frequency and record duration, and determine number of records
*/
void NEVFile::init()
{
	long before,after;

	// Get started
	clear();
	event[0] = 0;
	previousEvent[0] = 0;
	t = -1;

	// Read file header and determine position of first record
	seekg(0,ios::beg);
	read((char*)fileHeader,sizeof(fileHeader));
	before = tellg();

	// Determine position of end of file
	seekg(0,ios::end);
	after = tellg();

	// Compute number of records
	nRecords = (after-before)/NEV_RECORD_SIZE;

	if ( fmod((float)after-before,(float)nRecords) != 0 ) nRecords++;

	// Return to initial position in file
	seekg(before,ios::beg);
}

/**
	Read next record
*/
bool NEVFile::readRecord()
{
	int16_t		unused1[3],unused2[3];
	int32_t		unused3[8];
	int16_t		id;
	int16_t		ttl;

	t0 = t;
	memcpy(previousEvent,event,sizeof(event));

	read((char*)unused1,sizeof(unused1));
	read((char*)&t,sizeof(t));
	if ( t0 == -1 ) gap = 0; else gap = (t-t0);
	read((char*)&id,sizeof(id));
	read((char*)&ttl,sizeof(ttl));
	read((char*)unused2,sizeof(unused2));
	read((char*)unused3,sizeof(unused3));
	read((char*)event,sizeof(event));

	return !eof();
}

/**
	Test whether acquisition was restarted
*/
bool NEVFile::restarted()
{
	return ( strcmp(previousEvent,"Stopping Recording") == 0 && strcmp(event,"Starting Recording") == 0 );
}

/**
	Set initial shift from 0, to synchronize with wideband signals
*/
void NEVFile::setShift(Time s)
{
	shift = s;
}

/**
	Add shift, to synchronize with wideband signals
*/
void NEVFile::addToShift(Time s)
{
	shift += s;
}

/**
	Write record data to output file
*/
void NEVFile::writeRecord(ofstream &output)
{
	output << (t-shift)/1000.0 << " " << event << endl;
}

/**
	Show file header
*/
void NEVFile::showFileHeader()
{
	if ( good() )
	{
		cout << "######## " << filename << endl;
		cout << fileHeader << endl;
	}
}


