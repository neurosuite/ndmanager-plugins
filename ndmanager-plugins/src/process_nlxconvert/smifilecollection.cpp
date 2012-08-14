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

#include "smifilecollection.h"

#include <iostream>
#include <cmath>
#include <sstream>

using namespace std;

/**
	Add an input file to the collection
*/
void SMIFileCollection::add(string filename)
{
	files[n++].set(filename,frameDuration);
}

/**
	Open files
*/
void SMIFileCollection::open() throw(NLXError)
{
	for ( int i = 0 ; i < n ; ++i ) files[i].open();
}

/**
	Close files
*/
void SMIFileCollection::close()
{
	for ( int i = 0 ; i < n ; ++i ) files[i].close();
}

/**
	Read frequency and record duration, and determine number of records
*/
void SMIFileCollection::init() throw(NLXError)
{
	if ( n == 0 ) return;
	for ( int i = 0 ; i < n ; ++i ) files[i].init();
	t = -1;
}

/**
	Count the total number of records in the files
*/
long SMIFileCollection::getNFrames()
{
	long totalRecords = 0;

	for ( int i = 0 ; i < n ; ++i ) totalRecords += files[i].nFrames;
	return totalRecords;
}

/**
	Read next frame
*/
bool SMIFileCollection::readFrame()
{
	Time t0 = t;
	while ( current < n && !files[current].readFrame() ) current++;
	if ( current >= n ) return false;

	// Determine current timestamp
	t = files[current].t;
	// Gap cannot be obtained from current file because this would not work for e.g. the first
	// frame of the second file (this would yield 0, rather than the real gap between the last
	// frame of the first file and the first frame of the second file)
	if ( t0 == -1 ) gap = 0; else gap = t-(t0+frameDuration);

	return true;
}

