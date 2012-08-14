/***************************************************************************
                          process_pca.h  -  description
                             -------------------
    copyright            : (C) 2007 by Nicolas Lebas
    email                : nicolas.lebas@college-de-france.fr


 * Description : process_pca is a program which allow us to reduce spikes 
 * dimensions by computing a PCA.
 *
 * Usage : process_pca [options] [input]
 *
 ***************************************************************************/

/***************************************************************************
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 3 of the License, or
 *   (at your option) any later version.
 *
 ***************************************************************************/
#ifndef __PROCESS_PCAX_H
#define __PROCESS_PCAX_H

const unsigned long MAX_INPUT_SIZE = 2560000000; // = 128*2*25*400000
const int RECORD_BYTE_SIZE = 2;

#include <iostream>
using namespace std;


// Structure for all arguments
struct arguments {
	char *inputFileName; // Input file name
	char *outputFileName; // Ouput file name
	long long inputSize; // Size of the input (byte)
	int nChannels; // Total number of channels
	int beforeSpike; // number of records to consider before spike
	int afterSpike; // number of records to consider after spike
	int peakPosition; // peak position within the waveform
	int spikeLength; // number of records in a spike
	int nComponents; // number of principal components
	bool isCenteredData; // use or not centered data for the projection
	int offset; // Offset at the beginning of the datas
	
	bool isInputFileProvided;
	bool isOutputFileProvided;
	bool isInputSizeProvided;
	bool isNChannelsProvided;
	bool isBeforeSpikeProvided;
	bool isAfterSpikeProvided;
	bool isPeakPositionProvided;
	bool isSpikeLengthProvided;
	bool isNComponentsProvided;
	bool isExtraFeaturesProvided;
	bool isOffsetProvided;
#ifdef NBITS
	int nBits; // Number of bits for datas (resolution)
	bool isNBitsProvided;
#endif
};


// Parse arguments and fill corresponding variables
void parseArgs(const int, char **, arguments &);
// Check arguments values
bool checkInputs(const arguments);

#endif
