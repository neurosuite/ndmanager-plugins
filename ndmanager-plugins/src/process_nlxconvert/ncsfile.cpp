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

#include "ncsfile.h"

#include <iostream>
#include <cmath>
#include <sstream>

using namespace std;

/**
	Open file
*/
void NCSFile::open() throw(NLXError)
{
	ifstream::open(filename.c_str(),ifstream::in);
	if ( !good() ) throw NLXError(NLXError::READ_ERROR,"error: could not open '" + filename + "'.");
}

/**
	Read frequency and record duration, and determine number of records
*/
void NCSFile::init()
{
	long where,before,after;

	// Read file header and determine position of first record
	seekg(0,ios::beg);
	read((char*)fileHeader,sizeof(fileHeader));
	before = tellg();

	// Read record header to get frequency and record duration
	read((char*)recordHeader,sizeof(recordHeader));
	frequency = ((Frequency*)recordHeader)[3];
	recordDuration = sizeof(recordData)*1e6/frequency/sizeof(Data);

	// Determine position of end of file
	seekg(0,ios::end);
	after = tellg();

	// Compute number of records
	nRecords = (after-before)/(sizeof(recordHeader)+sizeof(recordData));
	if ( fmod((float)after-before,(float)nRecords) != 0 ) nRecords++;

/*	// Start from record number -1, i.e. read first record and move t by -recordDuration
	seekg(before,ios::beg);
	readRecord();
	t = t - recordDuration;
	gap = 0;*/

	// Return to initial position in file
	seekg(before,ios::beg);
}

/**
	Read next record
*/
bool NCSFile::readRecord() throw(NLXError)
{
	read((char*)recordHeader,sizeof(recordHeader));
	read((char*)recordData,sizeof(recordData));

	// Reverse if necessary
	if ( reverse ) for ( int i = 0 ; i < sizeof(recordData)/sizeof(Data) ; ++i ) recordData[i] = -recordData[i];

	// Set timestamp, and determine gap between previous and current records
	t0 = t;
	t = *((Time*)recordHeader);
	if ( t0 == -1 ) gap = 0; else gap = t-(t0+recordDuration);

	// Check that frequency is consistent
	Frequency f = ((Frequency*)recordHeader)[3];
	if ( fabs(f-frequency) > NCS_MAX_FREQUENCY_ERROR )
	{
		ostringstream message;
		message.setf(ios::fixed,ios::floatfield);
		message.precision(6);
		message << "file " << filename << " has an inconsistent sampling frequency (" << f << " vs " << frequency << ") @ t = " << t/1e6 << " s.";
		throw NLXError(NLXError::READ_ERROR,message.str());
	}

	return !eof();
}



