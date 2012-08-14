/***************************************************************************
 *                                                                         *
 *   Copyright (C) 2004-2008 by Michael Zugaro                             *
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

#include <iostream>

#include "nlxerror.h"

extern string program;

NLXError::NLXError(ErrorType type,string message) :
exception(),
type(type),
message(message)
{
}

NLXError::~NLXError() throw ()
{
}

NLXError::ErrorType NLXError::getType() const
{
	return type;
}

string NLXError::report() const
{
	return message;
}

void warning(string message)
{
	cerr << program << " - warning: " << message;
	if ( message[message.length()-1] != '.' ) cerr << ".";
	cerr << endl;
}

void error(string message)
{
	if ( message[message.length()-1] == '.' ) message[message.length()-1] = 0;
	cerr << program << " - error: " << message << " (type " << program << " -h for help)." << endl;
}
