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

#ifndef NCS_FILE_COLLECTION
#define NCS_FILE_COLLECTION

#include "customtypes.h"
#include "nlxerror.h"
#include "ncsfile.h"

#include <fstream>

using namespace std;

/**
@author Michael Zugaro
*/
class NCSFileCollection
{
	public:
		/**
			Constructor
		*/
		NCSFileCollection(bool reverse = false) : n(0),reverse(reverse),recordDuration(-1),t(-1),minGap(-1) { files = new NCSFile[NCS_MAX_N_CHANNELS]; };
		/**
			Destructor
		*/
		virtual ~NCSFileCollection() { delete[] files; };
		/**
			Setter
			@param	reverse true if signals should be reversed
		*/
		virtual void set(bool reverse) { this->reverse = reverse; };
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
			Read frequency and record duration, and determine number of records
		*/
		virtual void init() throw(NLXError);
		/**
			Count the number of records in the files
			@return	number of records
		*/
		virtual long getNRecords();
		/**
			Read next data record
			@return	false if the record could not be read (end of file)
		*/
		virtual bool readRecord();
		/**
			Skip missing records
		*/
		virtual void skipMissingRecords();
		/**
			Fill missing records
		*/
		virtual void fillMissingRecords(ofstream &output);
		/**
			Write record data to output file
		*/
		virtual void writeRecord(ofstream &output);
		/**
			Show file headers
		*/
		virtual void showFileHeaders();
		/**
			Get current timestamp
			@return	current timestamp
		*/
		virtual Time getTimestamp() { return t; };
		/**
			Get record duration
			@return	current record duration µs
		*/
		virtual Time getRecordDuration() { return recordDuration; };
		/**
			Get current gap
			@return	current gap in µs
		*/
		virtual Time getGap() { return minGap; };
		/**
			Get (common) sampling frequency
			@return	frequency
		*/
		virtual Frequency getFrequency() { if ( n == 0 ) return 0; else return files[0].frequency; };

	private:

		/**
			List of ncs file objects
		*/
		NCSFile		*files;
		/**
			Total number of ncs files
		*/
		int			n;
		/**
			Reverse signals?
		*/
		bool			reverse;
		/**
			Record duration
		*/
		Time			recordDuration;
		/**
			Previous timestamp
		*/
		Time			t0;
		/**
			Current timestamp = earliest timestamp across ncs files
		*/
		Time			t;
		/**
			In each ncs file, there is a (possibly non-zero) gap between current timestamp and the *end* of the previous record
			This is the minimum value across ncs files
		*/
		Time			minGap;
		/**
			Current record contents
		*/
		Data			buffer[NCS_N_SAMPLES_PER_RECORD*NCS_MAX_N_CHANNELS];
};

#endif
