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

#include <iostream>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include <cstring>

#include "nlxextractor.h"

using namespace std;

string	program;

int main(int argc,char *argv[])
{
	// Default values
	int      i;
	bool     defaults = true;
	bool     wideband = false;
	bool     spots = false;
	bool     events = false;
	bool     force = false;
	bool     info = false;
	bool     reverse = false;
	bool     timestamps = false;
	bool     sync = false;
	bool     restart = false;
	bool     acquisition = false;
	Time     threshold = 1;
	float		videoFrequency = 25;
	char		*output = 0;

	// Program name
	program = argv[0];
	const char *slash = strrchr(program.c_str(),'/');
	if ( slash ) program = slash+1;

	// Number format for output (fixed, 6 decimal digits)
	cout.precision(6);
	cout.setf(ios::fixed,ios::floatfield);
	cerr.precision(6);
	cerr.setf(ios::fixed,ios::floatfield);

	// Parse command line optional arguments
	for ( i = 1 ; i < argc ; ++i )
		if ( strcmp(argv[i],"-h" ) == 0 || strcmp(argv[i],"--help") == 0  )
		{
			cout << endl;
			cout << "usage: " << program << " [options] input1 [input2...] output" << endl << endl;
			cout << "  -o, --output base  output basename" << endl;
			cout << "  -w, --wideband     convert NCS channels (-> .dat file)" << endl;
			cout << "  -r, --reverse      reverse NCS channels" << endl;
			cout << "  -v, --video        convert NVT or SMI records (-> .spots and .sts files)" << endl;
			cout << "  -F, --frequency    optional video sampling rate, used to detect unexpected gaps (default = 25 Hz)" << endl;
			cout << "  -e, --events       convert NEV records (-> .evt file)" << endl;
			cout << "  -t, --timestamps   generate non-synchronized timestamp files (for debugging)" << endl;
			cout << "  -s, --sync         generate synchronization file" << endl;
			cout << "  -a, --acquisition  generate acquisition restart file" << endl;
			cout << "  -g, --gap dt       a data record is considered to match an 'acquisition restart' event if it is" << endl;
			cout << "                     close in time and follows a sampling gap exceeding this threshold (in seconds," << endl;
			cout << "                     default = 1)" << endl;
			cout << "  -i, --info         display file header information" << endl;
			cout << "  -f, --force        overwrite existing files (default = off)" << endl;
			cout << "  -h, --help         display help" << endl;
			cout << "  input1...          input files" << endl;
			cout << endl;
			cout << " Note: when no conversion options are provided, '-w -v -e -s -a' is assumed by default." << endl;
			exit(EXIT_SUCCESS);
		}
		else if ( strcmp(argv[i],"-o") == 0 || strcmp(argv[i],"--output") == 0 ) output = argv[++i];
		else if ( strcmp(argv[i],"-w") == 0 || strcmp(argv[i],"--wideband") == 0 ) { defaults = false; wideband = true; }
		else if ( strcmp(argv[i],"-v") == 0 || strcmp(argv[i],"--video") == 0 ) { defaults = false; spots = true; }
		else if ( strcmp(argv[i],"-e") == 0 || strcmp(argv[i],"--events") == 0 ) { defaults = false; events = true; }
		else if ( strcmp(argv[i],"-r") == 0 || strcmp(argv[i],"--reverse") == 0 ) reverse = true;
		else if ( strcmp(argv[i],"-F") == 0 || strcmp(argv[i],"--frequency") == 0 ) videoFrequency = atof(argv[++i]);
		else if ( strcmp(argv[i],"-t") == 0 || strcmp(argv[i],"--timestamps") == 0 ) { defaults = false; timestamps = true; }
		else if ( strcmp(argv[i],"-s") == 0 || strcmp(argv[i],"--sync") == 0 ) { defaults = false; sync = true; }
		else if ( strcmp(argv[i],"-a") == 0 || strcmp(argv[i],"--acquisition") == 0 ) { defaults = false; restart = true; }
		else if ( strcmp(argv[i],"-i") == 0 || strcmp(argv[i],"--info") == 0 ) { defaults = false; info = true; }
		else if ( strcmp(argv[i],"-g") == 0 || strcmp(argv[i],"--gap") == 0 ) threshold = atoi(argv[++i]);
		else if ( strcmp(argv[i],"-f") == 0 || strcmp(argv[i],"--force") == 0 ) force = true;
		else if ( argv[i][0]== '-' )
		{
			error(string("unknown option '")+argv[i]+"'");
			exit(EXIT_FAILURE);
		}
		else break;

	threshold *= 1e6; // in µs
	Time frameDuration = 1e6/videoFrequency;

	// Which output files?
	if ( defaults )
	{
		wideband = true;
		spots = true;
		events = true;
		timestamps = false;
		sync = true;
		restart = true;
		info = false;
	}

	// Check number of input file names
	if ( argc < (i+1) )
	{
		error("not enough arguments");
		exit(EXIT_FAILURE);
	}

	// Process
	NLXExtractor extractor(info,wideband,spots,events,timestamps,sync,restart,force,reverse,threshold,frameDuration);

	try
	{
		extractor.init(&argv[i],argc-i,output);
		if ( info ) extractor.showFileHeaders();
		extractor.extract();
	}
	catch(NLXError e)
	{
		error(e.report());
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

