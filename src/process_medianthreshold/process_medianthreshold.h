/***************************************************************************
                          process_medianthreshold.h
                             -------------------
    copyright            : (C) 2007 by Nicolas Lebas
    email                : nicolas.lebas@college-de-france.fr


 * Description : process_medianthreshold is a program which allow us to calculate a spike threshold
 * from an electrophysiologic record.
 * The calculation of this threshold is based on the article "Unsupervised Spike Detection and Sorting 
 * with Wavelets and Superparamagnetic Clustering" (R. Quiroga, Z. Nadasdy, and Y. Ben-Shaul, 
 * neural computation, 2004).
 * --> Threshold = 4*sigma_n ; with : sigma_n=median{|x|/0.6745}
 *
 * Usage : process_medianthreshold [options] [input]
 *
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
#ifndef __PROCESS_MEDIAN_THRESHOLD_H
#define __PROCESS_MEDIAN_THRESHOLD_H

#define MAX_INPUT_SIZE 1280000000 // = 128*2*25*20000
#define MAX_CHANNO 128 // max channel number
#define RECORD_BYTE_SIZE 2
#define GROUP_SEPARATOR ":"
#define CHANNEL_SEPARATOR ","
#define SIGMA_DIVISOR 0.6745

#include <iostream>
using namespace std;


// Structure for all arguments
struct arguments {
	char *inputFileName, *outputFileName; // Input and Ouput files name
	int inputSize; // Size of the input (byte)
	int totalChannelNumber; // Total number of channels
	char* channelList;	// List of channels for each group (comma separated integers between 
							// channel numbers in a group and semicolon separated between two groups)
	int offset; // Offset at the beginning of the datas

	bool isInputFileProvided, isOutputFileProvided;
	bool isInputSizeprovided;
	bool isTotalChannelNumberProvided;
	bool isChannelListProvided;
	bool isOffsetProvided;
#ifdef NBITS
	int nBits; // Number of bits for datas (resolution)
	bool isNBitsProvided;
#endif
};


// Quicksort algorithm (growing order)
// Tri rapide. (2007, mars 21). Wikipedia, The Free Encyclopedia. Retrieved 13:20, April 2, 2007,
// from http://fr.wikipedia.org/w/index.php?title=Tri_rapide&oldid=15255912.
int clusterize(short int[], int, int); // subfunction of Quicksort (sort a subarray)
void quickSort(short int[], int, int); // main recursive function of Quicksort

// Start Quicksort (Sort the short int array in growing order)
void sort(short int[], const int);
// Compute the threshold of each channel in the given group
long double* groupThresholds(const short*, const int, const int*, const int);
// Parse arguments and store them in arguments
void parseArgs(const int, char **, arguments &);
// Check arguments values
bool checkInputs(const arguments);

#endif
