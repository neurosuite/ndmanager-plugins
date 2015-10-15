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

#ifndef PROGRESS_BAR
#define PROGRESS_BAR

#include <fstream>
#include <iostream>

using namespace std;

/**
@author Michael Zugaro
*/
class ProgressBar
{
	public:
		/**
			Constructor
			@param	label description, e.g. file base name
			@param	step processing step, e.g. "NEV" or "SYNC"
			@param	total total number of iterations to reach 100%
			@param	length bar length (default 50)
		*/
		ProgressBar(string label,string step,int total,int length = 50) : label(label),step(step),done(0),total(total),marks(0),length(length) {};
		/**
			Destructor
		*/
		virtual ~ProgressBar() { cout << endl; };
		/**
			Display initial (empty) progress bar
		*/
		virtual void start();
		/**
			Advance progress bar by one iteration
		*/
		virtual void advance();
		/**
			Display a message
			@param	msg message
		*/
		virtual void message(string msg);

	private:

		string	label;
		string	step;
		int		done;
		int		total;
		int		marks;
		int		length;
};

#endif
