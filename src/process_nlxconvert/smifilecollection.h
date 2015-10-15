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

#ifndef SMI_FILE_COLLECTION
#define SMI_FILE_COLLECTION

#include "customtypes.h"
#include "nlxerror.h"
#include "smifile.h"

#include <fstream>

using namespace std;

/**
@author Michael Zugaro
*/
class SMIFileCollection
{
	public:
		/**
			Constructor
			@param	frameDuration		frame duration in µs
		*/
		SMIFileCollection(Time frameDuration) : frameDuration(frameDuration),n(0),current(0),t(-1),gap(-1) { files = new SMIFile[SMI_MAX_N_FILES]; };
		/**
			Constructor
		*/
		SMIFileCollection() : frameDuration(-1),n(0),current(0),t(-1),gap(-1) { files = new SMIFile[SMI_MAX_N_FILES]; };
		/**
			Destructor
		*/
		virtual ~SMIFileCollection() { delete[] files; };
		/**
			Setter
			@param	frameDuration		frame duration in µs
		*/
		virtual void set(Time frameDuration) { this->frameDuration = frameDuration; };
		
		/**
			Add an input file to the collection
			@param	filename name of the file to add
		*/
		virtual void add(string filename);
		/**
			Open files
			@return	false if one of the files could not be opened
		*/
		virtual void open() throw(NLXError);
		/**
			Close files
		*/
		virtual void close();
		/**
			Determine total number of records
		*/
		virtual void init() throw(NLXError);
		/**
			Count the total number of frames in the files
			@return	number of frames
		*/
		virtual long getNFrames();
		/**
			Read next frame
			@return	false if the frame could not be read (end of last file)
		*/
		virtual bool readFrame();
		/**
			Get current timestamp
			@return	current timestamp
		*/
		virtual Time getTimestamp() { return t; };
		/**
			Get current gap
			@return	current gap in µs
		*/
		virtual Time getGap() { return gap; };

	private:

		Time			frameDuration;
		SMIFile		*files;
		int			n;
		int			current;
		Time			t;
		Time			gap;
};

#endif
