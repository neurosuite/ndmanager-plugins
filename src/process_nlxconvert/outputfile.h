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

#ifndef OUTPUT_FILE
#define OUTPUT_FILE

#include "nlxerror.h"

#include <fstream>

using namespace std;

/**
@author Michael Zugaro
*/
class OutputFile : public ofstream
{
	public:
		/**
			Constructor
		*/
		OutputFile() : filename(""),force(false),mode(ios_base::trunc) {};
		/**
			Constructor
			@param	filename		output file name
			@param	force			overwrite existing file?
			@param	mode			text vs binary
		*/
		OutputFile(string filename,bool force,ios_base::openmode mode) : filename(filename),force(force),mode(mode) {};
		/**
			Destructor
		*/
		virtual ~OutputFile() {};
		/**
			Setter
			@param	filename		output file name
			@param	force			overwrite existing file?
			@param	mode			text vs binary
		*/
		virtual void set(string filename,bool force,ios_base::openmode mode) { this->filename = filename; this->force = force; this->mode = mode; };
		/**
			Create output file
		*/
		virtual void create() throw(NLXError);

	private:

		string						filename;
		bool							force;
		ios_base::openmode		mode;
};

#endif
