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

#ifndef NCS_FILE
#define NCS_FILE

#include "customtypes.h"
#include "nlxerror.h"

#include <fstream>

using namespace std;

/**
@author Michael Zugaro
*/
class NCSFile : public ifstream
{
	friend class NCSFileCollection;

	public:
		/**
			Constructor
			@param	filename		input file name
			@param	reverse		true if signals should be reversed
		*/
		NCSFile(string filename,bool reverse = false) : filename(filename),reverse(reverse),frequency(-1),recordDuration(-1),nRecords(-1),t(-1),gap(-1) {};
		/**
			Constructor
		*/
		NCSFile() : filename(""),reverse(false),frequency(-1),recordDuration(-1),nRecords(-1),t(-1),gap(-1) {};
		/**
			Destructor
		*/
		virtual ~NCSFile() {};
		/**
			Setter
			@param	filename		input file name
			@param	reverse		true if signals should be reversed
		*/
		virtual void set(string filename,bool reverse = false) { this->filename = filename; this->reverse = reverse; };
		/**
			Open file
		*/
		virtual void open() throw(NLXError);
		/**
			Read frequency and record duration, and determine number of records
		*/
		virtual void init();
		/**
			Read next data record
			@return	false if the record could not be read (end of file)
		*/
		virtual bool readRecord() throw(NLXError);

	private:

		/**
			File name
		*/
		string		filename;
		/**
			Reverse signals?
		*/
		bool			reverse;
		/**
			File header
		*/
		char			fileHeader[NCS_FILE_HEADER_SIZE];
		/**
			Record header
		*/
		char			recordHeader[NCS_RECORD_HEADER_SIZE];
		/**
			Record contents
		*/
		Data			recordData[NCS_N_SAMPLES_PER_RECORD];
		/**
			Approximate frequency as indicated in the ncs file (as of 2011, the real frequency is 32552.0833333333...)
		*/
		Frequency	frequency;
		/**
			Record duration
		*/
		Time			recordDuration;
		/**
			Total number of records
		*/
		long			nRecords;
		/**
			Previous timestamp
		*/
		Time			t0;
		/**
			Current timestamp
		*/
		Time			t;
		/**
			Gap between current timestamp and *end* of previous record
		*/
		Time			gap;
};

#endif
