/***************************************************************************
                              process_merge.cpp
                             -------------------
    copyright            : (C) 2007-2008 by Michaël Zugaro
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

//#define _GNU_SOURCE                     // includes support for large file support (LFS)
#define _LARGEFILE_SOURCE
#define _FILE_OFFSET_BITS 64

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string.h>
#include <math.h>

using namespace std;

int main(int argc, char *argv[])
{
	typedef short   Data; // The data type is defined as a custom type (easier to read and update)

	long    nRecordsPerChunk = 8192;
	int     i,inputFileArgument = 1;
	bool    truncate = false;

	// Parse command line optional arguments
	for ( i = 1 ; i < argc ; ++i )
		if ( strcmp(argv[i],"-h" ) == 0 || strcmp(argv[i],"--help") == 0  )
		{
			cout << endl;
			cout << "usage: " << argv[0] << " [options] input1 n1 input2 n2 [input3 n3...] output" << endl << endl;
			cout << "  -b factor, --buffer factor    buffer size factor (default 8192)" << endl;
			cout << "  -t, --truncate                truncate to size of smallest input file" << endl;
			cout << "  -h, --help                    display help" << endl;
			cout << "  input1...                     data files to process (in 16 bit .dat format)" << endl;
			cout << "  n1...                         number of channels in each input file" << endl;
			cout << "  output                        output file name" << endl << endl;
			cout << "Unless the -t option is set, using input files of different sizes generates an error." << endl << endl;
			cout << "NOTE: Channels are numbered from 0." << endl << endl;
			return EXIT_SUCCESS;
		}
		else
			if ( strcmp(argv[i],"-b") == 0 || strcmp(argv[i],"--buffer") == 0 )
				nRecordsPerChunk = atoi(argv[++i]);
			else
				if ( strcmp(argv[i],"-t") == 0 || strcmp(argv[i],"--truncate") == 0 )
					truncate = true;
				else
					if ( argv[i][0]== '-' )
					{
						cerr << "error: unknown option " << argv[i] << " (type " << argv[0] << " -h for help)" << endl;
						return EXIT_FAILURE;
					}
					else break;

	// Check number of mandatory arguments (should be 2k+1, k>=1)
	int  nArgumentsLeft = argc - i,nInputFiles = (nArgumentsLeft - 1)/2;
	if ( nInputFiles < 1 || nInputFiles*2+1 != nArgumentsLeft )
	{
		cerr << "error: incorrect number of arguments (type " << argv[0] << " -h for help)" << endl;
		return EXIT_FAILURE;
	}

	// Parse command line mandatory arguments
	string   inputFileName[nInputFiles],outputFileName;
	int      nChannels[nInputFiles];
	int      totalNChannels = 0;
	for ( int j = 0 ; j < nInputFiles ; ++j )
	{
		inputFileName[j] = argv[i+2*j];
		nChannels[j] = atoi(argv[i+2*j+1]);
		if ( nChannels[j] <= 0 )
		{
			cerr << "error: incorrect number of channels (" << argv[i+2*j+1] << ") for " << inputFileName[j] << " (type " << argv[0] << " -h for help)" << endl;
			return EXIT_FAILURE;
		}
		totalNChannels += nChannels[j];
	}
	outputFileName = argv[argc-1];

	// Open input files
	ifstream    inputFile[nInputFiles];
	long        before,after;
	long        nRecords,minNRecords;
	for ( int i = 0 ; i < nInputFiles ; ++i )
	{
		inputFile[i].open(inputFileName[i].c_str(),ifstream::in);
		if ( !inputFile[i].good() )
		{
			cerr << "error: could not open " << inputFileName[i] << endl;
			return EXIT_FAILURE;
		}

		// Compute number of records
		// (a 'record' corresponds to one sample for each channel)
		before = inputFile[i].tellg();
		inputFile[i].seekg(0,ios::end);
		after = inputFile[i].tellg();
		inputFile[i].seekg(0,ios::beg);
		nRecords = (after-before)/(nChannels[i]*sizeof(Data));
		if ( i == 0 ) minNRecords = nRecords;
		else if ( !truncate && nRecords != minNRecords )
		{
			cerr << "error: files have different durations (use option -t to truncate; type " << argv[0] << " -h for help)" << endl;
			return EXIT_FAILURE;
		}
		else if ( nRecords < minNRecords ) minNRecords = nRecords;
	}
	nRecords = minNRecords;

	// To increase throughput, the file will be read/written in chunks:
	//   size of chunk (in samples) = n records per chunk * total n channels

	// Create the input/output buffers
	Data         *inputBuffer,*outputBuffer;
	const int    nSamplesPerChunk = totalNChannels*nRecordsPerChunk;
	inputBuffer = new Data[nSamplesPerChunk];
	outputBuffer = new Data[nSamplesPerChunk];
	if ( inputBuffer == 0 || outputBuffer == 0 )
	{
		cerr << "error: out of memory" << endl;
		return EXIT_FAILURE;
	}

	// Create output file
	ofstream outputFile(outputFileName.c_str(),ofstream::binary);
	if ( !outputFile.good() )
	{
		cerr << "error: could not create " << outputFileName << endl;
		return EXIT_FAILURE;
	}

	// Determine the total number of data chunks to read
	// Because input file sizes may not be a multiple of the chunk size, we need to determine
	// how many records will be read in the last chunk
	long   nChunks,nRecordsInLastChunk;
	nChunks = nRecords/nRecordsPerChunk;
	nRecordsInLastChunk = nRecords-nChunks*nRecordsPerChunk;
	if ( nRecordsInLastChunk == 0 ) nRecordsInLastChunk = nRecordsPerChunk;
	else nChunks++;

/*	// Input file sizes may not be a multiple of the chunk size; thus, we need to keep track of how
	// much data was actually read from disk when we read the last chunk
	long   chunkSize,nRecordsInChunk;*/

	// Display progress information
	// Start with an "empty" progress bar
	long   nMarks = 0,maxNMarks = 50;
	cout << "Merging files (total " << totalNChannels << " channels) ";
	cout << "0% ";
	for ( int i = 0 ; i < maxNMarks ; i++ ) cout << ".";
	cout << " 100%\b\b\b\b\b";
	for ( int i = 0 ; i < maxNMarks ; i++ ) cout << "\b";
	cout << flush;

	// Process the data
	for ( long chunk = 0 ; chunk < nChunks ; ++chunk )
	{
		long nRecordsInChunk,position = 0;

		// Treat last chunk separately because it may contain fewer records
		if ( chunk == nChunks-1 )
			nRecordsInChunk = nRecordsInLastChunk;
		else nRecordsInChunk = nRecordsPerChunk;

		// Read from each input file
		for ( int i = 0 ; i < nInputFiles ; ++i )
		{
			long  size = nChannels[i]*nRecordsInChunk*sizeof(Data);
			inputFile[i].read((char *) inputBuffer+position,size);
			position += size;
		}

		// Update progress bar
		if ( ((float)chunk/(float)nChunks) > ((float)nMarks)/((float)maxNMarks) )
		{
			int nMarksToAdd = (int) (((float)(chunk+1)/(float)nChunks*(float)maxNMarks) - (float) nMarks);
			for ( int i = 0 ; i < nMarksToAdd ; i++ ) cout << "#";
			nMarks += nMarksToAdd;
			cout << flush;
		}

		// Copy and reorder the data from the input buffer to the output buffer, then to disk
		int inputIndex,outputIndex = 0;
		for ( int i = 0 ; i < nRecordsInChunk ; ++i )
		{
			// The next sample in the input buffer is the first sample of the current record
			// from the first file
			inputIndex = i*nChannels[0];
			// Copy the current record (= all channels) from all input files
			for ( int j = 0 ; j < nInputFiles ; ++j )
			{
				// Copy the current record from the current input file
				for ( int k = 0 ; k < nChannels[j] ; ++k )
					outputBuffer[outputIndex++] = inputBuffer[inputIndex++];

				// If there are more files to process, we must find the next sample in the input buffer
				if ( j < nInputFiles-1 )
				{
					// To find the next sample in the input buffer, we must first skip all remaining
					// records from the current file (i.e. a number of samples equal to the number
					// of records left * the number of channels in current file)...
					inputIndex += (nRecordsInChunk-i-1)*nChannels[j];
					// ... and then skip all records from the next file that have already been read
					// (i.e. a number of samples equal to the number of records read * the number of
					// channels in next file)
					inputIndex += i*nChannels[j+1];
				}
			}
			// The next sample in the input buffer is the first sample of the next record
			// from the first file
		}

		// Wow! we're done. Let's write this to disk
		outputFile.write((char *) outputBuffer,totalNChannels*nRecordsInChunk*sizeof(Data));
	}
	cout << endl;

	// Close all files
	outputFile.close();
	for ( int i = 0 ; i < nInputFiles ; ++i ) inputFile[i].close();

	return EXIT_SUCCESS;
}
