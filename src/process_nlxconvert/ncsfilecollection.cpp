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

#include "ncsfilecollection.h"

#include <iostream>
#include <cmath>
#include <cstring>
#include <limits>
#include <sstream>

using namespace std;

/**
	Add an input file to the collection
*/
void NCSFileCollection::add(string filename)
{
	files[n++].set(filename,reverse);
}

/**
	Open files
*/
void NCSFileCollection::open() throw(NLXError)
{
	for ( int i = 0 ; i < n ; ++i ) files[i].open();
}

/**
	Close files
*/
void NCSFileCollection::close()
{
	for ( int i = 0 ; i < n ; ++i ) files[i].close();
}

/**
	Read frequency and record duration, and determine number of records
*/
void NCSFileCollection::init() throw(NLXError)
{
	Frequency f;

	if ( n == 0 ) return;

	// Initialize all files
	for ( int i = 0 ; i < n ; ++i ) files[i].init();

	recordDuration = files[0].recordDuration;
	// Check frequency consistency across files
	f = files[0].frequency;
	for ( int i = 1 ; i < n ; ++i )
		if ( fabs(f-files[i].frequency) > NCS_MAX_FREQUENCY_ERROR )
		{
			ostringstream message;
			message.setf(ios::fixed,ios::floatfield);
			message.precision(6);
			message << "file " << i << " has a different sampling frequency (" << files[i].frequency << " vs " << f << ").";
			throw NLXError(NLXError::READ_ERROR,message.str());
		}
}

/**
	Count the maximum number of records in the files
*/
long NCSFileCollection::getNRecords()
{
	long minRecords = std::numeric_limits<long>::max();

	for ( int i = 0 ; i < n ; ++i ) if ( files[i].nRecords < minRecords ) minRecords = files[i].nRecords;
	return minRecords;
}

/**
	Read next record
*/
bool NCSFileCollection::readRecord()
{
	// Read next record for all files
	for ( int i = 0 ; i < n ; ++i )
	{
		if ( files[i].gap < recordDuration )
		{
			// Read next record for this file
			if ( !files[i].readRecord() ) return false;
		}
		else
		{
			// This file is 'waiting' for the others, so the next record should not be read yet
			// Instead, waiting time is decremented (one less record to wait)
			files[i].gap -= recordDuration;
		}
	}

	// Determine current timestamp: use minimum t across individual files
	t0 = t;
	t = std::numeric_limits<Time>::max();
	for ( int i = 0 ; i < n ; ++i ) if ( files[i].t < t ) t = files[i].t;
	if ( t0 == -1 )
	{
		// First record (special case): update individual gaps accordingly
		// [This cannot be done by each ncs file object separately, because we are actually synchronizing them]
		for ( int i = 0 ; i < n ; ++i ) files[i].gap = files[i].t - t;
	}
	// Determine minimum gap across files
	minGap = std::numeric_limits<Time>::max();
	for ( int i = 0 ; i < n ; ++i ) if ( files[i].gap < minGap ) minGap = files[i].gap;

	return true;
}

/**
	Skip missing records
*/
void NCSFileCollection::skipMissingRecords()
{
	for ( int i = 0 ; i < n ; ++i ) files[i].gap -= minGap;
}

/**
	Fill missing records
*/
void NCSFileCollection::fillMissingRecords(ofstream &output)
{
	int k = n*NCS_N_SAMPLES_PER_RECORD; // number of samples per output record
	memset(buffer,0,k*sizeof(Data));

	// Write zeros to output file
	for ( int i = 0 ; i < minGap/recordDuration ; ++i ) output.write((char*)buffer,k*sizeof(Data));
	// Update gaps
	for ( int i = 0 ; i < n ; ++i ) files[i].gap -= minGap;
}

/**
	Write record data to output file
*/
void NCSFileCollection::writeRecord(ofstream &output)
{
	// Write data to output file; for 'waiting' input files, output zeros
	int k = 0;
	for ( int j = 0 ; j < NCS_N_SAMPLES_PER_RECORD ; ++j )
		for ( int i = 0 ; i < n ; ++i )
			if ( files[i].gap > recordDuration ) buffer[k++] = 0;
			else buffer[k++] = files[i].recordData[j];

	output.write((char*)buffer,k*sizeof(Data));
}

/**
	Show file headers
*/
void NCSFileCollection::showFileHeaders()
{
	for ( int i = 0 ; i < n ; ++i )
	{
		cout << "######## " << files[i].filename << endl;
		cout << files[i].fileHeader << endl;
	}
}

