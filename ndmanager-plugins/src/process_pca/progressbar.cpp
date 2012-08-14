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

#include "progressbar.h"
#include "customtypes.h"

#include <iostream>

using namespace std;

void ProgressBar::start()
{
	cout << label << " [" << step << "]";
	int n = PROGRESS_MAX_N_CHARS - step.length() + 1;
	if ( n > 0 ) for ( int i = 0 ; i < n ; i++ ) cout << " ";
	cout << "0% ";
	for ( int i = 0 ; i < length ; i++ ) cout << ".";
	cout << " 100%\b\b\b\b\b";
	for ( int i = 0 ; i < length ; i++ ) cout << "\b";
}

void ProgressBar::advance()
{
	done++;
	if ( ((float)done/(float)total) > ((float)marks)/((float)length) )
	{
		int nMarksToAdd = (int) (((float)done/(float)total*(float)length) - (float) marks);
 		for ( int i = 0 ; i < nMarksToAdd ; i++ ) cout << "#";
		marks += nMarksToAdd;
 		cout << flush;
	}
}

void ProgressBar::message(string msg)
{
	for ( int i = 0 ; i < length-marks ; i++ ) cout << ".";cout << " 100% ";
	cout << " " << msg << endl;

	cout << label << " [" << step << "]";
	int n = PROGRESS_MAX_N_CHARS - step.length() + 1;
	if ( n > 0 ) for ( int i = 0 ; i < n ; i++ ) cout << " ";
	cout << "0% ";
	for ( int i = 0 ; i < marks ; i++ ) cout << "#";
	for ( int i = 0 ; i < length-marks ; i++ ) cout << ".";
	cout << " 100%\b\b\b\b\b";
	for ( int i = 0 ; i < length-marks ; i++ ) cout << "\b";
}

