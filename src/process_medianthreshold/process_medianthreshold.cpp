/***************************************************************************
                          process_medianthreshold.cpp  -  description
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
#include "process_medianthreshold.h"
#include <fstream>
#include <cstring>
#include <cstdlib>

static bool verbose = false;
const char *program_version = "process_medianthreshold 0.3b (21-12-2007)";
int allChannels_nbRecords = 0; // number of records in all channels
int oneChannel_nbRecords = 0; // number of records for a channel


/**
 * Usage error
 * @param name Name of the program
 */
void error(const char* name) {
	cerr << program_version << endl;
	cerr << "usage: " << name << " [options] [input] (type '" << name << " -h' for details)" << endl;
	exit(1);
} // error


/**
 * Usage information (~help)
 * @param name Name of the program
 */
void help(const char* name) {
	cout << "process_medianthreshold -- Computes a threshold using the median of the input." << endl;
	cout << "usage: " << name << " [options] [input]" << endl;
	cout << " -f input_file \t input file" << endl;
	cout << " -s size \t input size in bytes (ex : 32000000)" << endl;
	cout << " -n nb \t\t Total number of channels (ex : 10)" << endl;
	cout << " -c list \t Grouped channel list (e.g. 0,1,2:4,5,7 for 2 groups of 3 channels each)" << endl;
#ifdef NBITS
	cout << " -r nb \t\t Resolution of the acquisition system (bits)" << endl;
#endif
	cout << " -v \t\t verbose mode" << endl;
	cout << " -d data_file \t data file in text format (output)" << endl;
	cout << " -h \t\t show this message" << endl;
	cout << endl << "All the options are not mandatory except the total channel number (-n) and "
	<< "the list of channels (-c). The size (-s) is mandatory only if no input file (-f) is given."
	<< endl << "If no I/O file are given, default input and output are respectively stdin and stdout." << endl;
	exit(0);
} // help


// Start here
int main(int argc,char *argv[]) {
	struct arguments arguments;
	arguments.inputSize = 0;
	arguments.totalChannelNumber = 0;
	arguments.offset = 0;

	arguments.isInputFileProvided = false;
	arguments.isOutputFileProvided = false;
	arguments.isInputSizeprovided = false;
	arguments.isTotalChannelNumberProvided = false;
	arguments.isChannelListProvided = false;
	arguments.isOffsetProvided = false;
#ifdef NBITS
	arguments.nBits = int(RECORD_BYTE_SIZE*8);
	arguments.isNBitsProvided = false;
#endif

	short *input; // datas
	FILE *inputFile, *outputFile;
	char zero = '0'; // Zero as a char (used to test arguments)

	char *groups, *currentGroup, *channels; // Groups of electrodes and channels
	int nbChannel_currentGroup = 0, channelListSize = 0; // Channels numbers
	int nbGroups = 0; // Number of electrodes groups
	int *groupChannelId = new int[MAX_CHANNO]; // Store all channels number of a group
	long double *thresholds = new long double[MAX_CHANNO]; // Threshold = 4*sigma_n (one per channel)

	parseArgs(argc, argv, arguments); // Parse command-line
	// open input & output files
	if(arguments.isInputFileProvided) {
		inputFile = fopen(arguments.inputFileName,"rb");
		if(inputFile == NULL) {
			cerr<<"Error when opening Input File "<< arguments.inputFileName <<endl;
			exit(1);
		} // if
		fseek ( inputFile , 0 , SEEK_END ); // put the position indicator at the end of the stream
		arguments.inputSize = ftell (inputFile); // value of the position indicator of the stream (here ~file size)
	} // if
	if(arguments.isOutputFileProvided) {
		outputFile = fopen(arguments.outputFileName,"wb");
		if(outputFile == NULL) {
			cerr<<"Error when opening Output File "<< arguments.outputFileName <<endl;
			exit(1);
		} // if
	} // if

	if ( verbose ) {
		cout << endl;
		cout << "Input File = ";
		if (arguments.isInputFileProvided) cout << arguments.inputFileName << endl;
		else cout << "Not given" << endl;

		cout << "Output File = ";
		if (arguments.isOutputFileProvided) cout << arguments.outputFileName << endl;
		else cout << "Not given" << endl;

		cout << "Input Size = ";
		if (arguments.isInputSizeprovided) cout << arguments.inputSize << endl;
		else cout << "Not given" << endl;

#ifdef NBITS
		cout << "The number of bits (resolution) = ";
		if (arguments.isNBitsProvided) cout << arguments.nBits << " bits" << endl;
		else cout << "Not given" << endl;
#endif
		cout << endl;
	} // if verbose

	// Check Input Size
	if(!checkInputs(arguments)) // check arguments value
		exit(1);

	// number of records (for all and one channels)
	allChannels_nbRecords = arguments.inputSize/RECORD_BYTE_SIZE;
	oneChannel_nbRecords = int(allChannels_nbRecords/arguments.totalChannelNumber);
	if(allChannels_nbRecords < 1 || oneChannel_nbRecords < 1) {
		cerr << "Error : The record number " << arguments.inputSize
		<< " is < 1 per channel !" << endl;
		exit(1);
	}

	if ( verbose ) {
		cout << "Record Size Basis : " << (RECORD_BYTE_SIZE*8) << " bits" << endl;
		cout << "Number of Records in the input : " << allChannels_nbRecords << endl;
		cout << "Number of Records/Channel : " << oneChannel_nbRecords << endl;
		cout << endl;
	}

	input = new short[allChannels_nbRecords]; // Buffer for all datas (all channels)

	if ( verbose ) cout << "Acquiring all datas..." << endl;
	// Get datas
	if(arguments.isInputFileProvided) {
		rewind (inputFile); // put the position indicator at the beginning of the stream
		fread(input, sizeof(char), arguments.inputSize,inputFile);
		fclose ( inputFile );
	} else {
		fread(input, sizeof(char), arguments.inputSize,stdin);
	} // else

	// Output (output file)
	if(arguments.isOutputFileProvided && outputFile != NULL) {
		if ( verbose ) cout << "Generating output file..." << endl;
		for(int i = 0; i < allChannels_nbRecords; i++)
			fprintf(outputFile,"%d\n",input[i]);
		fclose( outputFile );
	}

	if ( verbose ) cout << "Converting datas into absolute values ..." << endl;
	for (int i = 0; i < allChannels_nbRecords; i++) {
		input[i] = abs(input[i]);
	} // for i

	/** Computes threshold for each channel groups */
	groups = strdupa (arguments.channelList); // Split groups
	currentGroup = strsep (&groups, GROUP_SEPARATOR); // focus on the first group

	while(currentGroup != NULL) {
		// initialization
		delete[] groupChannelId; // free memory
		nbChannel_currentGroup = 0; // init the group channels number
		groupChannelId = new int[MAX_CHANNO]; // init the group channels list

		channels = strsep (&currentGroup, CHANNEL_SEPARATOR); // split channels
		while(channels != NULL) {
			if(atoi(channels)<1 && *channels != zero) {
				cerr << "Error : The channels have to be positive integers." << endl;
				exit(1);
			}
			if(atoi(channels)>= arguments.totalChannelNumber) {
				cerr << "Error : The channel ids have to be inferior to the total number "
				<< " of channels (" << arguments.totalChannelNumber << ")." << endl;
				exit(1);
			}
			groupChannelId[nbChannel_currentGroup] = atoi(channels); // current group channel's id

			channels = strsep (&currentGroup, CHANNEL_SEPARATOR);
			nbChannel_currentGroup++;
			if(channels == NULL) {
				break;
			}
		} // while (channels)

		channelListSize += nbChannel_currentGroup; // Total number of given channels
		if ( verbose ) {
			cout << endl;
			cout << "Group #"<<(nbGroups+1)<<" : Compute threshold..." << endl;
		}
		thresholds=groupThresholds(input, arguments.totalChannelNumber,
											 groupChannelId, nbChannel_currentGroup);
		if ( verbose ) {
			cout << "**********************" << endl;
			cout << "Threshold(s) : ";
		} // if verbose
		for(int c = 0; c < nbChannel_currentGroup; c++) {
			if(nbGroups > 0 && c == 0 && !verbose) cout << ":"; // separate groups
			cout << thresholds[c];
			if(c < (nbChannel_currentGroup-1)) cout << ",";
/*			cout << "Threshold grp #" << (nbGroups+1) << ", channel #" << groupChannelId[c]
					 << " : " << thresholds[c] << endl;*/
		} // for c
		if ( verbose ) cout << endl << "**********************" << endl;

		currentGroup = strsep (&groups, GROUP_SEPARATOR);
		nbGroups++;
		if(currentGroup == NULL) {
			break;
		}
	} // while (currentGroup)
	cout << endl;

	if ( verbose ) {
		cout << endl;
		cout << "Total number of channels = " << arguments.totalChannelNumber << endl;
		cout << "List of channels = " << arguments.channelList << endl;
		cout << "Nb of Groups = " << nbGroups << endl;
		cout << "Nb of given channels = " << channelListSize << endl;
	}

	// Free Memory
	delete[] input; delete[] groupChannelId;
	delete[] currentGroup; delete[] groups; delete[] channels;
	delete[] thresholds;

	if ( verbose ) cout << endl;

	return 0;
} // main


/**
 * Quicksort algorithm (growing order) - Sort an array part
 * Tri rapide. (2007, mars 21). Wikipedia, The Free Encyclopedia. Retrieved 13:20, April 2, 2007,
 * from http://fr.wikipedia.org/w/index.php?title=Tri_rapide&oldid=15255912.
 * @param data Array to sort
 * @param p Pivot index to use for this sort
 * @param r End index of the range to sort
 * @return New pivot index
 */
int clusterize(short int data[], int p, int r) {
	int pivot = data[p], i = p-1, j = r+1;
	int temp;
	while(1) {
		do
			j--;
		while(data[j] > pivot);
		do
			i++;
		while(data[i] < pivot);
		if(i<j) {
			temp = data[i];
			data[i] = data[j];
			data[j] = temp;
		}
		else
			return j;
	}
	return j;
} // clusterize


/**
 * Quicksort algorithm (growing order) - Recursive function to sort completely a given array
 * Tri rapide. (2007, mars 21). Wikipedia, The Free Encyclopedia. Retrieved 13:20, April 2, 2007,
 * from http://fr.wikipedia.org/w/index.php?title=Tri_rapide&oldid=15255912.
 * @param data Array to sort
 * @param p Begin index of the range to sort
 * @param r End index of the range to sort
 */
void quickSort(short int data[], int p, int r) {
	int q;
	if(p<r) {
		q = clusterize(data, p, r);
		quickSort(data, p, q);
		quickSort(data, q+1, r);
	}
} // quickSort


/**
 * Sort the short int array in growing order (start Quicksort)
 */
void sort(short int data[], const int size) {
	quickSort(data, 0, size-1);
} // sort


/**
 * Compute the threshold of all channels in given group
 * @param allRecords the list of all records (all channels)
 * @param allChannelsNb total number of channels
 * @param groupChannelId list of the group channels id
 * @param groupChannelNb number of channel in the group
 * @return The threshold as a double precision variable
 */
long double* groupThresholds(const short* allRecords, int allChannelsNb,
							const int* groupChannelId, const int groupChannelNb) {

	// records for each channel in current group
	short **records = new short* [groupChannelNb];
	for(int c = 0; c < groupChannelNb; c++)
		records[c] = new short[oneChannel_nbRecords];
	// Thresholds for each channel
	long double *thres = new long double[groupChannelNb];
	// sigma_n = median{abs(x)/0.6745}
	long double sigma_n = 0;
	// Loop counter
	int index = 0;

	// store current group records
	for (int i = 0; i < allChannels_nbRecords; i += allChannelsNb) {
		for (int j = 0; j < groupChannelNb; j++) {
			records[j][index] = allRecords[i+groupChannelId[j]];
		} // for j
		index++;
	} // for i

	if ( verbose ) {
		cout << "Group Records Number : " << oneChannel_nbRecords*groupChannelNb << endl;
		cout << "Group Channels Number : " << groupChannelNb<< endl;
		cout << "Group Channels ID : ";
		for (int i = 0; i < groupChannelNb; i++)
			cout << groupChannelId[i] << " ";
		cout << endl << endl;
	}

	if ( verbose ) cout << "Starting sorting..." << endl;
	for(int i = 0; i < groupChannelNb; i++) {
		sort(records[i], oneChannel_nbRecords);

		// Median of records absolute values
		if(oneChannel_nbRecords > 1 && (oneChannel_nbRecords%2)==0)
			sigma_n = (records[i][int(oneChannel_nbRecords/2)-1]+records[i][int(oneChannel_nbRecords/2)])/(2.0*SIGMA_DIVISOR);
		else
			sigma_n = records[i][int(oneChannel_nbRecords/2)]/SIGMA_DIVISOR;

		thres[i] = (4*sigma_n); // Threshold = 4*sigma_n
		if ( verbose) {
			cout << "Index of the half : " << int(oneChannel_nbRecords/2) << endl;
			cout << "Sigma_n of the datas : " << sigma_n << endl;
			cout << "Threshold for channel #" << i << " : " << thres[i] << endl;
		} // verbose

	} // if i

	for(int c = 0; c < groupChannelNb; c++)
	 	delete[] records[c]; // free tmp records array
	delete[] records;

	return thres; // Threshold = 4*sigma_n
} // groupThresholds


/**
 * Check argument values
 * @param arguments Values of user's arguments
 * @return TRUE if everything is Ok, FALSE else.
 */
bool checkInputs(const arguments arguments) {
	// Make sure we get the correct arguments.
	if(!arguments.isTotalChannelNumberProvided) {
		cerr << "Error : The total number of channels has not been provided." << endl;
		return false;
	} // if
	if(!arguments.isChannelListProvided)  {
		cerr << "Error : The list of channels has not been provided." << endl;
		return false;
	} // if
#ifdef NBITS
	if(!arguments.isNBitsProvided) {
		cerr << "Warning : The number of bits (resolution) has not been provided."
				<< " Using default value " << int(RECORD_BYTE_SIZE*8) << endl;
	} // if
#endif
	if(!arguments.isInputSizeprovided) {
		if(!arguments.isInputFileProvided) {
			cerr << "Error : There is no Input Size for stdin !" << endl;
			return false;
		}
	} // if

	if(arguments.inputSize < 1) {
		cerr << "Error : Input Size " << arguments.inputSize << " is incorrect !" << endl;
		return false;
	} else if(arguments.inputSize > MAX_INPUT_SIZE) {
		cerr << "Error : Input Size " << arguments.inputSize << " is too long (>"
				<< MAX_INPUT_SIZE << ") !" << endl;
		return false;
	} else if(arguments.inputSize%RECORD_BYTE_SIZE != 0) {
		cerr << "Error : Input Size " << arguments.inputSize << " is not a multiple of record base size "
				<< RECORD_BYTE_SIZE << " !" << endl;
		return false;
	}

	return true;
} // checkInputs


/**
 * Fill arguments values from user's argv
 * @param argc number of arguments
 * @param argv list of arguments
 * @param arguments user arguments to fill
 */
void parseArgs(const int argc, char **argv, arguments &arguments) {
	if ( argc == 2 && strlen(argv[1]) == 2 && !strcmp(argv[1],"-h") ) help(argv[0]);
	if ( argc < 2) error(argv[0] );
	int nOptions = argc-1;
	for ( int i = 1 ; i <= nOptions ; ++i ) {
		if ( strlen(argv[i]) < 2 || argv[i][0] != '-' ) error(argv[0]);
		switch ( argv[i][1] ) {
			case 'f': // input file
				if ( i+1 > nOptions ) error(argv[0]);
				arguments.inputFileName = argv[++i];
				arguments.isInputFileProvided = true;
				break;

				case 's': // input size
					if ( i+1 > nOptions ) error(argv[0]);
					arguments.inputSize = atoi(argv[++i]);
					arguments.isInputSizeprovided = true;
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
							case 'r': // Resolution of the acquisition system
								if ( i+1 > nOptions ) error(argv[0]);
								arguments.nBits = atoi(argv[++i]);
								arguments.isNBitsProvided = true;
								break;
#endif

								case 'v': // verbose mode
									verbose = true;
									break;

									case 'd': // text data file ouput
										if ( i+1 > nOptions ) error(argv[0]);
										arguments.outputFileName = argv[++i];
										arguments.isOutputFileProvided = true;
										break;

			default:
				error(argv[0]);
				break;
		}
	} // for

} // parseArgs
