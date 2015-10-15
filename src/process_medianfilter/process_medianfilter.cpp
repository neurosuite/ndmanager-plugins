/***************************************************************************
                           process_medianfilter.cpp
                             -------------------
    copyright            : (C) 2005-2008 by Michaël Zugaro
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

//  Adapted from:	L. Fiore, G. Corsini, L. Geppetti (1997). Application of non-linear
//  filters based on the median filter to experimental and simulated multiunit neural
//  recordings. Journal of Neuroscience Methods 70:177-184.

#define _LARGEFILE_SOURCE64
#define _FILE_OFFSET_BITS 64

#include <fstream>
#include <iostream>
#include <string>
#include <limits>
#include <cstring>
#include <cstdlib>

using namespace std;

bool verbose = false;

// Quicksort algorithm
// (adapted from GPL code available at http://linux.wku.edu/~lamonml/kb.html)
void quickSort(short int data[],int left,int right)
{
	int pivot,l_hold,r_hold;

	l_hold = left;
	r_hold = right;
	pivot = data[left];
	while ( left < right )
	{
		while ( (data[right] >= pivot) && (left < right) ) --right;
		if ( left != right )
		{
			data[left] = data[right];
			++left;
		}
		while ( (data[left] <= pivot) && (left < right) ) ++left;
		if ( left != right )
		{
			data[right] = data[left];
			--right;
		}
	}
	data[left] = pivot;
	pivot = left;
	left = l_hold;
	right = r_hold;
	if (left < pivot) quickSort(data,left,pivot-1);
	if (right > pivot) quickSort(data,pivot+1,right);
}

void sort(short int data[],int size)
{
  quickSort(data,0,size-1);
}

// Median filter
void filter(short windowHalfLength,short* widebandData,short* filteredData,long int nSamplesPerChannel,short int nChannels = 1)
{
	// Filter data in chunks
	// For the first and last chunks, the data must be padded with L zeros, i.e. if the first
	// chunk contains M samples s1...sM, then you should allocate a block of (M+L) values
	// 0...0s1...sM and pass a pointer to the first non-zero value (for the last chunk,
	// pass s1...sM0...0)

	// In all following comments, we write the number of samples in the window as N = 2*L+1
	// Assuming that the time-sorted window W contains values W(i)...W(i+N) at step i, we
	// also write V = W(i) for the 'oldest' value in the window, and V' = W(i+N+1) for the
	// new value that we must insert into the window at step (i+1), when V must be discarded

	long int nSamples = nSamplesPerChannel * nChannels;

	// Sliding window
	short windowLength = windowHalfLength*2+1;
	short *window = new short[windowLength];

	// Offsets in memory
	short halfOffset = nChannels * windowHalfLength;
	short offset = nChannels * windowLength;

	// Moving pointers to input and output signals: during the 'for' loop, 'input' points to sample i+L+1 and 'output' to
	// sample i
	short *input,*output;

	if ( verbose )
	{
		for ( int i = 0 ; i < nChannels ; ++i ) cout << ".";
		for ( int i = 0 ; i < nChannels ; ++i ) cout << "\b";
		cout << flush;
	}

	for ( int channel = 0 ; channel < nChannels ; ++channel )
	{
		if ( verbose ) cout << "#" << flush;

		// The initial window values are: W[0->N] = samples (-L) to L
		// i.e., W contains L samples from the overlap window and L samples from the new data
		input = widebandData + channel - halfOffset; // points to input sample (-L), i.e. the first sample in the overlap window
		for ( int i = 0 ; i < windowLength ; input+=nChannels,++i ) window[i] = *input;

		input = widebandData + channel + halfOffset + nChannels; // points to input sample (L+1), i.e. the first sample not yet in W
		output = filteredData + channel; // points to output sample 0

		// Sort window W into ascending order using quicksort algorithm
		// This results in a value-sorted window that we will refer to as X in later comments
		sort(window,windowLength);

		short newInputValue,oldInputValue;
		int indexInWindow;
		int samplesLeft;

		for ( samplesLeft = nSamples ; samplesLeft > 0 ; samplesLeft -= nChannels,input += nChannels,output += nChannels)
		{
			// Slide the window: read the new input value V' and insert it into the value-sorted
			// window X, and discard the oldest input value V

			newInputValue = *input; // input sample i+L+1
			oldInputValue = *(input-offset); // input sample i-L

			// Update X
			if ( newInputValue != oldInputValue )
			{
				// 1) Locate V in value-sorted window X using dichotomy
				int upperIndex = windowLength-1;
				int lowerIndex = 0;
				while ( lowerIndex <= upperIndex )
				{
					indexInWindow = (lowerIndex+upperIndex)/2;
					if ( oldInputValue > window[indexInWindow] )
						lowerIndex = indexInWindow+1;
					else if ( oldInputValue < window[indexInWindow] )
						upperIndex = indexInWindow-1;
					else break;
				}

				// 2) Insert V' at the appropriate position
				if ( newInputValue > oldInputValue )
				{
					// a) V' is higher than V: shift all values in X down one index, until the first
					//    value that is higher than V'
					while ( newInputValue > window[indexInWindow] && indexInWindow < windowLength - 1 )
					{
						window[indexInWindow] = window[indexInWindow+1];
						indexInWindow++;
					}
					if ( newInputValue > window[indexInWindow] ) window[indexInWindow] = newInputValue;
					else window[indexInWindow-1] = newInputValue;
				}
				else if ( newInputValue < oldInputValue )
				{
					// b) V' is lower than V: shift all values in X up one index, until the first
					//    value that is lower than V'
					while ( newInputValue < window[indexInWindow] && indexInWindow > 0 )
					{
						window[indexInWindow] = window[indexInWindow-1];
						indexInWindow--;
					}
					if ( newInputValue < window[indexInWindow] ) window[indexInWindow] = newInputValue;
					else window[indexInWindow+1] = newInputValue;
				}
			}

			// The filter output at this point is the input value minus the median of the values in the window
			// Remember that 'input' points to input sample i+L+1
			// The median is the (L+1)th point in X, i.e. X[L]
			*output = *(input-halfOffset-nChannels) - window[windowHalfLength];
 			/////*output = newInputValue - window[windowHalfLength];
 		}
	}
	if ( verbose ) cout << endl;
}

// Usage error
void error(char* name)
{
	cerr << "usage: " << name << " [options] input output (type '" << name << " -h' for details)" << endl;
	exit(1);
}

// Usage information
void help(char* name)
{
	cout << "usage: " << name << " [options] input output" << endl;
	cout << " -n nChannels   number of data channels" << endl;
	cout << " -w nSamples    window half length" << endl;
	cout << " -b buffer      chunk size in bytes" << endl;
	cout << " -v             verbose mode" << endl;
	exit(0);
}

// Start here
int main(int argc,char *argv[])
{
	short	*input,*output;
	short sampleSize = sizeof(input[0]);
	short windowHalfLength = 10,nChannels = 1;
	long int chunkSize = 0;

	// Parse command-line
	int i = 0;
	while ( true )
	{
		i++;
		if ( i >= argc ) error(argv[0]);
		if ( argv[i][0] != '-' )
		{
			if ( i > argc-2 ) error(argv[0]);
			break;
		}
		if ( strlen(argv[i]) < 2 ) error(argv[0]);
		switch ( argv[i][1] )
		{
			case 'h':
				help(argv[0]);
				break;

			case 'n':
				i++;
				if ( i > argc ) error(argv[0]);
				nChannels = atoi(argv[i]);
				break;

			case 'b':
				i++;
				if ( i > argc ) error(argv[0]);
				chunkSize = atoi(argv[i]);
				break;

			case 'v':
				verbose = true;
				break;

			case 'w':
				i++;
				if ( i > argc ) error(argv[0]);
				windowHalfLength = atoi(argv[i]);
				break;

			default:
				error(argv[0]);
				break;
		}
	}
	// Chunk description
	if ( chunkSize == 0 ) chunkSize = 512*1024*1024;
	int nSamplesPerChunkPerChannel = chunkSize / sampleSize / nChannels;
	chunkSize = nSamplesPerChunkPerChannel * sampleSize * nChannels; // Correct for rounding errors
	int nSamplesPerChunk = chunkSize / sampleSize;

	if ( verbose )
	{
		cout << endl;
		cout << "Number of channels      = " << nChannels << endl;
		cout << "Chunk size (bytes)      = " << chunkSize << endl;
		cout << "Samples/chunk/channel   = " << nSamplesPerChunkPerChannel << endl;
		cout << "Window half length      = " << windowHalfLength << endl;
		cout << "Input file              = " << argv[argc-2] << endl;
		cout << "Output file             = " << argv[argc-1] << endl;
		cout << endl;
	}

	/// Support for large files in C++ is buggy; use C file functions instead
	/// (C++ code is commented out using triple slashes ///)
	/// ifstream inputFile(argv[argc-2]);
	/// ofstream outputFile(argv[argc-1]);
	FILE* inputFile = fopen(argv[argc-2],"rb");
	if ( !inputFile ) { cerr << "error: cannot open input file '" << argv[argc-2] << "'." << endl; exit(1); }
	FILE* outputFile = fopen(argv[argc-1],"wb");
	if ( !outputFile ) { cerr << "error: cannot write to output file '" << argv[argc-1] << "'." << endl; exit(1); }

	// Determine number of samples per channel
	/// inputFile.seekg(0,ios_base::end);
	/// long long int size = inputFile.tellg();
	fseeko(inputFile,0,SEEK_END);
	long long int size = ftello(inputFile);

	/// inputFile.seekg(0,ios_base::beg);
	fseeko(inputFile,0,SEEK_SET);
	long long int nSamplesPerChannel,nSamples;
	nSamples = size/sampleSize;
	nSamplesPerChannel = nSamples/nChannels;

	// Initialize a few convenient variables
	int nPaddingSamples = nChannels * windowHalfLength;
	int nOverlapSamples = nChannels * windowHalfLength;
	long int overlapSize = nOverlapSamples * sampleSize;

	if ( nSamplesPerChunk >= nSamples )
	{
		// Input file contains a single chunk of data
		if ( verbose ) cout << "Chunk 1/1 (" << size << ") ";
		// Allocate arrays (allocate enough space to zero-pad input both before and after)
		input = new short[nPaddingSamples+nSamples];
		memset(input,0,nPaddingSamples*sizeof(short));
		output = new short[nSamples];
		// Read, filter, write
		/// inputFile.read((char*)&input[nPaddingSamples],size);
		fread((char*)&input[nPaddingSamples],sizeof(char),size,inputFile);
		filter(windowHalfLength,&input[nPaddingSamples],output,nSamplesPerChannel,nChannels);
		/// outputFile.write((char*)output,size);
		fwrite((char*)output,sizeof(char),size,outputFile);
	}
	else
	{
		// Input file contains at least two chunks of data

		long long int chunk = 1,nChunks = size/chunkSize;
		if ( nChunks * chunkSize < size ) nChunks++;
		long long int sizeLeft = size;
		// Allocate arrays (allocate enough space to zero-pad input before or after or not at all)
		input = new short[nSamplesPerChunk+2*nOverlapSamples];
		output = new short[nSamplesPerChunk];
		// Read, filter, write chunk by chunk
		for ( sizeLeft = size ; sizeLeft > 0 ; sizeLeft -= chunkSize,++chunk )
		{
			if ( sizeLeft <= chunkSize )
			{
				chunkSize = sizeLeft;
				nSamplesPerChunkPerChannel = sizeLeft/nChannels/sampleSize;
			}

			if ( verbose ) cout << "Chunk " << chunk << "/" << nChunks << " (" << chunkSize << ") ";

			if ( sizeLeft == size )
			{
				// Read first chunk of data + overlap (zero-pad by shifting the input pointer)
				/// inputFile.read((char*)&input[nOverlapSamples],chunkSize+overlapSize);
				fread((char*)&input[nOverlapSamples],sizeof(char),chunkSize+overlapSize,inputFile);
				// Filter
				filter(windowHalfLength,&input[nOverlapSamples],output,nSamplesPerChunkPerChannel,nChannels);
			}
			else
			{
				// Copy 'trailing' overlapping data to beginning of input buffer
				// Notice: the overlapping data starts at nOverlapSamples before the end of the previous chunk,
				// i.e. if we write o = nOverlapSamples and c = nSamplesPerChunk, at o+c+o - 2*o = c
				memcpy(input,&input[nSamplesPerChunk],2*overlapSize);
				// Read next chunk of data + next overlap
				///inputFile.read((char*)&input[2*nOverlapSamples],chunkSize);
				fread((char*)&input[2*nOverlapSamples],sizeof(char),chunkSize,inputFile);
				// Filter
				filter(windowHalfLength,&input[nOverlapSamples],output,nSamplesPerChunkPerChannel,nChannels);
			}
			// Write
			/// outputFile.write((char*)output,chunkSize);
			fwrite((char*)output,sizeof(char),chunkSize,outputFile);
		}
	}
	delete[] input;
	delete[] output;
	if ( verbose ) cout << endl;
}

