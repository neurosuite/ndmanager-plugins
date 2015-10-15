/***************************************************************************
 *   Copyright (C) 2004-2011 by Michael Zugaro                             *
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

#ifndef NLX_EXTRACTOR
#define NLX_EXTRACTOR

#include "customtypes.h"
#include "ncsfilecollection.h"
#include "nvtfile.h"
#include "nevfile.h"
#include "smifilecollection.h"
#include "outputfile.h"

#include <fstream>

using namespace std;

/**
@author Michael Zugaro
*/
class NLXExtractor : public ifstream
{
	public:
		/**
			Constructor
			@param	info	show file headers?
			@param	wideband	convert wideband signals?
			@param	spots	convert tracking data?
			@param	events	convert events?
			@param	timestamps	generate timestamp files?
			@param	sync	generate sync file?
			@param	restart generate restart file?
			@param	force	force overwrite?
			@param	reverse	should wideband signals be reversed?
			@param	threshold max time gap between successive records
			@param	frameDuration	approx. duration of a video frame
		*/
		NLXExtractor(bool info,bool wideband,bool spots,bool events,bool timestamps,bool sync,bool restart,bool force,bool reverse,Time threshold,Time frameDuration) : info(info),wideband(wideband),spots(spots),events(events),timestamps(timestamps),sync(sync),restart(restart),force(force),reverse(reverse),threshold(threshold),frameDuration(frameDuration),ncsProvided(false),nvtProvided(false),nevProvided(false),nRestarts(0),syncsProvided(false),restartsProvided(false),restartsUndefined(true) { smiFiles.set(frameDuration); };
		/**
			Destructor
		*/
		virtual ~NLXExtractor() { ncsFiles.close(); nvtFile.close(); nevFile.close(); };
		/**
			Initialization
			@param	inputs	input file names
			@param	nInputs	number of input files
			@param	output	output file base name
		*/
		virtual void init(char *inputs[],int nInputs,char *output) throw(NLXError);
		/**
			Extract to .dat
		*/
		virtual void extractWideband() throw(NLXError);
		/**
			Read sync info from file
		*/
		virtual void readSyncs(string filename) throw(NLXError);
		/**
			Browse .ncs files to find sync info
		*/
		virtual void findSyncs();
		/**
			Extract to .spots
		*/
		virtual void extractSpots() throw(NLXError);
		/**
			Resynchronize .spots file (using .smi file)
		*/
		virtual void syncSpots() throw(NLXError);
		/**
			Extract to .evt
		*/
		virtual void extractEvents() throw(NLXError);
		/**
			Read 'restart acquisition' events from file
		*/
		virtual void readRestarts(string filename) throw(NLXError);
		/**
			Find 'restart acquisition' events
		*/
		virtual void findRestarts();
		/**
			Does a given timestamp correspond to a restart?
		*/
		virtual bool isRestart(Time gap,Time timestamp);
		/**
			Extract
		*/
		virtual void extract() throw(NLXError);
		/**
			Show file headers
		*/
		virtual void showFileHeaders();

	private:

		// Which operations should be performed?
		bool						info;
		bool						wideband;
		bool						spots;
		bool						events;
		bool						timestamps;
		bool						sync;
		bool						restart;
		bool						force;
		bool						reverse;

		// Processing parameters
		Time						threshold;
		Time						frameDuration;

		// Input and output files
		string					basename;
		bool						ncsProvided;	// ncs file(s) provided in command-line
		bool						nvtProvided;	// nvt file provided in command-line
		bool						nevProvided;	// nev file provided in command-line
		bool						smiProvided;	// smi file(s) provided in command-line
		NCSFileCollection 	ncsFiles;
		NVTFile 					nvtFile;
		SMIFileCollection		smiFiles;
		NEVFile 					nevFile;
		OutputFile				widebandFile;
		OutputFile				spotsFile;
		OutputFile				eventFile;
		OutputFile				spotsTimestampFile;
		OutputFile				ncsTimestampFile;
		OutputFile				nvtTimestampFile;
		OutputFile				nevTimestampFile;
		OutputFile				syncFile;
		OutputFile				restartFile;

		// Sync info and restart events
		Time						syncs[MAX_N_RESTARTS];
		int						nSyncs;
		Time						restarts[MAX_N_RESTARTS];
		int						nRestarts;
		bool						syncsProvided;			// sync file provided in command-line
		bool						restartsProvided;		// restart file provided in command-line
		bool						restartsUndefined;	// restarts cannot be determined (no restart file, no nev file)
};

#endif
