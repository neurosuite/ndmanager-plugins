/***************************************************************************
 *   process_pca.cpp							   							*
 *									   										*
 *   Copyright (C) 2010 by Nicolas Lebas   				   					*
 *   nicolas.lebas@college-de-france.fr   				   					*
 *                                                                         	*
 * Description : process_pca compute a Principal Component Analysis (PCA) 	*
 * with spikes data							   								*
 *									   										*
 * Usage : process_pca [options] [input]				   					*
 *									   										*
 *   This program is free software; you can redistribute it and/or modify  	*
 *   it under the terms of the GNU General Public License as published by  	*
 *   the Free Software Foundation; either version 2 of the License, or     	*
 *   (at your option) any later version.                                   	*
 *                                                                         	*
 *   This program is distributed in the hope that it will be useful,       	*
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        	*
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         	*
 *   GNU General Public License for more details.                          	*
 *                                                                         	*
 *   You should have received a copy of the GNU General Public License     	*
 *   along with this program; if not, write to the                         	*
 *   Free Software Foundation, Inc.,                                       	*
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             	*
 *									   										*
 ***************************************************************************/
#include "process_pca.h"
#include "progressbar.h"
#include <gsl/gsl_statistics_double.h> // covariance calculation
#include <gsl/gsl_eigen.h> // find eigen values and vectors
#include <gsl/gsl_blas.h> // matrix operations
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
using namespace std;


static bool verbose = false;
const char *programVersion = "process_pca 0.9.4 (28-11-2011)";

unsigned long int nRecords = 0; // number of records in all channels
unsigned long int nRecordsPerChannel = 0; // number of records for a channel


/**
 * Usage error
 * @param name Name of the program
 */
void error(const char* name)
{
	cerr << programVersion << endl;
	cerr << "usage: " << name << " [options] input (type '" << name << " -h' for details)" << endl;
	exit(1);
} // error


/**
 * Usage information (~help)
 * @param name Name of the program
 */
void help(const char* name)
{
	cout << name << ": perform principal component analysis (PCA)." << endl;
	cout << "usage: " << name << " [options] input" << endl;
	cout << "usage: " << name << " [options] -" << endl;
	cout << "(use the second form to read from stdin)" << endl << endl;
	cout << " input           input filename" << endl;
	cout << " -f output       output filename" << endl;
	cout << " -n channels     number of channels in the spike file" << endl;
	cout << " -p position     position of the peak within the waveform, in number of samples"<< endl;
	cout << " -b length       number of samples to consider for PCA before spike" << endl;
	cout << " -a length       number of samples to consider for PCA after spike" << endl;
	cout << " -w length       number of samples per waveform" << endl;
	cout << " -d components   number of principal components per channel" << endl;
#ifdef NBITS
	cout << " -o offset       offset value" << endl;
	cout << " -r bits         resolution of the acquisition system (in bits)" << endl;
#endif
	cout << " -s size         input data size in bytes (ex : 32000000) when reading from standard input" << endl;
	cout << " -c              use centered data for the projection" << endl;
	cout << " -x              include extra features in output file (spike peak values)" << endl;
	cout << " -v              verbose mode" << endl;
	cout << " -h              display help" << endl;
	cout << endl << "All arguments are mandatory except" << endl;
	cout << "  -x (-p is necessary if you use it)" << endl;
	cout << "  -s which is required only when reading from standard input" << endl;
	cout << "  -b, -a and -p could be not used (all spike length will be considered)" << endl;
#ifdef NBITS
	cout << "  -r if the resolution is 16 bits" << endl;
#endif
	cout << endl;
	exit(0);
} // help


// Start here
int main(int argc,char *argv[])
{
	struct arguments arguments;
	arguments.inputSize = 0;
	arguments.nChannels = 0;
	arguments.beforeSpike = -1;
	arguments.afterSpike = -1;
	arguments.peakPosition = -1;
	arguments.spikeLength = -1;
	arguments.nComponents = 0; // number of principal components
	arguments.isCenteredData = false;
	arguments.offset = 0;
	
	arguments.isInputFileProvided = false;
	arguments.isOutputFileProvided = false;
	arguments.isInputSizeProvided = false;
	arguments.isNChannelsProvided = false;
	arguments.isBeforeSpikeProvided = false;
	arguments.isAfterSpikeProvided = false;
	arguments.isPeakPositionProvided = false;
	arguments.isSpikeLengthProvided = false;
	arguments.isNComponentsProvided = false;
	arguments.isExtraFeaturesProvided = false;
	arguments.isOffsetProvided = false;
#ifdef NBITS
	arguments.nBits = int(RECORD_BYTE_SIZE*8);
	arguments.isNBitsProvided = false;
#endif
	
	parseArgs(argc,argv,arguments); // Parse command-line
	
	short *rawData; // data, means/channel
	short **peakVal; // peak values for extra features output
	// means, sum and variance-cov matrix for each dimension in each channel
	double **mean,**sum;
	// data for each channel/ spike dimension/ spike & reduced data (PCA)
	gsl_matrix **datSpkChanCenter, ** datSpkChan,**reducedData;
	gsl_matrix **varcov;// variance-cov matrix for each dimension
	FILE *inputFile = NULL,*outputFile = NULL;
	unsigned long int nSpikes = -1,nRecordsRead = 0;
	int data2use = arguments.spikeLength; // number of record to use in the waveform
	int recShift = 0; // shift for first record to consider in the waveform
	if(arguments.isBeforeSpikeProvided)
	{
		data2use = (arguments.beforeSpike+1+arguments.afterSpike);
		recShift = arguments.peakPosition-arguments.beforeSpike;
	}
	gsl_vector *eigenValues = gsl_vector_alloc(data2use);
	gsl_matrix *eigenVectors = gsl_matrix_alloc(data2use,data2use);
	gsl_eigen_symmv_workspace * w = gsl_eigen_symmv_alloc(data2use);
	gsl_matrix_view reducedEigenVectors; // for storing final data
	
	// open input
	if ( arguments.isInputFileProvided )
	{
		inputFile = fopen(arguments.inputFileName,"rb");
		if ( inputFile == NULL )
		{
			cerr << "error: cannot open '" << arguments.inputFileName << "'." << endl;
			exit(1);
		} // if
		fseeko(inputFile,0,SEEK_END); // put the position indicator at the end of the stream
		arguments.inputSize = ftello(inputFile); // value of the position indicator of the stream (here ~file size)
	} // if
	
	// Check Input Size
	if ( !checkInputs(arguments) ) // check arguments value
		exit(1);
	
	if ( verbose )
	{
		cout << endl;
		cout << "Input File            = ";
		if ( arguments.isInputFileProvided ) cout << arguments.inputFileName << endl;
		else cout << "-" << endl;
		cout << "Output File           = " << arguments.outputFileName << endl;
		cout << "Input Size            = ";
		if ( arguments.isInputSizeProvided ) cout << arguments.inputSize << endl;
		else cout << "N/A" << endl;
		
#ifdef NBITS
		cout << "Resolution            = ";
		if ( arguments.isNBitsProvided ) cout << arguments.nBits << " bits" << endl;
		else cout << "default" << endl;
#endif
		cout << endl;
	} // if verbose
	
	// number of records (for all and one channels)
	nRecords = arguments.inputSize/RECORD_BYTE_SIZE;
	nRecordsPerChannel = nRecords/arguments.nChannels;
	nSpikes = nRecordsPerChannel/arguments.spikeLength; // nb spikes
	
	if ( nRecords < 1 || nRecordsPerChannel < 1 )
	{
		cerr << "error: not enough records (size " << arguments.inputSize << ")" << endl;
		exit(1);
	}
	if ( nSpikes < 1 )
	{
		cerr << "error: incorrect spike number (" << nSpikes << "), check number of channels ("
				<< arguments.nChannels << ") and input size (" << arguments.inputSize << ")." << endl;
		exit(1);
	}
	
	if ( verbose )
	{
		cout << "Waveform length                 = " << arguments.spikeLength << endl;
		cout << "Number of principal components  = " << arguments.nComponents << endl;
		if(arguments.isBeforeSpikeProvided)
		{
			cout << "Number of samples before peak   = " << arguments.beforeSpike << endl;
			cout << "Number of samples after peak    = " << arguments.afterSpike << endl;
			cout << "Peak position in waveform       = " << arguments.peakPosition << endl;
		}
		cout << "Projection with centered data   = " << arguments.isCenteredData << endl;
		///cout << "Record size basis               = " << (RECORD_BYTE_SIZE*8) << " bits" << endl;
		cout << "Number of input records         = " << nRecords << endl;
		cout << "Number of records per channel   = " << nRecordsPerChannel << endl;
		cout << "Number of spikes                = " << nSpikes <<endl;
		cout << endl;
	}

	ProgressBar *progress = new ProgressBar("","PCA",(arguments.nChannels+4));
	
	// Init arrays
	rawData = new short[nRecords]; // Buffer for all data (all channels)
	mean = new double* [arguments.nChannels]; // means init
	sum = new double* [arguments.nChannels]; // sums init
	if(arguments.isExtraFeaturesProvided)
		peakVal = new short* [arguments.nChannels]; // peak values init&
	if(!arguments.isCenteredData)
		datSpkChan = new gsl_matrix* [arguments.nChannels];
	datSpkChanCenter = new gsl_matrix* [arguments.nChannels];
	reducedData = new gsl_matrix* [arguments.nChannels];
	varcov = new gsl_matrix* [arguments.nChannels];
	
	progress->start(); // Start progress bar
	///progress->message("Extracting data");
	// Get data
	if ( arguments.isInputFileProvided )
	{
		rewind(inputFile); // put the position indicator at the beginning of the stream
		nRecordsRead = fread(rawData,sizeof(short),nRecords,inputFile);
		fclose(inputFile);
	}
	else
	{
		nRecordsRead = fread(rawData,sizeof(short),nRecords,stdin);
	} // else
	
	if ( nRecordsRead != nRecords )
	{
		cerr << "error: insufficient number of records in the file (" << nRecordsRead 
		<< ", expecting " << nRecords << ")" << endl;
		exit(1);
	}
	progress->advance(); // Complete data importation
	
	// Fill arrays with rec values & compute means
	///progress->message("Preparing data for PCA");
	for ( int i = 0 ; i < arguments.nChannels ; ++i )
	{
		mean[i] = new double[data2use];
		sum[i] = new double[data2use];
		datSpkChanCenter[i] = gsl_matrix_alloc(data2use,nSpikes);
		if(!arguments.isCenteredData)
			datSpkChan[i] = gsl_matrix_alloc(data2use,nSpikes);
		if(arguments.isExtraFeaturesProvided)
			peakVal[i] = new short[nSpikes];
		
		varcov[i] = gsl_matrix_alloc(data2use,data2use);
		reducedData[i] = gsl_matrix_alloc(arguments.nComponents,nSpikes);
		for ( int j = 0 ; j < data2use ; ++j )
		{
			mean[i][j] = -1;
			sum[i][j] = 0;
			for ( unsigned int k = 0 ; k < nSpikes ; ++k )
			{
				double v = rawData[(arguments.nChannels*arguments.spikeLength)*k+((j+recShift)*arguments.nChannels)+i];
				gsl_matrix_set(datSpkChanCenter[i],j,k,v);
				if(!arguments.isCenteredData)
					gsl_matrix_set(datSpkChan[i],j,k,v);
				sum[i][j] += v;
				if(arguments.isExtraFeaturesProvided && (j+recShift)==arguments.peakPosition)
					peakVal[i][k] = v; // if this is the peak, store it !
			} // for k
			mean[i][j] = sum[i][j]/nSpikes; // mean computation
			for ( unsigned int k = 0 ; k < nSpikes ; ++k )
			{
				gsl_matrix_set (datSpkChanCenter[i],j,k,(gsl_matrix_get(datSpkChanCenter[i],j,k)-mean[i][j]));
			} // for k
		} // for j
	} // for i
	delete[] rawData; // free memory
	progress->advance(); // Complete data initialization
	
	if ( verbose )
	{
		cout << endl << endl;
		cout << "Total number of channels = " << arguments.nChannels << endl;
		for ( int c = 0 ; c < arguments.nChannels ; ++c )
		{
			cout << "Means for channel #" << c << " = ";
			for ( int d = 0 ; d < data2use ; ++d )
			{
				cout << mean[c][d] << ", ";
			} // for d
			cout << endl;
			cout << endl;
		} // for c
		cout << endl;
	} // verbose
	
	// PCA statement
	///progress->message("Computing PCA");
	for ( int i = 0 ; i < arguments.nChannels ; ++i )
	{
		// compute variance-covariance matrix (Data * DataTrans)
		gsl_blas_dgemm(CblasNoTrans,CblasTrans,(1.0/(nSpikes-1)),datSpkChanCenter[i],datSpkChanCenter[i],0.0,varcov[i]);
		
		// solve eigen system to get eigen values and vectors
		gsl_eigen_symmv(varcov[i],eigenValues,eigenVectors,w);
		gsl_eigen_symmv_sort(eigenValues,eigenVectors,GSL_EIGEN_SORT_VAL_DESC); // descending sort for eigen vectors and values
		
		// keep only eigen vectors for the given firt principal components
		reducedEigenVectors = gsl_matrix_submatrix(eigenVectors,0,0,data2use,arguments.nComponents);
		
		// compute new coordinates for data : Trans(evec_reduce) x Trans(data)
		if(!arguments.isCenteredData)
			gsl_blas_dgemm(CblasTrans,CblasNoTrans,1.0,&reducedEigenVectors.matrix,datSpkChan[i],0.0,reducedData[i]);
		else
			gsl_blas_dgemm(CblasTrans,CblasNoTrans,1.0,&reducedEigenVectors.matrix,datSpkChanCenter[i],0.0,reducedData[i]);
		
		progress->advance(); // Complete PCA for channel i
		
	} // for i
	
	///progress->message("Saving");
	// Write ouput file (FET format)
	outputFile = fopen(arguments.outputFileName,"w");
	if( !arguments.isExtraFeaturesProvided )
		fprintf(outputFile,"%i\n",(arguments.nChannels*arguments.nComponents));
	else
		fprintf(outputFile,"%i\n",(arguments.nChannels*arguments.nComponents + arguments.nChannels));
	for ( unsigned int k = 0 ; k < nSpikes ; ++k )
	{
		for ( int i = 0 ; i < arguments.nChannels ; ++i )
		{
			for ( int j = 0 ; j < arguments.nComponents ; ++j )
			{
				double *d = gsl_matrix_ptr(reducedData[i],j,k);
				if ( !fprintf(outputFile,"%i ",int(*d)) )
					cerr << "warning: missing data (channel " << i << ", dimension " << j << ", spike " 
					<< k << ")." << endl;
			} // for j
		} // for i
		if( arguments.isExtraFeaturesProvided ) // write peak value after all data
			for ( int i = 0; i < arguments.nChannels ; ++i )
				if ( !fprintf(outputFile,"%i ",int(peakVal[i][k])) )
						cerr << "warning: missing peak (channel " << i << ", spike " << k << ")." << endl;
			
		fprintf(outputFile,"\n");
	} // for k
	fclose(outputFile);
	progress->advance(); // Complete saving results
	
	// Free Memory
	///progress->message("Free Memory");
	for ( int i = 0 ; i < arguments.nChannels ; ++i )
	{
		delete[] mean[i];
		delete[] sum[i];
		if(arguments.isExtraFeaturesProvided)
			delete[] peakVal[i];
		if(!arguments.isCenteredData)
			gsl_matrix_free(datSpkChan[i]);
		gsl_matrix_free(datSpkChanCenter[i]);
		gsl_matrix_free(varcov[i]);
		gsl_matrix_free(reducedData[i]);
	}
	
	if(arguments.isExtraFeaturesProvided)
		delete[] peakVal;
	if(!arguments.isCenteredData)
		delete[] datSpkChan;
	delete[] datSpkChanCenter;
	delete[] mean;
	delete[] sum;
	delete[] varcov;
	delete[] reducedData;
	
	gsl_eigen_symmv_free(w);
	gsl_vector_free(eigenValues);
	gsl_matrix_free(eigenVectors);
	
	progress->advance(); // Complete free memory
	delete progress;
	
	if ( verbose ) cout << endl;
		
	return 0;
} // main


/**
 * Fill arguments values from user's argv
 * @param argc number of arguments
 * @param argv list of arguments
 * @param arguments user arguments to fill
 */
void parseArgs(const int argc,char **argv,arguments &arguments)
{
	// Parse command-line
	int nOptions = argc;
	int i;
	
	if ( argc == 2 && (!strcmp(argv[1],"-h") || !strcmp(argv[1],"--help")) ) help(argv[0]);
	if ( nOptions < 2 ) error(argv[0]);
	
	for ( i = 1 ; i < nOptions ; ++i )
	{
		if ( argv[i][0] != '-' ) break;
		if ( strlen(argv[i]) < 2 || argv[i][0] != '-' ) error(argv[0]);
		switch ( argv[i][1] )
		{
			case 's': // input size
				if ( i+1 > nOptions ) error(argv[0]);
				arguments.inputSize = atoi(argv[++i]);
				arguments.isInputSizeProvided = true;
				break;
				
			case 'n': // Total number of channels
				if ( i+1 > nOptions ) error(argv[0]);
				arguments.nChannels = atoi(argv[++i]);
				arguments.isNChannelsProvided = true;
				break;
				
			case 'b': // Number of record before spike to consider
				if ( i+1 > nOptions ) error(argv[0]);
				arguments.beforeSpike = atoi(argv[++i]);
				arguments.isBeforeSpikeProvided = true;
				break;
				
			case 'p': // peak position into the extracted spike
				if ( i+1 > nOptions ) error(argv[0]);
				arguments.peakPosition = atoi(argv[++i]);
				arguments.isPeakPositionProvided = true;
				break;
				
			case 'a': // Number of record after spike to consider
				if ( i+1 > nOptions ) error(argv[0]);
				arguments.afterSpike = atoi(argv[++i]);
				arguments.isAfterSpikeProvided = true;
				break;
				
			case 'w': // Extraction waveform length
				if ( i+1 > nOptions ) error(argv[0]);
				arguments.spikeLength = atoi(argv[++i]);
				arguments.isSpikeLengthProvided = true;
				break;
				
			case 'd': // number of principal components (dimensions)
				if ( i+1 > nOptions ) error(argv[0]);
				arguments.nComponents = atoi(argv[++i]);
				arguments.isNComponentsProvided = true;
				break;
				
			case 'f': // output file (for writing transformed data)
				if ( i+1 > nOptions ) error(argv[0]);
				arguments.outputFileName = argv[++i];
				arguments.isOutputFileProvided = true;
				break;
				
#ifdef NBITS
			case 'r': // Resolution of the acquisition system
				if ( i+1 > nOptions ) error(argv[0]);
				arguments.nBits = atoi(argv[++i]);
				arguments.isNBitsProvided = true;
				break;
#endif
			
			case 'c': // use centered data for projection
				arguments.isCenteredData = true;
				break;
				
			case 'x': // add extra features in output file
				arguments.isExtraFeaturesProvided = true;
				break;
				
			case 'v': // verbose mode
				verbose = true;
				break;
				
			case 'h': // show help
				help(argv[0]);
				break;
			
			default:
				cerr << "error: unknown option '" << argv[i] << "'." << endl;
				exit(1);
				break;
		}
	} // for
	
	// input file
	if ( i >= argc )
	{
		cerr << "error: missing input file." << endl;
		exit(1);
	}
	
	if ( i == argc && strcmp(argv[i],"-") ) arguments.isInputFileProvided = false;
	else
	{
		arguments.inputFileName = argv[i];
		arguments.isInputFileProvided = true;
	}
	
	// Make sure we get the correct arguments.
	if ( !arguments.isInputSizeProvided )
	{
		if ( !arguments.isInputFileProvided )
		{
			cerr << "error: missing size for standard input." << endl;
			exit(1);
		}
	} // if
	if ( !arguments.isNChannelsProvided )
	{
		cerr << "error: missing number of channels." << endl;
		exit(1);
	}
	if ( !arguments.isSpikeLengthProvided )
	{
		cerr << "error: missing waveform length." << endl;
		exit(1);
	}
	if ( !arguments.isNComponentsProvided)
	{
		cerr << "error: missing number of principal components to keep after PCA." << endl;
		exit(1);
	}
	if ( !arguments.isOutputFileProvided)
	{
		cerr << "error: missing output file." << endl;
		exit(1);
	}
	if( arguments.isBeforeSpikeProvided)
		if ( !arguments.isPeakPositionProvided || !arguments.isAfterSpikeProvided)
		{
			cerr << "error : missing number of samples after peak or peak position." << endl;
			exit(1);
		}
	if( arguments.isAfterSpikeProvided )
		if( !arguments.isBeforeSpikeProvided  || !arguments.isBeforeSpikeProvided)
		{
			cerr << "error : missing number of samples before peak or peak position." << endl;
			exit(1);
		}
#ifdef NBITS
	if ( !arguments.isNBitsProvided)
	{
		cerr << "warning : missing resolution,"
				<< " using default value " << int(RECORD_BYTE_SIZE*8) << endl;
	}
#endif
} // parseArgs


/**
 * Check argument values
 * @param arguments Values of user's arguments
 * @return TRUE if everything is Ok, FALSE else.
 */
bool checkInputs(const arguments arguments)
{
	// Make sure we get the correct arguments.
	if ( arguments.inputSize < 1 )
	{
		cerr << "error: incorrect input size " << arguments.inputSize << "." << endl<< endl;
		return false;
	}
// 	else if ( arguments.inputSize > MAX_INPUT_SIZE )
// 	{
// 		cerr << "error: input size " << arguments.inputSize << " is too large (>" << MAX_INPUT_SIZE << ")." << endl<< endl;
// 		return false;
// 	}
	else if ( arguments.inputSize%RECORD_BYTE_SIZE != 0 )
	{
		cerr << "error: input size " << arguments.inputSize << " is not a multiple of resolution " << RECORD_BYTE_SIZE << "." << endl<< endl;
		return false;
	}
	else if ( arguments.inputSize%arguments.spikeLength != 0 )
	{
		cerr << "error: input size " << arguments.inputSize << " is not a multiple of spike size " << arguments.spikeLength << "." << endl<< endl;
		return false;
	}

	if ( arguments.spikeLength > arguments.inputSize )
	{
		cerr << "error: waveform length (" << arguments.spikeLength << ") is greater than input size (" << arguments.inputSize << ")." << endl<< endl;
		return false;
	}
	
	if( arguments.isExtraFeaturesProvided && !arguments.isPeakPositionProvided)
	{
		cerr << "error: missing peak position for extra features." << endl<< endl;
		return false;
	}
	
	if ( arguments.isBeforeSpikeProvided )//|| arguments.isAfterSpikeProvided )
	{
		if( (arguments.beforeSpike+arguments.afterSpike) >= arguments.inputSize )
		{
			cerr << "error: interval between before ("<<arguments.beforeSpike<<") and after ("<<arguments.afterSpike
			<<") > waveform length ("<< arguments.spikeLength << ")." << endl<< endl;
			return false;
		}
		else if( arguments.peakPosition >= arguments.spikeLength )
		{
			cerr << "error: peak position ("<<arguments.peakPosition<<") must be in [0 - "<< arguments.spikeLength-1 << "]." << endl<< endl;
			return false;
		}
		else if( arguments.peakPosition < arguments.beforeSpike )
		{
			cerr << "error: peak position ("<<arguments.peakPosition<<") < number of samples before peak ("<< arguments.beforeSpike << ")." << endl<< endl;
			return false;
		}
		else if( (arguments.peakPosition+arguments.afterSpike) >= arguments.spikeLength )
		{
			cerr << "error: number of samples after spike ("<<arguments.afterSpike<<") is too large (max "
			<<(arguments.spikeLength-arguments.peakPosition-1)<< ")." << endl<< endl;
			return false;
		}
	}
	else if(arguments.isPeakPositionProvided)
	{
		cerr << "warning: incomplete information for PCA on partial waveform, "
		<< "using entire waveform length." << endl<< endl;
	}
	if ( arguments.nComponents < 1 )
	{
		cerr << "error: incorrect number of principal components (" << arguments.nComponents << ")." << endl<< endl;
		return false;
	}
	else if ( arguments.nComponents > arguments.spikeLength )
	{
		cerr << "error: number of principal components (" << arguments.nComponents << ") exceeds spike length (" << arguments.spikeLength << ")." << endl<< endl;
		return false;
	}
	else if( arguments.isBeforeSpikeProvided && arguments.isAfterSpikeProvided && 
		(arguments.beforeSpike+arguments.afterSpike+1) < arguments.nComponents )
	{
		cerr << "error: number of principal components (" << arguments.nComponents 
		<< ") exceeds interval used for PCA ("<<(arguments.beforeSpike+arguments.afterSpike+1)<<")." << endl<< endl;
		return false;
	}

	return true;
} // checkInputs
