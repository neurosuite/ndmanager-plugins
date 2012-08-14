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

#include "nvtfile.h"

#include <iostream>
#include <cmath>

using namespace std;

/**
	Open file
*/
void NVTFile::open() throw(NLXError)
{
	ifstream::open(filename.c_str(),ifstream::in);
	if ( !good() ) throw NLXError(NLXError::READ_ERROR,"could not open '" + filename + "'.");
}

/**
	Read frequency and frame duration, and determine number of frames
*/
void NVTFile::init()
{
	long		before,after;

	// Get started
	clear();
	t = -1;
	frameN = -1;

	// Read file header and determine position of first frame
	seekg(0,ios::beg);
	read((char*)fileHeader,sizeof(fileHeader));
	before = tellg();

	// Determine position of end of file
	seekg(0,ios::end);
	after = tellg();

	// Compute number of frames
	nFrames = (after-before)/NVT_FRAME_SIZE;
	if ( fmod((double)after-before,(double)nFrames) != 0 ) nFrames++;

	// Return to initial position in file
	seekg(before,ios::beg);

	gap = 0;
}

/**
	Read next frame
*/
bool NVTFile::readFrame()
{
	int16_t		unused;
	int16_t		id;

	t0 = t;
	frameN++;

	read((char*)&unused,sizeof(unused));
	read((char*)&id,sizeof(id));
	read((char*)&unused,sizeof(unused));
	read((char*)&t,sizeof(t));
	read((char*)dwPoints,sizeof(dwPoints));
	read((char*)&unused,sizeof(unused));
	read((char*)&x,sizeof(x));
	read((char*)&y,sizeof(y));
	read((char*)&angle,sizeof(angle));
	read((char*)targets,sizeof(targets));

	// Determine gap between previous and current frames
	if ( t0 == -1 ) gap = 0; else gap = t-(t0+frameDuration);

	return !eof();
}

/**
	Write frame data to output file
*/
void NVTFile::writeFrame(ofstream &output)
{
	double R,G,B;

	for ( int i = 0 ; i < NVT_N_TARGETS ; ++i )
	{
		if ( targets[i] == 0 ) break;
		output << frameN << '\t';														// frame number
		output << -1 << '\t';															// dummy number of pixels in the spot
		output << (targets[i] & NVT_MASK_X) << '\t';								// x coordinate
		output << (targets[i] & NVT_MASK_Y)/NVT_SHIFT_Y << '\t';				// y coordinate
		output << -1 << '\t';															// dummy x size stdev
		output << -1 << '\t';															// dummy y size stdev
		R = (targets[i] & NVT_MASK_RED)/NVT_MASK_RED;							// red component
		G = (targets[i] & NVT_MASK_GREEN)/NVT_MASK_GREEN;						// green component
		B = (targets[i] & NVT_MASK_BLUE)/NVT_MASK_BLUE;							// blue component

		output << 16.0+65.738*R+129.057*G+25.064*B << '\t';		// Y' value
		output << 128.0+112.439*R-94.154*G-18.285*B << '\t';		// Cr value
		output << 128.0-37.945*R-74.494*G+112.439*B << endl;		// Cb value
	}
}

/**
	Write last frame (dummy data) to output file (so that we know how many frames were in the video,
	even if there is no spot in the last frame!)
*/
void NVTFile::writeLastFrame(ofstream &output)
{
	output << nFrames-1 << '\t';	// frame number
	output << -1 << '\t';		// dummy number of pixels in the spot
	output << -1 << '\t';		// dummy x coordinate
	output << -1 << '\t';		// dummy y coordinate
	output << -1 << '\t';		// dummy x size stdev
	output << -1 << '\t';		// dummy y size stdev

	output << -1 << '\t';		// dummy Y' value
	output << -1 << '\t';		// dummy Cr value
	output << -1 << endl;		// dummy Cb value
}

/**
	Show file header
*/
void NVTFile::showFileHeader()
{
	cout << "######## " << filename << endl;
	cout << fileHeader << endl;
}

