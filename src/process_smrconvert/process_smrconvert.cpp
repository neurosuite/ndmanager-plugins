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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <iostream>
#include <cstdlib>
#include <stdint.h>
#include <cstring>

#include "sonfile.h"
#include "sonextractor.h"

bool verbose;
using namespace std;

void printUsage(ostream& out,string program)
{
	out << endl;
	out << "usage: " << program << " [OPTIONS] { filename | basename }" << endl;
	out << endl;
	out << " -i,--info                 display header info (file and channels)" << endl;
	out << " -w,--wideband             convert ADC channels (-> .dat file)" << endl;
	out << " -r,--reverse              reverse ADC channels (some acquisition systems invert all signals; compensate for this)" << endl;
	out << " -c,--channels c1[,c2...]  convert only listed ADC channels (incompatible with '-x' option)" << endl;
	out << " -x,--exclude c1[,c2...]   do not convert listed ADC channels (incompatible with '-c' option)" << endl;
	out << " -s,--spikes               convert ADC mark channels (-> .spk and .res files)" << endl;
	out << " -e,--events               convert events (-> .evt file)" << endl;
	out << endl;
	out << " -h, --help                display options" << endl;
	out << " -f, --force               overwrite existing files (default = off)" << endl;
	out << " -v, --verbose             produce more detailed output while converting (default = off)" << endl;
	out << endl;
	out << " Note: when no conversion options are provided, '-i -w -s -e' is assumed by default." << endl;
	out << endl;
}

int main(int argc,char *argv[])
{
	char		option;
	bool		header = false,dat = false,spk = false,evt = false,all = true,force = false,reverse = false;

	// Program name
	char *slash = strrchr(argv[0],'/');
	char *start;
	if ( slash ) start = slash+1;
	else start = argv[0];
	string	program(start);
	uint16_t nChannelsListed = 0;
	uint16_t channelList[1000];
	bool include = false,included = false,excluded = false;

	// Parse command-line
	for ( int i = 1 ; i < argc ; ++i )
	{
		if ( argv[i][0] != '-' ) break;
		if ( argv[i][1] != '-' ) option = argv[i][1];
		else
			if ( strlen(argv[i]) < 3 )
			{
				printUsage(cerr,program);
				return 1;
			}
			else option = argv[i][2];

		switch ( option )
		{
			case 'i':
				header = true;
				all = false;
				break;

			case 'd':
			case 'w':
				dat = true;
				all = false;
				break;

			case 'c':
				if ( excluded )
				{
					cerr << program << ": incompatible options '-c' and '-x' (type '" << program << " -h' for details)" << endl;
					return EXIT_FAILURE;
				}
				included = true;
				dat = true;
				all = false;
				if ( ++i < argc )
				{
					char *ptr = strtok(argv[i],",");
					while ( ptr != NULL )
					{
						channelList[nChannelsListed++] = atoi(ptr);
						ptr = strtok(NULL,",");
					}

				}
				else
				{
					printUsage(cerr,program);
					return EXIT_FAILURE;
				}
				include = true;
				break;

			case 'x':
				if ( included )
				{
					cerr << program << ": incompatible options '-c' and '-x' (type '" << program << " -h' for details)" << endl;
					return EXIT_FAILURE;
				}
				excluded = true;
				dat = true;
				all = false;
				if ( ++i < argc )
				{
					char *ptr = strtok(argv[i],",");
					while ( ptr != NULL )
					{
						channelList[nChannelsListed++] = atoi(ptr);
						ptr = strtok(NULL,",");
					}

				}
				else
				{
					printUsage(cerr,program);
					return EXIT_FAILURE;
				}
				include = false;
				break;

			case 'e':
				evt = true;
				all = false;
				break;

			case 'f':
				force = true;
				break;

			case 'h':
				printUsage(cout,program);
				return EXIT_SUCCESS;
				break;

			case 'r':
				reverse = true;
				break;

			case 's':
				spk = true;
				all = false;
				break;

			case 'v':
				verbose = true;
				break;

			default:
				printUsage(cerr,program);
				return EXIT_FAILURE;
				break;
		}
	}
	// Which output files?
	if ( all )
	{
		header = true;
		dat = true;
		spk = true;
		evt = true;
	}

	string 			basename(argv[argc-1]);

	// If command-line supplied file basename, extension = ".smr",
	// otherwise (complete filename) extension = ""
	string			extension = basename.substr(basename.length()-4);
	for( int i = 0 ; i < 4 ; ++i ) extension[i] = std::tolower(extension[i]);
	if ( extension.compare(".smr") == 0 ) extension = ""; else extension = ".smr";

	SONFile			file(basename+extension);
	SONExtractor	extractor(&file,header,dat,spk,evt,force,reverse,nChannelsListed,channelList,include);

	cout.precision(10);
	try
	{
		// Extract
		extractor.extract();
	}
	catch(SONError error)
	{
		cerr << endl << program << ": " << error.report() << endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

