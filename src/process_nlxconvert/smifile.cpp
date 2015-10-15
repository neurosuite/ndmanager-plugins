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

#include "smifile.h"

#include <iostream>
#include <string>
#include <cmath>

using namespace std;

/**
	Open file
*/
void SMIFile::open() throw(NLXError)
{
	ifstream::open(filename.c_str(),ifstream::in);
	if ( !good() ) throw NLXError(NLXError::READ_ERROR,"could not open '" + filename + "'.");
}

/**
	Read frequency and frame duration, and determine number of frames
*/
void SMIFile::init()
{
	long		before;
	string	line;
	
	// Get started
	clear();
	t = -1;
	frameN = -1;
	
	// Skip file header and determine position of first frame
	seekg(0,ios::beg);
	while ( !eof() )
	{
		before = tellg();
		std::getline(*this,line);
		if ( line.find(SMI_MATCH_PATTERN) != string::npos ) break;
	}

	// Count frames
	nFrames = 0;
	do
	{
		if ( line.find(SMI_MATCH_PATTERN) == string::npos ) break;
		nFrames++;
		std::getline(*this,line);
	}
	while ( !eof() );
	
	// Return to initial position in file
	seekg(before,ios::beg);

	gap = 0;
}

/**
	Read next frame
*/
bool SMIFile::readFrame()// throw(NLXError)
{
	string		line;
	int			pos;
	Time			t0 = t;

	frameN++;

	// Read next line, and erase xml tags surrounding timestamp
	std::getline(*this,line);
	if ( line.find(SMI_MATCH_PATTERN) == string::npos ) return false;
	
	pos = line.find(SMI_PATTERN_BEFORE) + sizeof(SMI_PATTERN_BEFORE) - 1;
	line.erase(0,pos);
	pos = line.find(SMI_PATTERN_AFTER);
	line.erase(pos,string::npos);
	t = atoll(line.c_str());
	
	// Determine gap between previous and current frames
	if ( t0 == -1 ) gap = 0; else gap = t-(t0+frameDuration);

	return !eof();
}

