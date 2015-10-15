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

#ifndef NEV_FILE
#define NEV_FILE

#include "customtypes.h"
#include "nlxerror.h"

#include <fstream>

using namespace std;

/**
@author Michael Zugaro
*/
class NEVFile : public ifstream
{
	public:
		/**
			Constructor
			@param	filename		input file name
		*/
		NEVFile(string filename) : filename(filename),nRecords(-1),t(-1),gap(-1),shift(0) {};
		/**
			Constructor
		*/
		NEVFile() : filename(""),nRecords(-1),t(-1),gap(-1),shift(0) {};
		/**
			Destructor
		*/
		virtual ~NEVFile() {};
		/**
			Setter
			@param	filename		input file name
		*/
		virtual void set(string filename) { this->filename = filename; };
		/**
			Open file
			@return	false if the file could not be opened
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
		virtual bool readRecord();
		/**
			Test whether acquisition was restarted
		*/
		virtual bool restarted();
		/**
			Write record data to output file
		*/
		virtual void writeRecord(ofstream &output);
		/**
			Set initial shift from 0, to synchronize with wideband signals
		*/
		virtual void setShift(Time s);
		/**
			Add shift, to synchronize with wideband signals
		*/
		virtual void addToShift(Time s);
		/**
			Count the number of records in the file
			@return	number of records
		*/
		virtual long getNRecords() { return nRecords; };
		/**
			Get current timestamp
			@return	current timestamp
		*/
		virtual Time getTimestamp() { return t; };
		/**
			Get time gap between current and previous events
			@return	time gap
		*/
		virtual Time getGap() { return gap; };
		/**
			Show file header
		*/
		virtual void showFileHeader();

	private:

		/**
			File name
		*/
		string		filename;
		/**
			File header
		*/
		char			fileHeader[NEV_FILE_HEADER_SIZE];
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
		/**
			Correction for time shift(s) between nev and ncs files
		*/
		Time			shift;
		/**
			Current record contents
		*/
		char			event[128];
		/**
			Previous record contents
		*/
		char			previousEvent[128];
};

#endif
