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

#ifndef SMI_FILE
#define SMI_FILE

#include <fstream>

#include "customtypes.h"
#include "nlxerror.h"

using namespace std;

/**
@author Michael Zugaro
*/
class SMIFile : public ifstream
{
	friend class SMIFileCollection;
	
	public:
		/**
			Constructor
			@param	filename				input file name
			@param	frameDuration		frame duration in µs
		*/
		SMIFile(string filename,Time frameDuration) : filename(filename),frameDuration(frameDuration),nFrames(-1),t(-1),gap(-1),frameN(-1) {};
		/**
			Constructor
		*/
		SMIFile() : filename(""),frameDuration(-1),nFrames(-1),t(-1),gap(-1),frameN(-1) {};
		/**
			Destructor
		*/
		virtual ~SMIFile() {};
		/**
			Setter
			@param	filename				input file name
			@param	frameDuration		frame duration in µs
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

	private:

		string		filename;
		Time			frameDuration;
		long			nFrames;
		Time			t;
		Time			gap;
		int			frameN;
};

#endif
