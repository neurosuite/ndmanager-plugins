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

#ifndef NVT_FILE
#define NVT_FILE

#include <fstream>

#include "customtypes.h"
#include "nlxerror.h"

using namespace std;

/**
@author Michael Zugaro
*/
class NVTFile : public ifstream
{
	public:
		/**
			Constructor
			@param	filename				input file name
			@param	frameDuration		frame duration in µs
		*/
		NVTFile(string filename,Time frameDuration) : filename(filename),frameDuration(frameDuration),nFrames(-1),t(-1),gap(-1),frameN(-1) {};
		/**
			Constructor
		*/
		NVTFile() : filename(""),frameDuration(-1),nFrames(-1),t(-1),gap(-1),frameN(-1) {};
		/**
			Destructor
		*/
		virtual ~NVTFile() {};
		/**
			Setter
			@param	filename		input file name
			@param	frameDuration		theoretical frame duration
		*/
		virtual void set(string filename,Time frameDuration) { this->filename = filename; this->frameDuration = frameDuration; };
		/**
			Open file
			@return	false if the file could not be opened
		*/
		virtual void open() throw(NLXError);
		/**
			Read frequency and frame duration, and determine number of frames
		*/
		virtual void init();
		/**
			Read next frame
			@return	false if the frame could not be read (end of file)
		*/
		virtual bool readFrame();
		/**
			Write frame data to output file
		*/
		virtual void writeFrame(ofstream &output);
		/**
			Write last frame (dummy data) to output file (so that we know how many frames were in the video,
			even if there is no spot in the last frame!)
		*/
		virtual void writeLastFrame(ofstream &output);
		/**
			Count the number of frames in the file
			@return	number of frames
		*/
		virtual long getNFrames() { return nFrames; };
		/**
			Get current gap
			@return	current gap in µs
		*/
		virtual Time getGap() { return gap; };
		/**
			Get current timestamp
			@return	current timestamp
		*/
		virtual Time getTimestamp() { return t; };
		/**
			Show file header
		*/
		virtual void showFileHeader();

	private:

		/**
			Filename
		*/
		string		filename;
		/**
			File header
		*/
		char			fileHeader[NVT_FILE_HEADER_SIZE];
		/**
			Frame duration provided on the command-line (this information is not provided in the nvt file)
		*/
		Time			frameDuration;
		/**
			Total number of frames
		*/
		long			nFrames;
		/**
			Previous timestamp
		*/
		Time			t0;
		/**
			Current timestamp
		*/
		Time			t;
		/**
			Gap between current timestamp and *end* of previous fame
		*/
		Time			gap;
		/**
			Current frame contents
		*/
		Coordinates	dwPoints[NVT_N_DWPOINTS],x,y,angle,targets[NVT_N_TARGETS];
		/**
			Current frame number
		*/
		int			frameN;
};

#endif
