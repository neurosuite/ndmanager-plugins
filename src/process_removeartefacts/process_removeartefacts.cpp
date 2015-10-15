/***************************************************************************
                          process_removeartefacts.cpp
                             -------------------
    copyright            : (C) 2006-2008 by Michaël Zugaro
    email                : michael.zugaro@college-de-france.fr
 ***************************************************************************/

/***************************************************************************
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

#define _LARGEFILE_SOURCE
#define _FILE_OFFSET_BITS 64

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string.h>
#include <sstream>
#include <math.h>

using namespace std;

main(int argc,char *argv[])
{
	int nClusters,clusterID,time;
	int feature[1000];
	int nChannels,nSamplesPerWaveform,nFeatures,resolution;
	string basename,electrodeGroup;
	bool spk = true,fet = true,res = true;

	// Read arguments
	if ( argc == 2 && (!strcmp(argv[1],"-h") || !strcmp(argv[1],"--help")) )
	{
		cout << endl;
		cout << "usage: " << argv[0] << " basename group nElectrodes nSamples resolution" << endl;
		cout << " basename        basename of the data files" << endl;
		cout << " group           electrode group" << endl;
		cout << " nElectrodes     number of electrodes in group" << endl;
		cout << " nSamples        number of samples per waveform" << endl;
		cout << " resolution      resolution of the acquisition system (in bits)" << endl;
		cout << endl;
		return EXIT_SUCCESS;
	}
	else if ( argc != 6 )
	{
		cout << "error: incorrect number of parameters (type '" << argv[0] << " -h' for details)." << endl;
		return EXIT_FAILURE;
	}
	basename = argv[1];
	electrodeGroup = argv[2];
	stringstream sc(argv[3]);
	sc >> nChannels;
	stringstream ss(argv[4]);
	ss >> nSamplesPerWaveform;
	stringstream sr(argv[5]);
	sr >> resolution;

	// Open required input files
	string cluInputFilename = basename+".clu."+electrodeGroup;
	ifstream  cluInputFile(cluInputFilename.c_str(),ifstream::in);
	if ( !cluInputFile.good() )
	{
		cerr << "error: could not open '" << cluInputFilename <<  "'." << endl;
		return EXIT_FAILURE;
	}
	string resInputFilename = basename+".res."+electrodeGroup;
	ifstream resInputFile(resInputFilename.c_str(),ifstream::in);
	if ( !resInputFile.good() )
	{
		res = false;
		cerr << "warning: could not open '" << resInputFilename <<  "', skipping this file." << endl;
	}
	string spkInputFilename = basename+".spk."+electrodeGroup;
	ifstream  spkInputFile(spkInputFilename.c_str(),ifstream::in);
	if ( !spkInputFile.good() )
	{
		spk = false;
		cerr << "warning: could not open '" << spkInputFilename <<  "', skipping this file." << endl;
	}
	string fetInputFilename = basename+".fet."+electrodeGroup;
	ifstream  fetInputFile(fetInputFilename.c_str(),ifstream::in);
	if ( !fetInputFile.good() )
	{
		fet = false;
		cerr << "warning: could not open '" << fetInputFilename <<  "', skipping this file." << endl;
	}

	// Open output files
	string resOutputFilename("tmp.res");
	ofstream  resOutputFile(resOutputFilename.c_str(),ifstream::out);
	if ( res && !resOutputFile.good() )
	{
		cerr << "error: could not create temporary output res file." << endl;
		return EXIT_FAILURE;
	}
	string spkOutputFilename("tmp.spk");
	ofstream  spkOutputFile(spkOutputFilename.c_str(),ifstream::out);
	if ( spk && !spkOutputFile.good() )
	{
		cerr << "error: could not create temporary output spk file." << endl;
		remove(resOutputFilename.c_str());
		return EXIT_FAILURE;
	}
	string cluOutputFilename("tmp.clu");
	ofstream  cluOutputFile(cluOutputFilename.c_str(),ifstream::out);
	if ( !cluOutputFile.good() )
	{
		cerr << "error: could not create temporary output clu file." << endl;
		remove(resOutputFilename.c_str());
		remove(spkOutputFilename.c_str());
		return EXIT_FAILURE;
	}
	string fetOutputFilename("tmp.fet");
	ofstream  fetOutputFile(fetOutputFilename.c_str(),ifstream::out);
	if ( fet && !fetOutputFile.good() )
	{
		cerr << "error: could not create temporary output fet file." << endl;
		remove(resOutputFilename.c_str());
		remove(spkOutputFilename.c_str());
		remove(cluOutputFilename.c_str());
		return EXIT_FAILURE;
	}

	// Read and write number of clusters and features
	cluInputFile >> nClusters;
	cluOutputFile << nClusters << '\n';
	if ( fet )
	{
		fetInputFile >> nFeatures;
		fetOutputFile << nFeatures << '\n';
	}
	// Buffer for spk data
	int bufferLength = nChannels*nSamplesPerWaveform*resolution/(sizeof(char)*8);
	char buffer[bufferLength];

	while ( !cluInputFile.eof() )
	{
		// Read data from clu, res, fet and spk files
		if ( res ) resInputFile >> time;
		cluInputFile >> clusterID;
		if ( fet ) for ( int i = 0 ; i < nFeatures ; ++i ) fetInputFile >> feature[i];
		if ( spk ) spkInputFile.read(buffer,bufferLength);

		// Check for read errors
		if ( cluInputFile.eof() || (res && resInputFile.eof()) || (fet && fetInputFile.eof()) || (spk && spkInputFile.eof()) )
		{
			if ( cluInputFile.eof() && (!res || resInputFile.eof()) && (!fet || fetInputFile.eof()) && (!spk || spkInputFile.eof()) ) break;

			// End of file was reached in one or more files, but not all
			cerr << "error: the res, fet, clu and spk files do not have the same number of points (stopped at t = " << time << "), leaving input files untouched." << endl;

			remove(cluOutputFilename.c_str());
			if ( res ) remove(resOutputFilename.c_str());
			if ( spk ) remove(spkOutputFilename.c_str());
			if ( fet ) remove(fetOutputFilename.c_str());
			return EXIT_FAILURE;
		}
		else if ( cluInputFile.fail() || (res && resInputFile.fail()) || (fet && fetInputFile.fail()) || (spk && spkInputFile.fail()) )
		{
			cerr << "error: failed to read from the res, fet, clu or spk file (stopped at t = " << time << "), leaving input files untouched." << endl;
			remove(cluOutputFilename.c_str());
			if ( res ) remove(resOutputFilename.c_str());
			if ( spk ) remove(spkOutputFilename.c_str());
			if ( fet ) remove(fetOutputFilename.c_str());
			return EXIT_FAILURE;
		}


		// Unless data corresponds to artefacts (clusterID==0), save to output files
		if ( clusterID > 0 )
		{
			cluOutputFile << clusterID << '\n';
			if ( res ) resOutputFile << time << '\n';
			if ( fet )
			{
				for ( int i = 0 ; i < nFeatures-1 ; ++i ) fetOutputFile << feature[i] << " ";
				fetOutputFile << feature[nFeatures-1] << '\n';
			}
			if ( spk ) spkOutputFile.write(buffer,bufferLength);

			// Check for write errors
			if ( cluInputFile.fail() || (res && resInputFile.fail()) || (fet && fetInputFile.fail()) || (spk && spkInputFile.fail()) )
			{
				cerr << "error: failed to write to the res, fet, clu or spk file (stopped at t = " << time << "), leaving input files untouched." << endl;
				remove(cluOutputFilename.c_str());
				if ( res ) remove(resOutputFilename.c_str());
				if ( spk ) remove(spkOutputFilename.c_str());
				if ( fet ) remove(fetOutputFilename.c_str());
				return EXIT_FAILURE;
			}
		}

	}

	// Close files
	cluInputFile.close();
	if ( res ) resInputFile.close();
	if ( fet ) fetInputFile.close();
	if ( spk ) spkInputFile.close();
	cluOutputFile.close();
	if ( res ) resOutputFile.close();
	if ( fet ) fetOutputFile.close();
	if ( spk ) spkOutputFile.close();

	// Backup (rename) existing files
	// (in case of an error, try to leave directory in initial state)
	if ( rename(cluInputFilename.c_str(),(cluInputFilename+"~").c_str())  )
	{
		cerr << "error: cannot backup existing clu file (make sure you have removed all previously created backup files), leaving input files untouched." << endl;
		remove(cluOutputFilename.c_str());
		if ( res ) remove(resOutputFilename.c_str());
		if ( spk ) remove(spkOutputFilename.c_str());
		if ( fet ) remove(fetOutputFilename.c_str());
		return EXIT_FAILURE;
	}
	if ( res && rename(resInputFilename.c_str(),(resInputFilename+"~").c_str()) )
	{
		cerr << "error: cannot backup existing res file (make sure you have removed all previously created backup files), leaving input files untouched." << endl;
		rename((cluInputFilename+"~").c_str(),cluInputFilename.c_str());
		remove(cluOutputFilename.c_str());
		if ( res ) remove(resOutputFilename.c_str());
		if ( spk ) remove(spkOutputFilename.c_str());
		if ( fet ) remove(fetOutputFilename.c_str());
		return EXIT_FAILURE;
	}
	if ( fet && rename(fetInputFilename.c_str(),(fetInputFilename+"~").c_str()) )
	{
		cerr << "error: cannot backup existing fet file (make sure you have removed all previously created backup files), leaving input files untouched." << endl;
		rename((cluInputFilename+"~").c_str(),cluInputFilename.c_str());
		if ( res ) rename((resInputFilename+"~").c_str(),resInputFilename.c_str());
		remove(cluOutputFilename.c_str());
		if ( res ) remove(resOutputFilename.c_str());
		if ( spk ) remove(spkOutputFilename.c_str());
		if ( fet ) remove(fetOutputFilename.c_str());
		return EXIT_FAILURE;
	}
	if ( spk && rename(spkInputFilename.c_str(),(spkInputFilename+"~").c_str()) )
	{
		cerr << "error: cannot backup existing spk file (make sure you have removed all previously created backup files), leaving input files untouched." << endl;
		rename((cluInputFilename+"~").c_str(),cluInputFilename.c_str());
		if ( res ) rename((resInputFilename+"~").c_str(),resInputFilename.c_str());
		if ( fet ) rename((fetInputFilename+"~").c_str(),fetInputFilename.c_str());
		remove(cluOutputFilename.c_str());
		if ( res ) remove(resOutputFilename.c_str());
		if ( spk ) remove(spkOutputFilename.c_str());
		if ( fet ) remove(fetOutputFilename.c_str());
		return EXIT_FAILURE;
	}

	// Rename temporary output files
	if ( rename(cluOutputFilename.c_str(),cluInputFilename.c_str()) )
	{
		cerr << "warning: cannot rename temporary output clu file (current name: '" << cluOutputFilename << "')." << endl;
	}
	if ( res && rename(resOutputFilename.c_str(),resInputFilename.c_str()) )
	{
		cerr << "warning: cannot rename temporary output res file (current name: '" << resOutputFilename << "')." << endl;
	}
	if ( fet && rename(fetOutputFilename.c_str(),fetInputFilename.c_str()) )
	{
		cerr << "warning: cannot rename temporary output fet file (current name: '" << fetOutputFilename << "')." << endl;
	}
	if ( spk && rename(spkOutputFilename.c_str(),spkInputFilename.c_str()) )
	{
		cerr << "warning: cannot rename temporary output spk file (current name: '" << spkOutputFilename << "')." << endl;
	}

	return EXIT_SUCCESS;
}
