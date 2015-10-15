/***************************************************************************
 *   Copyright (C) 2004-2009 by Michael Zugaro                             *
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

int main(int argc,char *argv[])
{
	typedef short   Data; // The data type is defined as a custom type (easier to read and update)

	long    bufferSizeFactor = 8192;
	int     i,inputFileArgument = 1;
	int     reverse = 1;

	// Parse command line optional arguments

	for ( i = 1 ; i < argc ; ++i )
		if ( strcmp(argv[i],"-h" ) == 0 || strcmp(argv[i],"--help") == 0  )
		{
			cout << endl;
			cout << "usage: " << argv[0] << " [options] input output nChannels channel1 [channel2...]" << endl << endl;
			cout << "  -r, --reverse                 reverse signals (required for some acquisition systems)" << endl;
			cout << "  -b factor, --buffer factor    buffer size factor (default 8192)" << endl;
			cout << "                                input buffer size: factor * channels" << endl;
			cout << "                                output buffer size: factor * N" << endl;
			cout << "                                (N is the number of channels to extract)" << endl;
			cout << "  -h, --help                    display help" << endl;
			cout << "  input                         input file" << endl;
			cout << "  output                        output file" << endl;
			cout << "  nChannels                     number of channels in the input file" << endl;
			cout << "  channel1 [channel2...]        list of channels to extract" << endl << endl;
			cout << "Custom gains can be applied to one or more channels. For instance, to multiply" << endl;
			cout << "channel 5 by a gain of 1.5, write 5*1.5 in the channel list." << endl << endl;
			cout << "Custom references can be applied post-hoc to one or more channels. For instance," << endl;
			cout << "to subtract channel 2 from channel 5, write 5-2 in the channel list." << endl << endl;
			cout << "NOTE: Channels are numbered from 0." << endl << endl;
			cout << endl;
			return EXIT_SUCCESS;
		}
		else if ( strcmp(argv[i],"-b") == 0 || strcmp(argv[i],"--buffer") == 0 ) bufferSizeFactor = atoi(argv[++i]);
		else if ( strcmp(argv[i],"-r") == 0 || strcmp(argv[i],"--reverse") == 0 ) reverse = -1;
		else if ( argv[i][0]== '-' )
		{
			cerr << "error: unknown option " << argv[i] << " (type " << argv[0] << " -h for help)" << endl;
			return EXIT_FAILURE;
		}
		else break;

	// Check number of required arguments (minimum 5 including command name)

	if ( argc < (i+3)+1 )
	{
		cerr << "error: not enough arguments (type " << argv[0] << " -h for help)" << endl;
		return EXIT_FAILURE;
	}

  // Get file names and channel list

	int   nChannels;
	int   nChannelsToExtract = argc - (i+3);
	int   channelToExtract[nChannelsToExtract];
	float gain[nChannelsToExtract];
	int   reference[nChannelsToExtract];
	bool  gainBefore[nChannelsToExtract];

	inputFileArgument = i;
	i += 2;
	nChannels = atoi(argv[i]);
	for ( int j = 0 ; j < nChannelsToExtract ; j++ )
	{
		i++;
		string channel(argv[i]);
		int times = channel.find('*');
		int minus = channel.find('-');
		int timesLength,minusLength;
		if ( times == -1 && minus == -1 )
		{
			// No gain, no reference
			gain[j] = 1;
			reference[j] = -1;
			channelToExtract[j] = atoi(channel.c_str());
		}
		else if ( times == -1 )
		{
			// No gain, reference
			gain[j] = 1;
			reference[j] = atof(channel.substr(minus+1,channel.length()).c_str());
			channelToExtract[j] = atoi(channel.substr(0,minus).c_str());
		}
		else if ( minus == -1 )
		{
			// Gain, no reference
			gain[j] = atof(channel.substr(times+1,channel.length()).c_str());
			reference[j] = -1;
			channelToExtract[j] = atoi(channel.substr(0,times).c_str());
		}
		else
		{
			// Gain and reference
			if ( minus > times )
			{
				// Gain before
				gain[j] = atof(channel.substr(times+1,minus-times+1).c_str());
				reference[j] = atof(channel.substr(minus+1,channel.length()).c_str());
				channelToExtract[j] = atoi(channel.substr(0,times).c_str());
				gainBefore[j] = true;
			}
			else
			{
				// Gain after
				gain[j] = atof(channel.substr(times+1,channel.length()).c_str());
				reference[j] = atof(channel.substr(minus+1,times-minus+1).c_str());
				channelToExtract[j] = atoi(channel.substr(0,minus).c_str());
				gainBefore[j] = false;
			}
		}
		if ( channelToExtract[j] < 0 )
		{
			cerr << "error: negative channel number (" << channelToExtract[j] << ")." << endl;
			return EXIT_FAILURE;
		}
		else if ( channelToExtract[j] >= nChannels )
		{
			cerr << "error: cannot extract channel " << channelToExtract[j] << " (" << nChannels << " channels, numbered from 0)." << endl;
			return EXIT_FAILURE;
		}
		if ( reference[j] != -1 )
			if ( reference[j] < 0 )
			{
				cerr << "error: negative reference number (" << reference[j] << ")." << endl;
				return EXIT_FAILURE;
			}
			else if ( reference[j] >= nChannels )
			{
				cerr << "error: cannot use channel " << reference[j] << " as reference (" << nChannels << " channels, numbered from 0)." << endl;
				return EXIT_FAILURE;
			}
	}
// 	for ( int j = 0 ; j < nChannelsToExtract ; j++ )
// 	{
// 		i++;
// 		string channel(argv[i]);
// 		int n = channel.find('*');
// 		channelToExtract[j] = atoi(channel.substr(0,n).c_str());
// 		if ( n == -1 ) gain[j] = 1;
// 		else gain[j] = atof(channel.substr(n+1,channel.length()).c_str());
// 		if ( channelToExtract[j] < 0 )
// 		{
// 			cerr << "error: negative channel number (" << channelToExtract[j] << ")." << endl;
// 			return EXIT_FAILURE;
// 		}
// 		else if ( channelToExtract[j] >= nChannels )
// 		{
// 			cerr << "error: cannot extract channel " << channelToExtract[j] << " (" << nChannels << " channels, numbered from 0)." << endl;
// 			return EXIT_FAILURE;
// 		}
// 	}

	// Open the source file

	ifstream  inputFile(argv[inputFileArgument],ifstream::in);
	if ( !inputFile.good() )
	{
		cerr << "error: could not open " << argv[inputFileArgument] << endl;
		return EXIT_FAILURE;
	}

	// To increase throughput, the file will be read/written in chunks:
	//   size of input chunk = nChannels * buffersizeFactor * sizeof(Data)
	//   size of output chunk = nChannelsToExtract * buffersizeFactor * sizeof(Data)

	// Create the input/output buffers

	Data         *inputBuffer,*outputBuffer;
	const int    nSamplesInInputBuffer = nChannels*bufferSizeFactor;
	const int    nSamplesInOutputBuffer = nChannelsToExtract*bufferSizeFactor;

	inputBuffer = new Data[nSamplesInInputBuffer];
	outputBuffer = new Data[nSamplesInOutputBuffer];
	if ( inputBuffer == 0 || outputBuffer == 0 )
	{
		cerr << "error: out of memory" << endl;
		return EXIT_FAILURE;
	}

	// Create the target file

	ofstream outputFile(argv[inputFileArgument+1],ofstream::binary);
	if ( !outputFile.good() )
	{
		cerr << "error: could not create " << argv[inputFileArgument+1] << endl;
		return EXIT_FAILURE;
	}

	// Determine the total number of data chunks to read
	// This is needed to display progress information

	long   before,after;
	long   nIterations,iterationsDone = 0,nMarks = 0,maxNMarks = 50;

	before = inputFile.tellg();
	inputFile.seekg(0,ios::end);
	after = inputFile.tellg();
	inputFile.seekg(0,ios::beg);
	nIterations = (after-before)/(nSamplesInInputBuffer*sizeof(Data));

	if ( fmod((float) after-before,(float) nIterations) != 0 || fmod((float) after-before,(float) nIterations*100) != 0 ) nIterations++;

	// Display "empty" progress bar

	cout << argv[inputFileArgument] << " (" << nChannelsToExtract << "/" << nChannels;
	if ( nChannels == 1 ) cout << " channel) "; else  cout << " channels) ";
	cout << "0% ";
	for ( int i = 0 ; i < maxNMarks ; i++ ) cout << ".";
	cout << " 100%\b\b\b\b\b";
	for ( int i = 0 ; i < maxNMarks ; i++ ) cout << "\b";

	// Extract the data

	while ( !inputFile.eof() )
	{
		long   inputChunkSize,outputChunkSize;
		long   nInputRecords;

		// Read data into input buffer
		// Note: because EOF can be reached before filling the entire buffer,
		// we need to determine the actual amount of data read from the file

		inputFile.read((char *) inputBuffer,nSamplesInInputBuffer*sizeof(Data));
		inputChunkSize = inputFile.gcount();
		outputChunkSize = inputChunkSize / nChannels * nChannelsToExtract;

		// Update progress bar

		iterationsDone++;
		if ( ((float)iterationsDone/(float)nIterations) > ((float)nMarks)/((float)maxNMarks) )
		{
			int nMarksToAdd = (int) (((float)iterationsDone/(float)nIterations*(float)maxNMarks) - (float) nMarks);
			for ( int i = 0 ; i < nMarksToAdd ; i++ ) cout << "#";
			nMarks += nMarksToAdd;
			cout << flush;
		}

		// Copy the relevant data from the input buffer to the output buffer, then to disk
		// (a 'record' corresponds to one sample for each electrode)

		nInputRecords = (long) ((float) inputChunkSize / sizeof(Data) / nChannels);
		for ( int i = 0 ; i < nInputRecords ; i++ )
			for ( int j = 0 ; j < nChannelsToExtract ; j++ )
			{
				if ( reference[j] == -1 ) outputBuffer[i*nChannelsToExtract+j] = reverse*gain[j]*inputBuffer[i*nChannels+channelToExtract[j]];
				else if ( gainBefore[j] ) outputBuffer[i*nChannelsToExtract+j] = reverse*(gain[j]*inputBuffer[i*nChannels+channelToExtract[j]]-inputBuffer[i*nChannels+reference[j]]);
				else outputBuffer[i*nChannelsToExtract+j] = reverse*gain[j]*(inputBuffer[i*nChannels+channelToExtract[j]]-inputBuffer[i*nChannels+reference[j]]);
			}

		outputFile.write((char *) outputBuffer,outputChunkSize);
	}

	cout << endl;

	outputFile.close();
	inputFile.close();

	return EXIT_SUCCESS;
}
