/***************************************************************************
                          process_extractspikes.cpp  -  description
                             -------------------
    copyright            : (C) 2007 by Nicolas Lebas
    email                : nicolas.lebas@college-de-france.fr

	 Enabling LFS + code cleaning
    copyright            : (C) 2008 by Michaël Zugaro
    email                : michael.zugaro@college-de-france.fr


 * Description : process_extractspikes is a program which detect and extract 
 * spikes from a filtered electrophysiologic record (following a given
 * threshold).
 *
 * Usage : process_extractspikes [options] [input]
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

#define _LARGEFILE_SOURCE64
#define _FILE_OFFSET_BITS 64

#include "process_extractspikes.h"
#include <cmath>
#include <cstdlib>
#include <cstring>

#include <sstream>
#include <fstream>

int buffer_size = BUFFER_CHANNEL_SIZE; // total buffer size
static bool verbose = false;
const char *program_version = "process_extractspikes 0.8 (01-12-2010)";

/** Usage error **/
void error(const char* name) {
	cerr << program_version << endl;
	cerr << "usage: " << name << " [options] basename (type '" << name 
	<< " -h' for details)" << endl;
	exit(1);
} // error


/** Usage information **/
void help(const char* name) {
	cout << name << " -- Detect and extract spikes from high-pass filtered file"
	<< endl;
	cout << "usage: " << name << " [options] basename" << endl;
	cout << "usage: " << name << " [options] basename -" << endl;
	cout << "(use the second form to read from stdin)" << endl;
	cout << " basename\t\tSession file base name (i.e. without extention)" 
	<< endl;
	cout << " -t thresholds\t\tList of thresholds for spike detection "
	<< "(e.g. 10.4,11.2,10:54.1,51,57.2 for 2 groups with 3 channels each)"
	<< endl;
	cout << " -r refractoryPeriod\tRefractory period (i.e minimum time between"
	<< " 2 spikes)" << endl;
	cout << " -w waveformLength\tNumber of samples per waveform" << endl;
	cout << " -p peakSample\t\tPosition of the peak within the waveform, in"
	"number of samples" << endl;
	cout << " -n nChannels\t\tTotal number of channels in the file" << endl;
	cout << " -c channels\t\tList of channels for each group "
	<< "(e.g. 0,1,2:4,5,7 for 2 groups with 3 channels each)" << endl;
#ifdef NBITS
	cout << " -o offset\t\tOffset value" << endl;
	cout << " -b bits\t\tResolution of the acquisition system (in bits)"
	<< endl;
#endif
	cout << " -l peakSearchLength\t(size of the window within which to look for"
	<< " the peak; do not change this unless you know what you are doing)" 
	<< endl;
//	cout << " -a look for the maxi absolute value to look for a spike peak" 
//	<< endl; // Cheat option
	cout << " -v\t\t\tverbose mode" << endl;
	cout << " -h\t\t\tdisplay help" << endl;
	cout << endl << "All arguments are mandatory (except -l peakSearchLength";
#ifdef NBITS
	cout << " and -b bits if the resolution is 16bits";
#endif
	cout << ")." << endl;
	exit(0);
} // help


// Start here
int main(int argc,char *argv[]) {
	struct arguments arguments;
	arguments.totalChannelNumber = 0;
	arguments.peakLength = -1;
	arguments.refractoryPeriod = -1;
	arguments.spikeLength = -1;
	arguments.timeBeforeSpike = -1;

	arguments.isInputFileProvided = false;
	arguments.isOutputBaseFileProvided = false;
	arguments.isPeakLengthProvided = false;
	arguments.isRefractoryPeriodProvided = false;
	arguments.isTotalChannelNumberProvided = false;
	arguments.isChannelListProvided = false;
	arguments.isThresListProvided = false;
	arguments.isSpikeLengthProvided = false;
	arguments.isTimeBeforeSpikeProvided = false;
	arguments.isDisableAbs = true; // use real value as default
#ifdef NBITS
	arguments.offset = 0;
	arguments.nBits = int(RECORD_BYTE_SIZE*8);
	arguments.isNBitsProvided = false;
	arguments.isOffsetProvided = false;
#endif
	
	// buffers with current, previous and next datas value
	short *cur_buffer, *prev_buffer, *nextRec;
	// I/O files
	FILE *inputFile, **spikeTimeOutputFile, **spikeOutputFile;
	// Time ouput files name
	string *spikeTimeOutputFileName;
	// Length to extract after spike peak
	int timeAfterSpike = 0;
	unsigned int *nSpikeTot, nRecTot = 0;
	// nb of read records and nb of read buffers (to det the full position).
	unsigned long long rec_nb = 0, nbLoops = 0;
	// nb of record concerning the current spike in previous buffer
	int *recInPrevBuffer;
	// nb of rec to ignore in next buffer (too close of last peak)
	int *ignoredInNextBuffer;
	// current and previous buffer max. value index
	int *maxId, *prevBuffer_maxID;
	// is current and previous buffer max value negative
	bool *isNegativeMax, *prevBuffer_isNegativeMax;
	// is max in current buffer
	bool *isMaxInCurBuf;
	// Index of previous spike (calculate from the beginning of all datas)
	off_t *lastSpikeFullId;
	// Id of the channel containing a spike in previous buffer (-1 if it is not)
	int *prevBuffer_spkChanId;
	// Id of the channel containing a possible spike (-1 if there is not)
	int *spkChanId;
	// TRUE if this is the last reading loop in input file, FALSE else
	bool isLastLoop = false;
	
	// Total Channels number
	int channelListSize = 0;
	// Number of electrodes groups
	int nbGroups = 0;
	// Store all channels Ids
	int **channelList = new int* [MAX_CHANNO];
	// store number of channels in each group
	int *channelNb_group = new int[MAX_CHANNO];
	
	// Number of threshold groups
	int nbThres = 0;
	// Store all thresholds
	double **thresList = new double* [MAX_CHANNO];
	// store number of thresholds in each group
	int *thresNb_group = new int[MAX_CHANNO];

#ifdef DEBUG
	FILE *debugFile = fopen("process_extractspikes.log", "w");
#endif

	parseArgs(argc, argv, arguments); // get arguments
	buffer_size *= arguments.totalChannelNumber; // increase buffer size
	inputFile = fopen(arguments.inputFileName,"rb");

	if ( verbose ) {
		cout << endl;
		cout << "Input File           = ";
		if (arguments.isInputFileProvided)
			cout << arguments.inputFileName << endl;
		else cout << "N/A" << endl;
		cout << "Output Base Name     = " << arguments.outputBaseFileName 
		<< endl;
		cout << "Peak Search Length   = " << arguments.peakLength << endl;
		cout << "Refractory Period    = " << arguments.refractoryPeriod << endl;
		cout << "Waveform Length      = " << arguments.spikeLength << endl;
		cout << "Peak Position        = " << arguments.timeBeforeSpike << endl;
		cout << "Disable absolute value for threshold = "
		<< arguments.isDisableAbs << endl;
#ifdef NBITS
		cout << "Offset                = ";
		if (arguments.isOffsetProvided) cout << arguments.offset << endl;
		else cout << "N/A" << endl;
		cout << "Resolution            = ";
		if (arguments.isNBitsProvided)
			cout << arguments.nBits << " bits" << endl;
		else cout << "N/A" << endl;
#endif
		cout << "Buffer Size           = " << BUFFER_CHANNEL_SIZE << endl;
		cout << endl;
	} // if verbose

	if(!checkInputs(arguments, buffer_size, inputFile)) // check arguments value
		exit(1);

	// init arrays
	for (int i=0;i<MAX_CHANNO;i++) {
		channelList[i]=new int[MAX_CHANNO]; // Channels
		channelNb_group[i] = -1;

		thresList[i]=new double[MAX_CHANNO]; // Thresholds
		thresNb_group[i] = -1;

		for (int j=0;j<MAX_CHANNO;j++) {
			channelList[i][j]=-1;
			thresList[i][j]=-1;
		} // for j
	} // for i

	// time to keep after spike peak (-1 due to the max record).
	timeAfterSpike = arguments.spikeLength - arguments.timeBeforeSpike - 1;

	// get channels groups
	nbGroups = getChannelsFromArg(channelNb_group, channelList, arguments);
	// get threshold groups
	nbThres = getThresholdsFromArg(thresNb_group, thresList, arguments);

	if(!checkChanAndThres(channelList, channelNb_group, nbGroups, 
						 thresNb_group, nbThres)) {
		cerr << "error: problem with lists of channels and/or thresholds." 
		<< endl;
		return 1;
	} // if

	/* init buffers */
	
	// +totChN for next Rec
	cur_buffer = new short[buffer_size+arguments.totalChannelNumber];
	prev_buffer = new short[buffer_size];
	// store next record
	nextRec = new short[arguments.totalChannelNumber];

	// init variables
	
	// id of the previous spike / group
	prevBuffer_spkChanId = new int[nbGroups];
	// id of the current spike / group
	spkChanId = new int[nbGroups];
	// is a negative max detect / group
	isNegativeMax = new bool[nbGroups];
	// is a negative max in previous buffer / group
	prevBuffer_isNegativeMax = new bool[nbGroups];
	// the peak value in current buffer / group
	isMaxInCurBuf = new bool[nbGroups];
	// id for the peak in current spike / group
	maxId = new int[nbGroups];
	// id for the current peak in previous buff/group
	prevBuffer_maxID = new int[nbGroups];
	// nb of records in previous buffer / group
	recInPrevBuffer = new int[nbGroups];
	// nb of records to ignore in next buffer
	ignoredInNextBuffer = new int[nbGroups];
	// last spike peak full id / group
	lastSpikeFullId = new off_t[nbGroups];
	// total nb of spike / group
	nSpikeTot = new unsigned int[nbGroups];

	for (int g =0; g < nbGroups; g++) {
		prevBuffer_spkChanId[g] = -1;
		spkChanId[g] = -1;
		isNegativeMax[g] = false;
		prevBuffer_isNegativeMax[g] = false;
		isMaxInCurBuf[g] = true;
		maxId[g] = -1;
		prevBuffer_maxID[g] = -1;
		recInPrevBuffer[g] = 0;
		ignoredInNextBuffer[g] = 0;
		lastSpikeFullId[g] = -1;
		nSpikeTot[g] = 0;
	} // for g

	if ( verbose ) {
		for (int g =0; g < nbGroups; g++) {
			cout << "Group #"<<g+1<<" : "<< channelNb_group[g]
			<< "channels" << endl;
			cout << "IDs : ";
			for (int i=0; i < channelNb_group[g]; i++)
				cout << channelList[g][i] << " ";
			cout << endl;
		} // for g
		cout << endl << "<<----- Begin Spike Time Extraction" << endl;
		cout << endl;
	} // if verbose

	spikeTimeOutputFile = new FILE*[nbGroups];
	spikeTimeOutputFileName = new string[nbGroups];
	spikeOutputFile = new FILE*[nbGroups];
	
	/************** SPIKES TIME DETECTION **************/
	// Open positions output files
	for (int grp=0; grp<nbGroups; grp++) {
		ostringstream oss;

		// Output file name for spikes time
		oss << arguments.outputBaseFileName << "." << SPIKE_TIME_OUT_EXT << ".";
		oss << (grp+1);
		spikeTimeOutputFileName[grp] = oss.str();
		spikeTimeOutputFile[grp] = fopen(spikeTimeOutputFileName[grp].c_str(), 
										 "w");

		if(verbose)
			cout << "Positions output file for group #"<<grp+1<<" : " 
			<< spikeTimeOutputFileName[grp] << endl;
	} // for grp

	// Search spikes position loop
	while(!feof(inputFile) && !isLastLoop) {

		if(nbLoops == 0) {
			// Store 1st records in nextRec for the init phase
			if(arguments.isInputFileProvided) {
				int nRec = fread(nextRec, sizeof(short), 
								 arguments.totalChannelNumber, inputFile);
			} else {
				int nRec = fread(nextRec, sizeof(char), 
								 arguments.totalChannelNumber, stdin);
			} // else
		} else {
			// store previous buffer
			memcpy(prev_buffer, cur_buffer, sizeof(short)*buffer_size);
		} // if nbLoops

		// Get a datas buffer
		if(arguments.isInputFileProvided) {
			rec_nb = fread(cur_buffer+arguments.totalChannelNumber,
						   sizeof(short), buffer_size, inputFile);
		} else {
			rec_nb = fread(cur_buffer+arguments.totalChannelNumber,
						   sizeof(char), buffer_size, stdin);
		} // else
		
		// add the first record (ie the old "next record")
		memcpy(cur_buffer, nextRec, sizeof(short)*arguments.totalChannelNumber);
		
		// there is no next record (ie = last records)
		if(rec_nb < buffer_size || feof(inputFile)) {
			isLastLoop = true;
			nextRec = NULL;
			rec_nb += arguments.totalChannelNumber; // correct records number
		}
		else { // store next record
			memcpy(nextRec, cur_buffer+buffer_size, 
				   sizeof(short)*arguments.totalChannelNumber);
		} // if rec_nb
		
		/* Analyse each channel group */
		for(int grp = 0; grp < nbGroups; grp++) {
			int i = 0;
			// start analyzing after the given time before spike (only at the
			// begening)
			if(nbLoops == 0)
				i = arguments.timeBeforeSpike*arguments.totalChannelNumber;
			else
				i = ignoredInNextBuffer[grp]*arguments.totalChannelNumber;
					//+ arguments.totalChannelNumber; // start after ignored rec
			
			ignoredInNextBuffer[grp] = 0; // reset nb of next buffer ignored rec
			
			// read buffer
			while (i < rec_nb) {
				
				if(spkChanId[grp] == -1) {// no spike actually
					spkChanId[grp]=searchSpikeInChannels(&cur_buffer[i], 
														 channelNb_group[grp],
														 channelList[grp],
														 thresList[grp],
														 isNegativeMax[grp]);
				}
				
				if(spkChanId[grp] > -1) {// threshold is reached
					// Max index for detecting a maximum (~peak)
					int maxEndSpike = i
					+ (arguments.peakLength-recInPrevBuffer[grp]-1)
					* arguments.totalChannelNumber;
					
					// Searching the max id in cur buffer (RELATIVE index /!\)
					if(maxEndSpike >= rec_nb) { // end of spike in next buffer
						// previous record
						short prevVals[arguments.totalChannelNumber];
						memcpy(prevVals, cur_buffer+i-arguments.totalChannelNumber,
							   arguments.totalChannelNumber*sizeof(short));
						
						if(arguments.isDisableAbs) {
							maxId[grp] = lookForMaxNegative(cur_buffer, i, 
												rec_nb-1,
												spkChanId[grp],
												channelNb_group[grp], 
												channelList[grp],
												arguments.totalChannelNumber,
												thresList[grp], prevVals,
												isNegativeMax[grp]);
							prevBuffer_isNegativeMax[grp] = isNegativeMax[grp];
							isNegativeMax[grp] = false;
							
						} else {
							maxId[grp] = lookForMax(cur_buffer, i, rec_nb-1, 
												spkChanId[grp],
												channelNb_group[grp],
												channelList[grp],
												arguments.totalChannelNumber,
												prevVals);
						}
						
						if(maxId[grp] != -1) {
							double prevVal = cur_buffer[(maxId[grp]
								- arguments.totalChannelNumber
								+ spkChanId[grp])];
							
							double nextVal; // rec after max
							// next rec is current buffer 1st rec
							if((maxId[grp] + 
								arguments.totalChannelNumber) ==
								buffer_size) {
								
								nextVal = nextRec[spkChanId[grp]];
							}
							// next rec is in previous buffer
							else {
								nextVal = cur_buffer[(maxId[grp]
									+ arguments.totalChannelNumber
									+ spkChanId[grp])];
							}
							
							if(isRealPeak(cur_buffer[maxId[grp]+spkChanId[grp]],
										prevVal,
										nextVal) ) {
								prevBuffer_maxID[grp] = maxId[grp];
								prevBuffer_spkChanId[grp] = spkChanId[grp];
							} // if isRealPeak
						} // if maxId != -1
						recInPrevBuffer[grp] = (rec_nb-i) /
												arguments.totalChannelNumber;
						maxId[grp] = -1;  // reset MAX value for next buffer
						i = buffer_size; // go to the end of the buffer
						
					// spike is in current buffer (if it is a significant one)
					} else {
						off_t maxFullId = -1;
						// previous records
						short prevVals[arguments.totalChannelNumber];
						
						// previous record in current buffer
						if(i >= arguments.totalChannelNumber) {
							memcpy(prevVals, cur_buffer+i-arguments.totalChannelNumber,
									arguments.totalChannelNumber*sizeof(short));
						} else { // previous record is the last of previous buffer
							memcpy(prevVals, prev_buffer+buffer_size
									-arguments.totalChannelNumber, 
									arguments.totalChannelNumber*sizeof(short));
						}
						
						// Look prioritary for a negative peak
						if(arguments.isDisableAbs) {
							maxId[grp] = lookForMaxNegative(cur_buffer, i, 
												maxEndSpike,
												spkChanId[grp],
												channelNb_group[grp],
												channelList[grp],
												arguments.totalChannelNumber,
												thresList[grp], prevVals,
												isNegativeMax[grp]);
							
							double thr = getThresholdFromChan(spkChanId[grp], 
												arguments.totalChannelNumber,
												channelList[grp], 
												  thresList[grp]);
							double prevBuffer_thr = -1;
							if(prevBuffer_spkChanId[grp] != -1) {
								prevBuffer_thr = getThresholdFromChan(
												prevBuffer_spkChanId[grp],
												arguments.totalChannelNumber,
												channelList[grp],
												thresList[grp]);
							}
							
							maxFullId = getNegativePeakFullId(maxId[grp], 
												cur_buffer,
												isNegativeMax[grp],
												spkChanId[grp],
												prevBuffer_maxID[grp], 
												prev_buffer,
												prevBuffer_isNegativeMax[grp],
												prevBuffer_spkChanId[grp],
												buffer_size,
												arguments.totalChannelNumber,
												nbLoops, thr, prevBuffer_thr,
												isMaxInCurBuf[grp]);
						}
						else { // Use absolute value for peak detection
							maxId[grp] = lookForMax(cur_buffer, i, maxEndSpike, 
												spkChanId[grp],
												channelNb_group[grp],
												channelList[grp],
												arguments.totalChannelNumber,
												prevVals);
							
							double thr = getThresholdFromChan(spkChanId[grp],
												arguments.totalChannelNumber,
												channelList[grp],
												thresList[grp]);
							
							double prevBuffer_thr = -1;
							if(prevBuffer_spkChanId[grp] != -1) {
								prevBuffer_thr =  getThresholdFromChan(
												prevBuffer_spkChanId[grp],
												arguments.totalChannelNumber,
												channelList[grp],
												thresList[grp]);
							}
							
							maxFullId = getPeakFullId(maxId[grp], cur_buffer, 
												spkChanId[grp],
												prevBuffer_maxID[grp],
												prev_buffer,
												prevBuffer_spkChanId[grp],
												buffer_size,
												arguments.totalChannelNumber,
												nbLoops,
												thr, prevBuffer_thr,
												isMaxInCurBuf[grp]);
						} // if isDisableAbs
						
						// true if new spike detection start
						bool isKeepPrevMax = false;
						
						// there is maybe a spike
						if(maxFullId > -1)  {
							// There is a spike !
							if(!isRefractoryPeriod( lastSpikeFullId[grp],
													maxFullId,
													arguments)) {
								// can extract spike (not eof)
								if(!(isLastLoop &&
									isMaxInCurBuf[grp] &&
									(maxId[grp] +
									(timeAfterSpike *
									arguments.totalChannelNumber)) >= rec_nb)) {
									
										writeTimeSpike( maxFullId,
												arguments.totalChannelNumber,
												spikeTimeOutputFile[grp]);
								      lastSpikeFullId[grp] = maxFullId;
								      nSpikeTot[grp]++; // One spike more !
								} // if(!(isLastLop & ...) // not eof
							} // if isRefractoryPeriod
							
							// next spike detection is in current or next buffer
							if(isMaxInCurBuf[grp]) {
								maxEndSpike = maxId[grp] + (arguments.peakLength
											- arguments.timeBeforeSpike - 1)
											* arguments.totalChannelNumber;
								
								if(maxEndSpike >= buffer_size) {
									ignoredInNextBuffer[grp] = 1
												+ (maxEndSpike - buffer_size)
												/ arguments.totalChannelNumber;
								}
							}
							// next spk detection in prev buffer or in cur one
							else {
								// end of last detected spike
								maxEndSpike = prevBuffer_maxID[grp] 
											+ (arguments.peakLength 
											- arguments.timeBeforeSpike - 1) 
											* arguments.totalChannelNumber;
								
								// new spike search in current buffer
								if((maxEndSpike+arguments.totalChannelNumber) >=
									buffer_size) {
									// end of previous spike (new search :
									// i+totalChannelNumber)
									maxEndSpike -= buffer_size;
									isKeepPrevMax = false;
								}
								// new spike search start in previous buffer
								else {
									// start of new spike search
									int x = maxEndSpike
											+ arguments.totalChannelNumber;
									// no more MAX value
									maxId[grp] = -1; isNegativeMax[grp] = false;
									prevBuffer_spkChanId[grp] = -1;
									prevBuffer_maxID[grp] = -1;
									spkChanId[grp] = -1; // no more channel
									
									while(x < buffer_size && 
										  spkChanId[grp] == -1) {
												spkChanId[grp]=
													searchSpikeInChannels(
														&prev_buffer[x],
														channelNb_group[grp],
														channelList[grp],
														thresList[grp],
														isNegativeMax[grp]);
										x += arguments.totalChannelNumber;
									} // while
									
									// new spk start in prev buffer
									if(spkChanId[grp] != -1) {
										// index of the 1st record > thr
										x -=  arguments.totalChannelNumber;
										// previous record
										short prevVals[arguments.totalChannelNumber];
										memcpy(prevVals, prev_buffer+x-arguments.totalChannelNumber,
												arguments.totalChannelNumber*sizeof(short));
										
										if(arguments.isDisableAbs) {
											maxId[grp] = lookForMaxNegative(
												prev_buffer, x, buffer_size-1,
												spkChanId[grp], 
												channelNb_group[grp],
												channelList[grp],
												arguments.totalChannelNumber,
												thresList[grp], prevVals,
												isNegativeMax[grp]);
											prevBuffer_isNegativeMax[grp] = 
															isNegativeMax[grp];
										} else {
											maxId[grp] = lookForMax(prev_buffer,
												x, buffer_size-1,
												spkChanId[grp],
												channelNb_group[grp], 
												channelList[grp],
												arguments.totalChannelNumber,
												prevVals);
										} // if isDisableAbs
										
										if(maxId[grp] != -1) {
											double prevVal = prev_buffer[(maxId[grp]
												- arguments.totalChannelNumber
												+ spkChanId[grp])];
											
											double nextVal; // rec after max
											// next rec is current buffer 1st rec
											if((maxId[grp] + 
												arguments.totalChannelNumber) ==
												buffer_size) {
												
												nextVal=cur_buffer[spkChanId[grp]];
											}
											// next rec is in previous buffer
											else {
												nextVal = prev_buffer[(maxId[grp]
													+ arguments.totalChannelNumber
													+ spkChanId[grp])];
											}
											
											if(isRealPeak(
												prev_buffer[maxId[grp]+
												spkChanId[grp]],
												prevVal,
												nextVal)) {
												
												prevBuffer_maxID[grp] = maxId[grp];
												prevBuffer_spkChanId[grp] = 
																	spkChanId[grp];
											} // if isRealPeak
										} // if maxId != -1
										recInPrevBuffer[grp] = (buffer_size - x)
												/ arguments.totalChannelNumber;
										// do not initialize previous buffer par
										isKeepPrevMax = true;
									} // if spkChanId != -1
									// reset to the 1st rec of cur buffer
									// (add totalChannelNumber to this value)
									maxEndSpike = -arguments.totalChannelNumber;
								} // if spike in current buffer
							} // if isMaxInCurBuf
						} // if maxFullId
						
						i = maxEndSpike;
						
						// Reset all variables
						maxId[grp] = -1;  // no more MAX value
						// no more MAX value
						isNegativeMax[grp] = false;  isMaxInCurBuf[grp] = true;
						maxEndSpike = -1; isMaxInCurBuf[grp] = true;
						if(!isKeepPrevMax) {
							spkChanId[grp] = -1; prevBuffer_spkChanId[grp] = -1;
							// nothing in old buffer
							recInPrevBuffer[grp] = 0; 
							prevBuffer_maxID[grp] = -1; 
							prevBuffer_isNegativeMax[grp] = false;
						}
					} // if maxEndSpike >= rec_nb (spike in current buffer)
				} // if spkChanId[grp] > -1
				i+=arguments.totalChannelNumber;
			} // while i (current buffer)
		} // for grp

		nRecTot += rec_nb/arguments.totalChannelNumber;
		nbLoops++; // one more loop
	} // while (inputFile)

	for(int g =0; g<nbGroups; g++) {
		fclose( spikeTimeOutputFile[g] ); // close spikes time file
	} // for g

	cout << "Number of spikes:" << endl;
	for (int g = 0; g < nbGroups; g++)
		cout << "- Group " << g << " = " << nSpikeTot[g] << endl;
	if ( verbose ) {
		cout << "Total number of records : " << nRecTot << endl;
		cout << endl << " End Of Spike Time Extraction ----->>" << endl;
		cout << endl;
		cout << "Total number of channels  = " << arguments.totalChannelNumber
		<< endl;
		cout << "List of channels          = " << arguments.channelList << endl;
		cout << "List of thresholds        = " << arguments.thresList << endl;
		cout << "Number of groups          = " << nbGroups << endl;
		cout << "Number of channels        = " << channelListSize << endl;
		cout << "Number of loops           = " << nbLoops << endl;
	}

	delete[] cur_buffer; delete[] prev_buffer; delete[] nextRec;
	delete[] spkChanId; delete[] prevBuffer_spkChanId; delete[] isNegativeMax;
	delete[] maxId; delete[] prevBuffer_maxID;
	delete[] prevBuffer_isNegativeMax; delete[] isMaxInCurBuf;
	delete[] recInPrevBuffer; delete[] ignoredInNextBuffer;
	delete[] lastSpikeFullId; delete[] nSpikeTot;

	/************** SPIKE RECORDS EXTRACTION **************/
	if ( verbose ) {
		cout << endl << "<<----- Begin Spike Records Extraction " << endl;
		cout << endl;
	} // end if
	
	char line[12];
	unsigned long spkBegin = -1;
	cur_buffer = new short[buffer_size];
	spikeTimeOutputFile = new FILE*[nbGroups];

	for(int grp = 0; grp < nbGroups; grp++) {
		ostringstream oss;

		// Re-open spike times file, but in read access only
		spikeTimeOutputFile[grp] = fopen(spikeTimeOutputFileName[grp].c_str(), 
										 "r");

		// Open output file for spikes extraction
		oss << arguments.outputBaseFileName << "." << SPIKE_REC_OUT_EXT << ".";
		oss << (grp+1);
		spikeOutputFile[grp] = fopen((oss.str()).c_str(), "wb");

		if(verbose) {
			cout << "Input file ''" << spikeTimeOutputFileName[grp] << "''" 
			<< endl;
			cout << "Output file for group "<<grp+1<<": " << oss.str() << endl;
		} // if verbose

		// init
		rewind (inputFile);
		spkBegin = -1;

		// Search spikes position loop
		while(fgets(line, 12, spikeTimeOutputFile[grp]) != NULL) {
			// Determine the start position, in the input file, for current spk
			spkBegin = atoi(line) - arguments.timeBeforeSpike;

			// Place the file pointer at the beginning of the spike to extract 
			// and read datas
			fseeko	(inputFile, (((off_t) spkBegin) *
					((off_t) arguments.totalChannelNumber)) *
					((off_t) sizeof(short)),
					 SEEK_SET );
			rec_nb = fread(cur_buffer, sizeof(short),
(arguments.spikeLength*arguments.totalChannelNumber), inputFile);

#ifdef DEBUG
			// Store in LOG file extracted records
			fprintf(debugFile, "Position: %d, Line: %d\n", 
					(spkBegin*arguments.totalChannelNumber), atoi(line));

			for (int i=0; i < rec_nb; i++)
				fprintf(debugFile, "%d, ", cur_buffer[i]);
			fprintf(debugFile, "\n");
#endif

			// There is a mismatch between spk length to extract and read rec nb
			if(rec_nb != (arguments.spikeLength*arguments.totalChannelNumber)) {
				cerr << "error: cannot extract full spike at line " << line 
				<< ". Please check input file: " << arguments.inputFileName 
				<< endl;
				fclose(spikeOutputFile[grp]);

				return 1;
			} // if rec_nb

			// write records of current spike (only corresponding channels)
			for(unsigned int r = 0; r < rec_nb; 
				r += arguments.totalChannelNumber) {
				for(int c = 0; c < channelNb_group[grp]; c++) {
					int nWr = fwrite(cur_buffer+r+channelList[grp][c], 
									 sizeof(short), 1, spikeOutputFile[grp]);
				} // for c
			} // for r

		} // while inputFile

		fclose( spikeOutputFile[grp] ); // close output file
		
	} // for grp

	if ( verbose ) {
		cout << endl << " End Of Spike Records Extraction ----->>" << endl;
		cout << endl;
	} // end if
	
	//************** FREE VARIABLES **************
	if(arguments.isInputFileProvided)
		fclose ( inputFile );

	for(int g =0; g<nbGroups; g++) {
		channelListSize += channelNb_group[g]; // Total number of given channels
   		fclose( spikeTimeOutputFile[g] ); // close spikes time file
	} // for g
	
	// Free Memory
	delete[] channelNb_group;
	delete[] thresNb_group;
	for(int i=0;i<MAX_CHANNO;i++) {
		delete[] channelList[i];
		delete[] thresList[i];
	}
	delete[] channelList;
	delete[] thresList;

	delete[] spikeTimeOutputFileName; delete[] spikeTimeOutputFile;
	delete[] spikeOutputFile;

	if ( verbose ) cout << endl;

#ifdef DEBUG
	fclose(debugFile);
#endif

	return 0;
} // main


/**
 * Determine if the refractory period is resolved (need to be strictly higher 
 * than refractory period).
 * @param lastSpikeFullId Last spike peak absolute index
 * @param curSpikeFullId Current peak absolute index
 * @param args Arguments values
 * @return TRUE if the refractory period is not resolved 
 * (d <= refractoryPeriod), FALSE else.
 */
bool isRefractoryPeriod(const off_t lastSpikeFullId, const off_t curSpikeFullId,
						const arguments args) {
	off_t diff = curSpikeFullId-lastSpikeFullId;

	if(diff/int(args.totalChannelNumber) > args.refractoryPeriod ||
		lastSpikeFullId < 0) {
		return false;
	}
	else {
		return true;
	}
} // isRefractoryPeriod


/**
 * Write the time for a peak
 * @param iPeak Spike peak absolute index
 * @param nChanTot Total channels number
 * @param output Output to write
 * @return TRUE if there was no problem, FALSE else
 */
bool writeTimeSpike(const off_t iPeak, const int nChanTot, FILE *output) {
	if(output == NULL || iPeak < 0 || nChanTot < 1)
		return false;

	off_t i = iPeak/nChanTot; // Index of the peak (in timestamp)
	fprintf(output,"%lld\n",i);

	return true;
} // writeTimeSpike


/**
 * The maximum value is a peak or not ?
 * @param peak value of the supposed peak
 * @param beforePeak value of the record before the supposed peak
 * @param afterPeak value of the record after the supposed peak
 * @return TRUE if the max is a peak, FALSE else.
 */
bool isRealPeak(const double peak, const double beforePeak, 
				const double afterPeak) {

	if(peak == 0) {
		if(beforePeak > 0 && afterPeak < 0)
			return false;
		else if(beforePeak < 0 && afterPeak > 0)
			return false;
	}
	else if(peak > 0) {
		// end or beginning of a spike (not a real max)
		if(beforePeak > peak || afterPeak > peak)
			return false;
	}
	else if(peak < 0) {
		// end or beginning of a spike (not a real max)
		if(beforePeak < peak || afterPeak < peak)
			return false;
	}

	return true;
} // isRealPeak


/**
 * Look for the the max negative value in spike (or positive if there is non 
 * over-threshold negative value), and estimate if this is a real peak or an
 * artefact.
 * @param maxID Index of the max value in current buffer
 * @param cur_buf Current datas buffer
 * @param isNegativeMax Is the extremum in current buffer negative
 * @param chanId The channel Id for the max in current buffer
 * @param prevBuf_maxID Index of the max value in previous buffer
 * @param prev_buf Previous datas buffer
 * @param prevBuf_isNegativeMax Is the extremum in previous buffer negative
 * @param prevBuf_chanId The channel Id for the max in previous buffer
 * @param buf_size Length of datas buffer
 * @param nChanTot Total channels number
 * @param nLoops Number of loop before current buffer
 * @param threshold Threshold of the current buffer
 * @param prevBuf_threshold Threshold of the previous buffer
 * @param isMaxInCurBuf True if the maximum (the peak) is in the current buffer,
 * False else
 * @return the full index of the spike max or -1 if this is not a spike
 */
off_t getNegativePeakFullId(	const int maxID, const short *cur_buf, 
								const bool isNegativeMax, const int chanId,
								const int prevBuf_maxID, const short *prev_buf,
								const bool prevBuffer_isNegativeMax,
								const int prevBuf_chanId, const int buf_size,
								const int nChanTot, const int nLoops,
								const double threshold, 
								const double prevBuf_threshold, 
								bool &isMaxInCurBuf) {

	
	off_t peakFullId = -1;
	// value of the peak record
	float peakVal;
	// before and after peak records value
	float recBeforePeak = 0, recAfterPeak = 0;
	// threshold
	double thr;
	bool isPrevNull = false, isCurNull = false;
	
	// is there a max in current buffer ?
	if(maxID == -1 || chanId == -1) {
		isCurNull = true;
	}
	
	// no record or peak in previous buffer
	if(prevBuf_maxID == -1 || prevBuf_chanId == -1) {
		isMaxInCurBuf = true;
		isPrevNull = true;
	// no record in current buffer but record in previous one
	} else if(isCurNull) {
		isMaxInCurBuf = false;
	} else { // records in previous buffer
		if(prevBuffer_isNegativeMax && 
			prev_buf[prevBuf_maxID+prevBuf_chanId] <= -abs(prevBuf_threshold)) {
			// previous max value is negative (-/...)
			if(isNegativeMax) {// current max value is negative (-/-)
				// keep the min value
				if(cur_buf[maxID+chanId] < 
					prev_buf[prevBuf_maxID + prevBuf_chanId]) {
					isMaxInCurBuf = true;
				} else {
					isMaxInCurBuf = false;
				}
			} else {// current max value is positive (-/+)
				isMaxInCurBuf = false; // keep negative over-threshold value
			}
		} else { // previous value is positive (+/...)
			if(isNegativeMax) { // current max value is negative (+/-)
				// keep current negative value (if it is an over-threshold one)
				if(cur_buf[maxID+chanId] <= -abs(threshold)) {
					isMaxInCurBuf = true;
				} else {
					isMaxInCurBuf = false;
				}
			} else {// current max value is positive (+/+)
				if( cur_buf[maxID+chanId] > 
					prev_buf[prevBuf_maxID+prevBuf_chanId]) {
					isMaxInCurBuf = true;
				} else {
					isMaxInCurBuf = false;
				}
			}
		}
	} // if prevBuffer_maxID == -1
	
	// no peak to search in both prev and cur buffers
	if(isPrevNull && isCurNull) {
		return -1;
	}
	
	if(isMaxInCurBuf) { // peak in current buffer
		
		peakFullId = ((off_t)maxID) + ((off_t)nLoops)*((off_t)buf_size);
		peakVal = cur_buf[maxID+chanId];
		thr = threshold;
	} else { // peak in previous buffer
		
		peakFullId = ((off_t) prevBuf_maxID) + ((off_t)(nLoops-1)) *
					 ((off_t) buf_size);
		peakVal = prev_buf[prevBuf_maxID+prevBuf_chanId];
		thr = prevBuf_threshold;
	} // if isMaxInCurBuf
	
	if(isMaxInCurBuf) { // if peak in current buffer
		recAfterPeak = cur_buf[chanId+maxID+nChanTot]; // store next record
		if(maxID >= nChanTot) {// if previous record is in current buffer
			// store previous record
			recBeforePeak = cur_buf[chanId+maxID-nChanTot];
		}
		else {// previous record is in previous buffer
			// store previous record
			recBeforePeak = prev_buf[buf_size-nChanTot+chanId];
		}
	}
	else { // peak is in previous buffer
		// store previous record
		recBeforePeak = prev_buf[prevBuf_chanId+prevBuf_maxID-nChanTot];
		// if next record in current buffer
		if(prevBuf_maxID+nChanTot >= buf_size) {
			recAfterPeak = cur_buf[prevBuf_chanId]; // store next record
		}
		else { // next record is in previous buffer
			// store next record
			recAfterPeak = prev_buf[prevBuf_chanId+prevBuf_maxID+nChanTot];
		}
	} // if isMaxInCurBuf
	
	// Test if the max record is not an artefact
	if(isRealPeak(peakVal, recBeforePeak, recAfterPeak) && 
		abs(peakVal) >= abs(thr)) {
		return peakFullId; // real peak, so return the full id
	} else {
		return -1; // not a real peak (i.e this is an artefact), return -1
	}
} // getNegativePeakFullId


/**
 * Look for the the max value in spike, and estimate if this is a real peak or 
 * an artefact.
 * @param maxID Index of the max value in current buffer
 * @param cur_buf Current datas buffer
 * @param chanId The channel Id for the max in current buffer
 * @param prevBuf_maxID Index of the max value in previous buffer
 * @param prev_buf Previous datas buffer
 * @param prevBuf_chanId The channel Id for the max in previous buffer
 * @param buf_size Length of datas buffer
 * @param nChanTot Total channels number
 * @param nLoops Number of loop before current buffer
 * @param threshold Threshold of the current buffer
 * @param prevBuf_threshold Threshold of the previous buffer
 * @param isMaxInCurBuf True if the maximum (the peak) is in the current buffer,
 * False else
 * @return the full index of the spike max or -1 if this is not a spike
 */
off_t getPeakFullId(const int maxID, const short *cur_buf, const int chanId,
					const int prevBuf_maxID, const short *prev_buf, 
					const int prevBuf_chanId, const int buf_size, 
					const int nChanTot, const int nLoops, 
					const double threshold, const double prevBuf_threshold, 
					bool &isMaxInCurBuf) {
	off_t peakFullId = -1;
	// value of the peak record
	float peakVal;
	// before and after peak records value
	float recBeforePeak = 0, recAfterPeak = 0;
	double thr; // threshold
	
	// peak in previous buffer
	if(	prevBuf_chanId > -1 &&
		prevBuf_maxID > -1 &&
		abs(prev_buf[prevBuf_maxID+prevBuf_chanId]) >= 
		abs(cur_buf[maxID+chanId])) {
				
		isMaxInCurBuf = false;
		peakFullId = ((off_t) prevBuf_maxID)+((off_t) nLoops-1) * 
					 ((off_t) buf_size);
		peakVal = abs(prev_buf[prevBuf_maxID+prevBuf_chanId]);
		thr = prevBuf_threshold;
	} else { // peak is in current buffer
		isMaxInCurBuf = true;
		peakFullId = ((off_t) maxID)+((off_t) nLoops)*((off_t) buf_size);
		peakVal = abs(cur_buf[maxID+chanId]);
		thr = threshold;
	} // if

	if(isMaxInCurBuf) { // if peak in current buffer
		recAfterPeak = abs(cur_buf[chanId+maxID+nChanTot]); // store next record
		if(maxID >= nChanTot) // if previous record is in current buffer
			// store previous record
			recBeforePeak = abs(cur_buf[chanId+maxID-nChanTot]);
		else // previous record is in previous buffer
			// store previous record
			recBeforePeak = abs(prev_buf[buf_size-nChanTot+chanId]);
	}
	else { // peak is in previous buffer
		// store previous record
		recBeforePeak = abs(prev_buf[prevBuf_chanId+prevBuf_maxID-nChanTot]);
		// if next record in current buffer
		if(prevBuf_maxID+nChanTot >= buf_size)
			recAfterPeak = abs(cur_buf[prevBuf_chanId]); // store next record
		else // next record is in previous buffer
			// store next record
			recAfterPeak = abs(prev_buf[prevBuf_chanId+prevBuf_maxID+nChanTot]);
	} // if isMaxInCurBuf
	
	// Test if the max record is not an artefact
	if(isRealPeak(peakVal, recBeforePeak, recAfterPeak) && 
		abs(peakVal) >= abs(thr))
		return peakFullId; // real peak, so return the full id
	else
		return -1; // not a real peak (i.e this is an artefact), return -1
} // getPeakFullId


/**
 * Search the local minimum (or maximum if there is not) over-threshold value
 * in a given interval for all given channels.
 * Notice : only return the first channel index corresponding to the max, not 
 * directly chanId !
 * @param buf Datas buffer
 * @param start Begin index
 * @param stop End index
 * @param chanId Channel Id of the extremum
 * @param nChanGrp Total number of channels in the group
 * @param channelList List containing the channels Id
 * @param nChanTot Total channels number
 * @param thresholds Threshold for spikes in current group
 * @param isNegativeMax Does the current max value negative
 * @return The index of the max negative (or positive if there is not) 
 * over-threshold value
 */
int lookForMaxNegative(const short *buf, const int start, const int stop, 
					   int &chanId, const int nChanGrp, const int *channelList,
					   const int nChanTot, const double *thresholds,
					   const short prevVal[], bool &isNegativeMax) {
	if(chanId < 0) {
		cerr << "Error: negative channel ID (" << chanId << ")." << endl;
		exit(1);
	}	else if(start > stop) {
		cerr << "Error: starting point (" << start 
		<< ") greater than end point(" << stop << ")." << endl;
		exit(1);
	}
	
	int maxID = start; // param to update
	isNegativeMax = false; // param to update
	bool isFirstMax = true; // first max store
	
	// look the minimum and max for each channel
	for (int c = 0; c < nChanGrp; c++ ) {
		
		int chan = channelList[c]; // current channel to consider
		int chanMaxID = start; // index of the maximum value found
		int chanMinID = start; // index of the minimum value found
		double thr = abs(thresholds[c]); // use a positive value for threshold

		// firsts min and max ids points are extremums ?
		bool isMinPeak = isRealPeak(buf[start+chan], prevVal[chan], 
									buf[start+nChanTot+chan]);
		bool isMaxPeak = isRealPeak(buf[start+chan], prevVal[chan], 
									buf[start+nChanTot+chan]);
		
		for(int i = (start+nChanTot); i <= stop; i += nChanTot) {
			if(buf[i+chan] > buf[chanMaxID+chan] ||
				(!isMaxPeak && buf[i+chan] >= 0)) { // new maximum
				if(isRealPeak(buf[i+chan], buf[i-nChanTot+chan],
					buf[i+nChanTot+chan])) { // this is a local max
					chanMaxID = i;
					isMaxPeak = true;
				}
			}
			if(buf[i+chan] < buf[chanMinID+chan] || 
				(!isMinPeak && buf[i+chan] <= 0)) { // new minimum
				if(isRealPeak(buf[i+chan], buf[i-nChanTot+chan], 
					buf[i+nChanTot+chan])) { // this is a local min
					
					chanMinID = i;
					isMinPeak = true;
				}
			}
		} // for i
		
		// there a significant min
		if(isMinPeak && buf[chanMinID+chan] <= -thr) {
			if(isFirstMax || (buf[chanMinID+chan] < buf[maxID+chanId])) {
				isFirstMax = false;
				isNegativeMax = true;
				chanId = chan;
				maxID = chanMinID;
			} else 	if(buf[chanMinID+chan] == buf[maxID+chanId] 
					&& chanMinID < maxID) { // miss first min channel
				isFirstMax = false;
				isNegativeMax = true;
				chanId = chan;
				maxID = chanMinID;
			}// if isFirstLoop + buf... < buf...
		// keep the significant max
		} else if(!isNegativeMax && isMaxPeak && buf[chanMaxID+chan] >= thr) {
			if(isFirstMax || buf[chanMaxID+chan] > buf[maxID+chanId]) {
				isFirstMax = false;
				chanId = chan;
				maxID = chanMaxID;
			} else 	if(buf[chanMaxID+chan] == buf[maxID+chanId] 
					&& chanMaxID < maxID) { // miss first max channel
				isFirstMax = false;
				chanId = chan;
				maxID = chanMaxID;
			} // if isFirstLoop OR (isNegativeMax + buf[chanMx] > buf[max])
		}
	} // for c
	
	if(isFirstMax) {
		return -1;
	}
	
	return maxID; // return the max value (with a priority for negative value)
} // lookForMaxNegative


/**
 * Search the local max absolute value in a given interval for a given channel
 * Notice : only return the first channel index corresponding to the max, not
 * directly chanId !
 * @param buf Datas buffer
 * @param start Begin index
 * @param stop End index
 * @param chanId The channel Id
 * @param nChanGrp Total number of channels in the group
 * @param channelList List containing the channels Id
 * @param nChanTot Total channels number
 * @return The index of the max absolute value
 */
int lookForMax(const short *buf, const int start, const int stop, int &chanId, 
			   const int nChanGrp, const int *channelList, const int nChanTot,
			   const short prevVal[]) {
	if(chanId < 0) {
		cerr << "Error: negative channel ID (" << chanId << ")." << endl;
		exit(1);
	}	else if(start > stop) {
		cerr << "Error: starting point (" << start
		<< ") greater than end point(" << stop << ")." << endl;
		exit(1);
	}
	int maxID = start;
	bool isMaxPeak = false;
	
	// look the minimum and max for each group's channel
	for (int c = 0; c < nChanGrp; c++ ) {
		int chan = channelList[c]; // current channel to consider
		
		// is max id point a local max ?
		isMaxPeak = isRealPeak(abs(buf[start+chan]), abs(prevVal[chan]), 
									abs(buf[start+nChanTot+chan]));

		for(int i = (start+nChanTot); i <= stop; i += nChanTot)
			if( abs(buf[i+chan]) > abs(buf[maxID+chan]) || !isMaxPeak )
				// this is a local max
				if(isRealPeak(buf[i+chan], buf[i-nChanTot+chan], 
					buf[i+nChanTot+chan])) {
					maxID = i;
					chanId = chan;
					isMaxPeak = true;
				} // if isRealPeak
	} // for c
	
	if(!isMaxPeak) {
		maxID = -1; // FIXME : to check !!!
	}
	
	return maxID;
} // lookForMax


/**
 * Looking for a value higher than it corresponding threshold in one channel.
 * @param cur_buf The current buffer values
 * @param nbChanTot Total number of channel
 * @param channelList List containing the channels Id
 * @param thresholds Thresholds list (for each channel) to detect a spike
 * @return The id of the first channel higher than it threshold, -1 if no 
 * threshold was reached.
 */
int searchSpikeInChannels(const short *cur_buf, const int nbChanTot,
						  const int *channelList, const double *thresholds, 
						  bool &isNegValue) {

	for(int i = 0; i < nbChanTot; i++) {
		if( abs( cur_buf[channelList[i]] ) >= thresholds[i] ) {
			if(cur_buf[channelList[i]] < 0)
				// if the over threshold value is a negative one
				isNegValue = true;
			else
				isNegValue = false;
			
			// Id of the channel with a supra-threshold value
			return channelList[i];
		} // if
	} // for

	return -1; // No supra-threshold value was found
} // searchSpikeInChannels


/**
 * Give the threshold value corresponding to a given channel.
 * @param chanId Channel id
 * @param nbChanTot Total number of channel
 * @param channelList List containing the channels Id
 * @param thresholds Thresholds list (for each channel) to detect a spike
 * @return The threshold corresponding to the given channel id.
 */
double getThresholdFromChan(const int chanId, const int nbChanTot, 
							const int *channelList, const double *thresholds) {

	for(int i = 0; i < nbChanTot; i++) {
		if( channelList[i] == chanId )
			return thresholds[i];
	} // for

	cerr << "Error : Channel Id : " << chanId 
	<< " was not found in channel list : ";
	
	for(int i = 0; i < nbChanTot; i++) {
		if(i != 0)
			cerr << ", ";
		cerr << channelList[i];
	}
	cerr << " !" << endl;
	return -1; // Channel id was not found in channel list !
} // getThresholdFromChan


/**
 * Fill arguments values from user's argv
 * @param argc number of arguments
 * @param argv list of arguments
 * @param arguments user arguments to fill
 */
void parseArgs(const int argc, char **argv, arguments &arguments) {
	// Parse command-line
	int nOptions = argc;
	int i;

	if(nOptions < 2)
		error(argv[0]);
	
	for ( i = 1 ; i < nOptions ; ++i ) {
		if ( argv[i][0] != '-' ) break;
		if ( strlen(argv[i]) < 2 ) error(argv[0]);
		switch ( argv[i][1] ) {

			case 'h': // help
				help(argv[0]);
				break;

			case 't': // thresholds
				if ( i+1 > nOptions ) error(argv[0]);
 				arguments.thresList = argv[++i];
				arguments.isThresListProvided = true;
				break;

			case 'l': // spike length (search window length)
				if ( i+1 > nOptions ) error(argv[0]);
				arguments.peakLength = atoi(argv[++i]);
				arguments.isPeakLengthProvided = true;
				break;

			case 'r': // refractory period
				if ( i+1 > nOptions ) error(argv[0]);
				arguments.refractoryPeriod = atoi(argv[++i]);
				arguments.isRefractoryPeriodProvided = true;
				break;

			case 'w': // Extraction window length
				if ( i+1 > nOptions ) error(argv[0]);
				arguments.spikeLength = atoi(argv[++i]);
				arguments.isSpikeLengthProvided = true;
				break;

			case 'p': // peak position into the extracted spike
				if ( i+1 > nOptions ) error(argv[0]);
				// shift to extract before the spike peak
				arguments.timeBeforeSpike = atoi(argv[++i])-1;
				arguments.isTimeBeforeSpikeProvided = true;
				break;

			case 'n': // Total number of channels
				if ( i+1 > nOptions ) error(argv[0]);
				arguments.totalChannelNumber = atoi(argv[++i]);
				arguments.isTotalChannelNumberProvided = true;
				break;

			case 'c': // List of channels for each group
				if ( i+1 > nOptions ) error(argv[0]);
				arguments.channelList = argv[++i];
				arguments.isChannelListProvided = true;
				break;

#ifdef NBITS
				case 'o': // OffsetpeakVal
				if ( i+1 > nOptions ) error(argv[0]);
				arguments.offset = atoi(argv[++i]);
				arguments.isOffsetProvided = true;
				break;

			case 'b': // Resolution of the acquisition system
				if ( i+1 > nOptions ) error(argv[0]);
				arguments.nBits = atoi(argv[++i]);
				arguments.isNBitsProvided = true;
				break;
#endif
			
			case 'a': // use Absolute value for threshold
				arguments.isDisableAbs = false;
				break;
			
			case 'v': // verbose mode
				verbose = true;
				break;

			default:
				cerr << "error: unknown option '" << argv[i] << "'." << endl;
				exit(1);
				break;
		}
	} // for
	
	// input file
	if ( i >= argc ) {
		cerr << "error: missing session file basename." << endl;
		exit(1);
	}
	arguments.outputBaseFileName = argv[i];
	arguments.isOutputBaseFileProvided = true;
	if (i == argc && strcmp(argv[i],"-")) arguments.isInputFileProvided = false;
	else
	{
		arguments.isInputFileProvided = true;
		arguments.inputFileName = new char[256];
		strcpy(arguments.inputFileName,arguments.outputBaseFileName);
		strcat(arguments.inputFileName,".fil");
	}
	
	// Make sure we get the correct arguments.
	if(!arguments.isThresListProvided)  {
		cerr << "error: missing list of thresholds." << endl;
		exit(1);
	}
	if(!arguments.isPeakLengthProvided)  {
		cerr << "warning : missing peak search length." 
		<< " Using default value " << arguments.spikeLength << endl;
		arguments.peakLength = arguments.spikeLength; // default
	}
	if(!arguments.isRefractoryPeriodProvided)  {
		cerr << "error: missing refractory period." << endl;
		exit(1);
	}
	if(!arguments.isTotalChannelNumberProvided) {
		cerr << "error: missing total number of channels." << endl;
		exit(1);
	}
	if(!arguments.isSpikeLengthProvided)  {
		cerr << "error: missing waveform length." << endl;
		exit(1);
	}
	if(!arguments.isTimeBeforeSpikeProvided)  {
		cerr << "error: missing position of the peak within the waveform."
		<< endl;
		exit(1);
	}
	if(!arguments.isChannelListProvided)  {
		cerr << "error: missing list of channels." << endl;
		exit(1);
	}
#ifdef NBITS
	if(!arguments.isNBitsProvided) {
		cerr << "warning : missing resolution (number of bits). "
		<< "Using default value " << int(RECORD_BYTE_SIZE*8) << endl;
	}
#endif
} // parseArgs


/**
 * Check argument values
 * @param arguments Values of user's arguments
 * @param buffer_size Length of the buffer
 * @param inputFile Input file desciptor
 * @return TRUE if everything is Ok, FALSE else.
*/
bool checkInputs(const arguments arguments, const int buffer_size,
				 const FILE *inputFile) {
	// Check Inputs
	if(inputFile == NULL) {
		cerr<<"error: cannot open input file ('" << arguments.inputFileName
		<< "')." << endl;
		return false;
	} else if(arguments.peakLength < 1) {
		cerr << "error: negative peak search length (" << arguments.peakLength
		<< ")" << endl;
		return false;
	} else if(arguments.peakLength*arguments.totalChannelNumber > buffer_size) {
			cerr << "error: peak search length (" << arguments.peakLength
			<< ") greater than buffer size (" << buffer_size << ")." << endl;
			return false;
	} else if(arguments.spikeLength > int(buffer_size/2)) {
			cerr << "error: waveform length (" << arguments.spikeLength
			<< ") greater than buffer size/2 (" << (buffer_size/2) << ")."
			<< endl;
			return false;
	} else if(arguments.timeBeforeSpike > arguments.spikeLength) {
			cerr << "error: peak position (" << (arguments.timeBeforeSpike+1)
			<< ") greater than waveform length ("
					<< arguments.spikeLength << ")." << endl;
			return false;
	} else if(arguments.timeBeforeSpike < 0) {
		cerr << "error: peak position (" << (arguments.timeBeforeSpike+1)
		<< ") equal 0 !" << endl;
		return false;			
	} else if(arguments.timeBeforeSpike > arguments.peakLength) {
			cerr << "error: peak position (" << (arguments.timeBeforeSpike+1)
			<< ") greater than peak search length (" << arguments.peakLength 
			<< ")." << endl;
			return false;
	} else if(arguments.refractoryPeriod < 0) {
		cerr << "error: negative refractory period (" 
		<< arguments.refractoryPeriod << ")." << endl;
		return false;
	}

	return true;
} // checkInputs


/**
 * Check and compare channels and thresholds lists.
 * @param cList List of given channels
 * @param cNb_group Number of channels in each group
 * @param nbC Number of channels groups
 * @param tNb_group Number of thresholds in each group
 * @param nbT Number of thresholds groups
 * @return TRUE if channels and thresholds lists are correct, FALSE else.
 */
bool checkChanAndThres(int **cList, int *cNb_group, int nbC, int *tNb_group, 
					   int nbT) {
	if(nbC != nbT) {
		cerr << "error: incoherent number of electrode groups (" << nbC
				<<") and thresholds (" << nbT << ")." << endl;
		return false; // different number of channels and thresholds groups !
	} // if

	for(int g = 0; g < nbC; g++) {
		if(cNb_group[g] != tNb_group[g]) {
			cerr << "error: incoherent number of channels (" << cNb_group[g] 
			<< ") and thresholds (" << tNb_group[g] << ") for electrode group "
			<< g << "." << endl;
			// different number of item in current group between channels and
			// thresholds ones !
			return false;
		} // if

		for(int i = 0; i < cNb_group[g]; i++) {
			if(cList[g][i] < 0) {
				cerr << "error: negative channel ID (" << cList[g][i] 
				<< ") in electrode group " << g << "." << endl;
			} // if
		} // for i
	} // for g

	return true; // if no problem return true
} // checkChanAndThres


/**
 * Extract thresholds values from arguments
 * @param thresNb_group number of thresholds per group (to be completed)
 * @param thresList list of the groups thresholds value (to be completed)
 * @param arguments user arguments
 * @return The number of thresholds
 */
int getThresholdsFromArg(int *thresNb_group, double **thresList, 
						 const struct arguments arguments) {

	int nbGroups = 0; // Number of thresholds groups
	char *groups, *currentGroup, *thresholds; // Groups of electrodes and thr

	// extract thresholds from arguments
	groups = strdupa (arguments.thresList); // Split groups
	currentGroup = strsep (&groups, GROUP_SEPARATOR); // focus on the 1st group

	while(currentGroup != NULL) {
		// initialization
		thresNb_group[nbGroups] = 0; // init the group thresholds number

		thresholds = strsep (&currentGroup, CHANNEL_SEPARATOR); // split thr
		while(thresholds != NULL) {
			// current group threshold
			thresList[nbGroups][thresNb_group[nbGroups]] = atof(thresholds);

			thresholds = strsep (&currentGroup, CHANNEL_SEPARATOR);
			if(thresholds == NULL) {
				thresNb_group[nbGroups]++;
				break;
			} // if
			thresNb_group[nbGroups]++;
		} // while (thresholds)

		currentGroup = strsep (&groups, GROUP_SEPARATOR);
		if(currentGroup == NULL) {
			nbGroups++;
			break;
		} // if
		nbGroups++;
	} // while (currentGroup)

	delete[] currentGroup; delete[] groups; delete[] thresholds;
	return nbGroups;

} // getThresholdsFromArg


/**
 * Extract channels IDs from arguments
 * @param channelNb_group number of channels per group (to be completed)
 * @param channelList list of the groups channels id (to be completed)
 * @param arguments user arguments
 * @return The number of groups
 */
int getChannelsFromArg(int *channelNb_group, int **channelList,
							  const struct arguments arguments) {

	char zero = '0'; // Zero as a char (used to test arguments)
	int nbGroups = 0; // Number of electrodes groups
	char *groups, *currentGroup, *channels; // Groups of electrodes and channels

	// extract channels from arguments
	groups = strdupa (arguments.channelList); // Split groups
	currentGroup = strsep (&groups, GROUP_SEPARATOR); // focus on the 1st group

	while(currentGroup != NULL) {
		// initialization
		channelNb_group[nbGroups] = 0; // init the group channels number

		channels = strsep (&currentGroup, CHANNEL_SEPARATOR); // split channels
		while(channels != NULL) {
			if(atoi(channels)<1 && *channels != zero) {
				cerr << "error: negative channel IDs." << endl;
				exit(1);
			}
			if(atoi(channels)>= arguments.totalChannelNumber) {
				cerr << "error: channel ID greater to total number of channels"
				<< "(" << arguments.totalChannelNumber << ")." << endl;
				exit(1);
			}
			// current group channel's id
			channelList[nbGroups][channelNb_group[nbGroups]] = atoi(channels);

			channels = strsep (&currentGroup, CHANNEL_SEPARATOR);
			if(channels == NULL) {
				channelNb_group[nbGroups]++;
				break;
			}
			channelNb_group[nbGroups]++;
		} // while (channels)

		currentGroup = strsep (&groups, GROUP_SEPARATOR);
		if(currentGroup == NULL) {
			nbGroups++;
			break;
		}
		nbGroups++;
	} // while (currentGroup)

	delete[] currentGroup; delete[] groups; delete[] channels;
	return nbGroups;

} // getChannelsFromArg
