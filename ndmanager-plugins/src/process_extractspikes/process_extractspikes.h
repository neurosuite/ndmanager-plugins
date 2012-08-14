/***************************************************************************
                          process_extractspikes.h  -  description
                             -------------------
    copyright            : (C) 2007 by Nicolas Lebas
    email                : nicolas.lebas@college-de-france.fr


 * Description : process_extractspikes is a program which detect and extract spikes from a
 * filtered electrophysiologic record (following a given threshold).
 *
 * Usage : process_extractspikes [options] [input]
 *
 ***************************************************************************/

/***************************************************************************
 *
 *   This file is a part of the free software process_extractspikes ; you can redistribute it and/or
 *   modify it under the terms of the GNU General Public License as published by the Free
 *   Software Foundation; either version 3 of the License, or (at your option) any later version.
 *
 ***************************************************************************/
#ifndef __PROCESS_DETECT_SPK_H
#define __PROCESS_DETECT_SPK_H

#define BUFFER_CHANNEL_SIZE 50000
#define MAX_CHANNO 128 // max channel number
#define RECORD_BYTE_SIZE 2
#define GROUP_SEPARATOR ":"
#define CHANNEL_SEPARATOR ","
#define SPIKE_TIME_OUT_EXT "res"
#define SPIKE_REC_OUT_EXT "spk"

#include <iostream>
using namespace std;


// Structure for all arguments
struct arguments {
	char *inputFileName; // Input files name
	char *outputBaseFileName; // Ouput base name (i.e without extension)
	int peakLength; // Timestep for a spike duration
	int refractoryPeriod; // Timestep for the refractory period
	int totalChannelNumber; // Total number of channels
	char* channelList;	// List of channels for each group (comma separated integers between
										// channel numbers in a group and semicolon separated between two groups)
	char* thresList;	// List of thresholds for each group (comma separated integers between
									// thresholds in a group and semicolon separated between two groups)
	int spikeLength; // Extract window length
	int timeBeforeSpike; // Time before spike to extract

	bool isInputFileProvided, isOutputBaseFileProvided;
	bool isPeakLengthProvided, isRefractoryPeriodProvided;
	bool isTotalChannelNumberProvided;
	bool isChannelListProvided;
	bool isThresListProvided;
	bool isSpikeLengthProvided;
	bool isTimeBeforeSpikeProvided;
	bool isDisableAbs;
#ifdef NBITS
	int offset; // Offset at the beginning of the datas
	int nBits; // Number of bits for datas (resolution)
	bool isOffsetProvided;
	bool isNBitsProvided;
#endif
};

// Write a spike apparition time (in timestamp)
bool writeTimeSpike(const off_t, const int, FILE*);
// Determine if the refractory period is resolved
bool isRefractoryPeriod(const off_t, const off_t, const arguments);
// Does the max is a peak or an artefact ?
bool isRealPeak(const double, const double, const double);
// Give the full id of a supposed spike (keep the negative peak if it is possible). Return -1 if there is no peak.
off_t getNegativePeakFullId(	const int, const short*, const bool, const int, const int, const short*, const bool,
						const int, const int, const int, const int, const double, const double, bool &);
// Give the full id of a supposed spike. Return -1 if there is no peak.
off_t getPeakFullId(const int, const short*, const int, const int, const short*, const int, const int, const int,
				const int, const double, const double, bool &);
// Search the maximum negative (or positive if there is not) over-threshold value in a given
// interval for a given channel.
int lookForMaxNegative(	const short *, const int, const int, int &, const int, const int *, const int, const double *,
					const short[], bool &);
// Look for the max absolute value index in a given interval for a channel
int lookForMax(const short *, const int, const int, int &, const int, const int *, const int, const short[]);
// Look for a spike at the current time
int searchSpikeInChannels(const short *, const int, const int *, const double *, bool &);
// Give the threshold value corresponding to a given channel
double getThresholdFromChan(const int, const int, const int *, const double *);
// Parse arguments and store them in arguments
void parseArgs(const int, char**, arguments &);
// Check arguments values
bool checkInputs(const arguments, const int, const FILE *);
// Check channels and thresholds lists
bool checkChanAndThres(int **, int *, int, int *, int);
// Extract thresholds from arguments
int getThresholdsFromArg(int *, double **, const arguments);
// Extract channels ids from arguments
int getChannelsFromArg(int *, int **, const arguments);

#endif
