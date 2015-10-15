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

#include "outputfile.h"

#include <iostream>

using namespace std;

void OutputFile::create() throw(NLXError)
{
	// Data file exists?
	if ( !force )
	{
		open(filename.c_str(),ios::in);
		bool exists = good();
		close();
		clear();
		
		if ( exists ) throw NLXError(NLXError::WRITE_ERROR,"'" + filename + "' exists.");
	}
	// Create data file
	open(filename.c_str(),mode);
	if ( !good() ) throw NLXError(NLXError::WRITE_ERROR,"could not create '" + filename + "'.");
}

