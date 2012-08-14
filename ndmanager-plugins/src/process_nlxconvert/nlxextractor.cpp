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

#include "nlxextractor.h"
#include "progressbar.h"

#include <iostream>
#include <cmath>
#include <sstream>
#include <cstring>

using namespace std;
extern string program;

/**
	Initialization
*/
void NLXExtractor::init(char *inputs[],int nInputs,char *output) throw(NLXError)
{
	// Initialize input file objects
	for ( int i = 0 ; i < nInputs ; ++i )
	{
		string filename = inputs[i];
		int length = filename.length();
		if ( strcmp(&filename[length-4],".ncs") == 0 )
		{
			ncsFiles.add(filename);
			ncsProvided = true;
		}
		else if ( strcmp(&filename[length-4],".nvt") == 0 )
		{
			nvtFile.set(filename,frameDuration);
			nvtProvided = true;
		}
		else if ( strcmp(&filename[length-4],".smi") == 0 )
		{
			smiFiles.add(filename);
			smiProvided = true;
		}
		else if ( strcmp(&filename[length-4],".nev") == 0 )
		{
			nevFile.set(filename);
			nevProvided = true;
		}
		else if ( strcmp(&filename[length-8],".restart") == 0 )
		{
			readRestarts(filename);
			if ( restart )
			{
				warning("restart file provided as input, ignoring '-a' option.");
				restart = false;
			}
		}
		else if ( strcmp(&filename[length-5],".sync") == 0 )
		{
			readSyncs(filename);
			syncsProvided = true;
			if ( sync )
			{
				warning("sync file provided as input, ignoring '-s' option.");
				sync = false;
			}
		}
		else throw NLXError(NLXError::READ_ERROR,"unknown file type '" + filename + "'.");
	}
	// Open input files
	if ( ncsProvided )
	{
		ncsFiles.open();
		ncsFiles.init();
	}
	else if ( wideband || sync ) throw NLXError(NLXError::READ_ERROR,"missing .ncs files.");
	if ( nvtProvided )
	{
		if ( smiProvided )
		{
			warning("both .nvt and .smi files were provided, ignoring .smi file");
			smiProvided = false;
		}
		nvtFile.open();
		nvtFile.init();
	}
	if ( smiProvided )
	{
		smiFiles.open();
		smiFiles.init();
	}
	if ( spots && !nvtProvided && !smiProvided ) throw NLXError(NLXError::READ_ERROR,"missing .nvt or .smi file.");
	if ( nevProvided )
	{
		nevFile.open();
		nevFile.init();
	}
	else
	{
		if ( events || restart ) throw NLXError(NLXError::READ_ERROR,"missing .nev files.");
		else if ( (wideband || sync || spots) && !restartsProvided ) warning("missing .nev and .restart files, will try to guess 'restart acquisition' events.");
	}

	if ( wideband || spots || events || timestamps || sync || restart )
	{
		// Check output basename
		if ( output == 0 ) throw NLXError(NLXError::WRITE_ERROR,"missing output basename.");

		// Output file names
		string outputFilename(output);
		int dot = outputFilename.find_last_of('.');
		basename = outputFilename.substr(0,dot);

		// Initialize output file objects
		widebandFile.set(basename+".dat",force,ios_base::binary);
		spotsFile.set(basename+".spots",force,ios_base::trunc);
		eventFile.set(basename+".evt",force,ios_base::trunc);
		spotsTimestampFile.set(basename+".sts",force,ios_base::trunc);
		ncsTimestampFile.set(basename+".ncs-timestamps",force,ios_base::trunc);
		nvtTimestampFile.set(basename+".nvt-timestamps",force,ios_base::trunc);
		nevTimestampFile.set(basename+".nev-timestamps",force,ios_base::trunc);
		syncFile.set(basename+".sync",force,ios_base::trunc);
		restartFile.set(basename+".restart",force,ios_base::trunc);
	}
}

/**
	Extract to .dat
*/
void NLXExtractor::extractWideband() throw(NLXError)
{
	bool doWideband = wideband;
	bool doTimestamps = timestamps;
	bool doSync = sync;

	// Create output files
	// (most of the code here deals with making sure we do not override existing files, except if option '-f' is set)
	if ( wideband )
		try
		{
			widebandFile.create();
		}
		catch ( NLXError e )
		{
			error(e.report());
			doWideband = false;
		}
	if ( timestamps )
	{
		try
		{
			ncsTimestampFile.create();
			ncsTimestampFile.setf(ios::fixed,ios::floatfield);
			ncsTimestampFile.precision(3);
		}
		catch ( NLXError e )
		{
			error(e.report());
			doTimestamps = false;
		}
	}
	if ( sync )
	{
		try
		{
			syncFile.create();
			syncFile.setf(ios::fixed,ios::floatfield);
			syncFile.precision(3);
		}
		catch ( NLXError e )
		{
			error(e.report());
			doSync = false;
		}
	}
	if ( !doWideband && !doTimestamps && !doSync ) return;

	// Display progress bar
	string step;
	if ( doWideband ) step += "W";
	if ( doTimestamps ) { if ( step.length() != 0 ) step += ","; step += "Wt"; }
	if ( doSync ) { if ( step.length() != 0 ) step += ","; step += "S"; }
	else if ( !syncsProvided ) { if ( step.length() != 0 ) step += ","; step += "S*"; }
	ProgressBar progress(basename,step,ncsFiles.getNRecords());
	progress.start();

	Time gap,t;
	bool first = true;
	while ( true )
	{
		// Read next data record for each file
		if ( !ncsFiles.readRecord() ) break;
		gap = ncsFiles.getGap();
		t = ncsFiles.getTimestamp();

		// Remember timestamp of first record
		if ( first )
		{
			if ( doSync ) syncFile << t << endl;
			if ( !syncsProvided ) syncs[nSyncs++] = t;
			first = false;
		}

		// Update progress bar
		progress.advance();

		// If recording was restarted at this point, skip and display infomation next to progress bar
		if ( isRestart(gap,t) )
		{
			ostringstream message;
			message.setf(ios::fixed,ios::floatfield);
			message.precision(6);
			message << "(restart @ " << t/1e6 << ")";
			progress.message(message.str());
 			ncsFiles.skipMissingRecords();
			if ( !syncsProvided ) syncs[nSyncs] = gap;
			if ( doSync ) syncFile << syncs[nSyncs] << endl;
			if ( !syncsProvided ) nSyncs++;
		}
		// If there is a gap between records, warn that it will be filled with zeros
		// (gaps smaller than a full record are not considered here, this is *not* supposed to happen)
		else if ( gap > ncsFiles.getRecordDuration() )
		{
			ostringstream message;
			message.setf(ios::fixed,ios::floatfield);
			message.precision(6);
			if ( doWideband || doTimestamps ) message << "(filling " << gap/1e6 << " @ " << t/1e6 << ")";
			else message << "(large gap " << gap/1e6 << " @ " << t/1e6 << ")";
			ncsFiles.fillMissingRecords(widebandFile);
			progress.message(message.str());
		}
		// If gap between records is negative, this is *bad*!
		else if ( gap < -NCS_MAX_OVERLAP )
		{
			ostringstream message;
			message.setf(ios::fixed,ios::floatfield);
			message.precision(6);
			message << "negative gap " << gap/1e6 << " @ " << t/1e6;
			progress.message(message.str());
			throw NLXError(NLXError::READ_ERROR,"negative gap between successive records.");
		}

		// Write data to output files
		if ( doWideband ) ncsFiles.writeRecord(widebandFile);
		if ( doTimestamps ) ncsTimestampFile << t << endl;
	}
	// Close files
	if ( doTimestamps ) ncsTimestampFile.close();
	if ( doWideband ) widebandFile.close();
	if ( doSync ) syncFile.close();
}

/**
	Read sync info from file
*/
void NLXExtractor::readSyncs(string filename) throw(NLXError)
{
	ifstream file;
	file.open(filename.c_str(),ifstream::in);
	if ( !file.good() ) throw NLXError(NLXError::READ_ERROR,"cannot read '"+filename+"'.");
	while ( !file.eof() ) file >> syncs[nSyncs++];
}

/**
	Browse .ncs files to find sync info
*/
void NLXExtractor::findSyncs()
{
	Time gap,t;
	bool first = true;

	// Display progress bar
	ProgressBar progress(basename,"S*",ncsFiles.getNRecords());
	progress.start();
	while ( true )
	{
		// Update progress bar
		progress.advance();
		// Read next data record for each file
		if ( !ncsFiles.readRecord() ) break;
		gap = ncsFiles.getGap();
		t = ncsFiles.getTimestamp();

		if ( first ) { syncs[nSyncs++] = t; first = false; }

		// If recording was restarted at this point, skip and display infomation next to progress bar
		if ( isRestart(gap,t) )
		{
			syncs[nSyncs++] = gap;
			ncsFiles.skipMissingRecords();
		}
	}
}

/**
	Extract to .spots
*/
void NLXExtractor::extractSpots() throw(NLXError)
{
	bool doSpots = spots;
	bool doSpotsTimestamps = spots;
	bool doTimestamps = timestamps;

	// Create output files
	// (most of the code here deals with making sure we do not override existing files, except if option '-f' is set)
	if ( spots )
	{
		try
		{
			spotsFile.create();
		}
		catch ( NLXError e )
		{
			error(e.report());
			doSpots = false;
		}
		try
		{
			spotsTimestampFile.create();
		}
		catch ( NLXError e )
		{
			error(e.report());
			doSpotsTimestamps = false;
		}
	}
	if ( timestamps )
		try
		{
			nvtTimestampFile.create();
			nvtTimestampFile.setf(ios::fixed,ios::floatfield);
			nvtTimestampFile.precision(3);
		}
		catch ( NLXError e )
		{
			error(e.report());
			doTimestamps = false;
		}
	if ( !doSpots && !doSpotsTimestamps && !doTimestamps ) return;

	Time gap,t,shift = 0;

	// Start synchronizing
	int n = 0;
	if ( nSyncs == 0 )
	{
		// There must always be at least one sync point (start timestamp of .ncs files)
		warning("missing .ncs and .sync files, video cannot be synchronized with wideband signals.");
	}
	else shift = syncs[n++];

	// Display "empty" progress bar
	string step;
	if ( doSpots || doSpotsTimestamps ) step += "V";
	if ( doTimestamps ) { if ( step.length() != 0 ) step += ","; step += "Vt"; }
	ProgressBar progress(basename,step,nvtFile.getNFrames());
	progress.start();

	// Start reading records
	while ( !nvtFile.eof() )
	{
		// Read next frame
		if ( !nvtFile.readFrame() ) break;
		gap = nvtFile.getGap();
		t = nvtFile.getTimestamp();

		// Update progress bar
		progress.advance();

		// If acquisition was restarted, then skip and display a warning
		if ( isRestart(gap,t) )
		{
			ostringstream message;
			message.setf(ios::fixed,ios::floatfield);
			message.precision(6);
			if ( nSyncs != 0 )
			{
				shift += syncs[n++];
				message << "(restart @ " << t/1e6 << ")";
			}
			else message << "gap " << gap/1e6 << " @ " << t/1e6;
			progress.message(message.str());
		}
		// If gap between records is negative, this is *bad*!
		else if ( gap < -frameDuration )
		{
			ostringstream message;
			message.setf(ios::fixed,ios::floatfield);
			message.precision(6);
			message << "negative gap " << gap/1e6 << " @ " << t/1e6;
			progress.message(message.str());
			// Erroneous timestamps in spots files can later be corrected when creating pos files.
			// Hence, we do not throw an error here, and continue converting instead
			///throw NLXError(NLXError::READ_ERROR,"negative gap between successive frames.");
		}

		// Write data to output files
		if ( doSpots ) nvtFile.writeFrame(spotsFile);
		if ( doSpotsTimestamps ) spotsTimestampFile << t-shift << endl;
		if ( doTimestamps ) nvtTimestampFile << t << endl;
	}
	// Close files
	if ( doSpots ) { nvtFile.writeLastFrame(spotsFile); spotsFile.close();}
	if ( doSpotsTimestamps ) spotsTimestampFile.close();
	if ( doTimestamps ) nvtTimestampFile.close();
}

/**
	Resynchronize .spots file (using .smi file)
*/
void NLXExtractor::syncSpots() throw(NLXError)
{
	bool doSpotsTimestamps = spots;

	// Create output file
	// (most of the code here deals with making sure we do not override existing files, except if option '-f' is set)
	try
	{
		spotsTimestampFile.create();
	}
	catch ( NLXError e )
	{
		error(e.report());
		doSpotsTimestamps = false;
	}
	if ( !doSpotsTimestamps ) return;

	Time gap,t,shift = 0;

	// Start synchronizing
	int n = 0;
	if ( nSyncs == 0 )
	{
		// There must always be at least one sync point (start timestamp of .ncs files)
		warning("missing .ncs and .sync files, video cannot be synchronized with wideband signals.");
	}
	else shift = syncs[n++];

	// Display "empty" progress bar
	ProgressBar progress(basename,"V",smiFiles.getNFrames());
	progress.start();

	// Start reading records
	while ( true )
	{
		// Read next frame
		if ( !smiFiles.readFrame() ) break;
		gap = smiFiles.getGap();
		t = smiFiles.getTimestamp();

		// Update progress bar
		progress.advance();

		// If acquisition was restarted, then skip and display a warning
		if ( isRestart(gap,t) )
		{
			ostringstream message;
			message.setf(ios::fixed,ios::floatfield);
			message.precision(6);
			if ( nSyncs != 0 )
			{
				shift += syncs[n++];
				message << "(restart @ " << t/1e6 << ")";
			}
			else message << "gap " << gap/1e6 << " @ " << t/1e6;
			progress.message(message.str());
		}
		// If gap between records is negative, this is *bad*!
		else if ( gap < -frameDuration )
		{
			ostringstream message;
			message.setf(ios::fixed,ios::floatfield);
			message.precision(6);
			message << "negative gap " << gap/1e6 << " @ " << t/1e6;
			progress.message(message.str());
			// Erroneous timestamps in spots files can later be corrected when creating pos files.
			// Hence, we do not throw an error here, and continue converting instead
			///throw NLXError(NLXError::READ_ERROR,"negative gap between successive frames.");
		}

		// Write data to output file
		spotsTimestampFile << t-shift << endl;
	}
	// Close file
	spotsTimestampFile.close();
}

/**
	Extract to .evt
*/
void NLXExtractor::extractEvents() throw(NLXError)
{
	bool doEvents = events;
	bool doTimestamps = timestamps;

	// Open output files
	// (most of the code here deals with making sure we do not override existing files, except if option '-f' is set)
	if ( events )
		try
		{
			eventFile.create();
			eventFile.setf(ios::fixed,ios::floatfield);
			eventFile.precision(3);
		}
		catch ( NLXError e )
		{
			error(e.report());
			doEvents = false;
		}
	if ( timestamps )
		try
		{
			nevTimestampFile.create();
			nevTimestampFile.setf(ios::fixed,ios::floatfield);
			nevTimestampFile.precision(3);
		}
		catch ( NLXError e )
		{
			error(e.report());
			doTimestamps = false;
		}
	if ( !doEvents && !doTimestamps ) return;

	// Start synchronizing
	int n = 0;
	if ( nSyncs == 0 )
	{
		// There must always be at least one sync point (start timestamp of .ncs files)
		warning("missing .ncs and .sync files, events cannot be synchronized with wideband signals.");
	}
	else nevFile.setShift(syncs[n++]);

	// Display "empty" progress bar
	string step;
	if ( doEvents ) step += "E";
	if ( doTimestamps ) { if ( step.length() != 0 ) step += ","; step += "Et"; }
	ProgressBar progress(basename,step,nevFile.getNRecords());
	progress.start();

	Time t;

	while ( true )
	{
		// Read next record
		if ( !nevFile.readRecord() ) break;
		t = nevFile.getTimestamp();

		// Update progress bar
		progress.advance();

		// Warn if acquisition was restarted
		if ( nevFile.restarted() )
		{
			// Display infomation next to progress bar
			ostringstream message;
			message.setf(ios::fixed,ios::floatfield);
			message.precision(6);

			if ( nSyncs != 0 )
			{
				nevFile.addToShift(syncs[n++]);
				message << "(restart @ " << t/1e6 << ")";
			}
			else message << "restart @ " << t/1e6;
			progress.message(message.str());
		}

		// Write data to output files
		if ( doEvents ) nevFile.writeRecord(eventFile);
		if ( doTimestamps ) nevTimestampFile << t << endl;

	}
	// Close files
	if ( doEvents ) eventFile.close();
	if ( doTimestamps ) nevTimestampFile.close();
}

/**
	Read 'restart acquisition' events from file
*/
void NLXExtractor::readRestarts(string filename) throw(NLXError)
{
	ifstream file;
	file.open(filename.c_str(),ifstream::in);
	if ( !file.good() ) throw NLXError(NLXError::READ_ERROR,"cannot read '"+filename+"'.");
	while ( !file.eof() ) file >> restarts[nRestarts++];
	restartsProvided = true;
	restartsUndefined = false;
}

/**
	Find 'restart acquisition' events
*/
void NLXExtractor::findRestarts()
{
	bool doRestart = restart;

	if ( restart )
		try
		{
			restartFile.create();
			restartFile.setf(ios::fixed,ios::floatfield);
			restartFile.precision(3);
		}
		catch ( NLXError e )
		{
			error(e.report());
			doRestart = false;
		}

	// Display "empty" progress bar
	string step = "A";
	if ( !restart ) step += "*";
	ProgressBar progress(basename,step,nevFile.getNRecords());
	progress.start();

	while ( true )
	{
		if ( !nevFile.readRecord() ) break;

		// Update progress bar
		progress.advance();

		if ( nevFile.restarted() )
		{
			restarts[nRestarts] = nevFile.getTimestamp();
			if ( doRestart ) restartFile << restarts[nRestarts] << endl;
			nRestarts++;
		}
	}
	restartsUndefined = false;

	// Reset nev file
	nevFile.init();
}

/**
	Does a given timestamp correspond to a restart?
*/
bool NLXExtractor::isRestart(Time gap,Time timestamp)
{
	// A data record is considered to match a restart event if it is close in time and follows a large sampling gap
	// ('close' is taken as half the max gap, so that no subsequent timestamp can match the same restart event)
	if ( fabs(gap) <= threshold ) return false;
	if ( restartsUndefined ) return true; // best guess if no nev and no restart files are provided
	for ( int i = 0 ; i < nRestarts ; ++ i ) if ( fabs(restarts[i]-timestamp) < threshold/2 ) return true;
	return false;
}

/**
	Extract
*/
void NLXExtractor::extract() throw(NLXError)
{
	// If restart file was not provided among inputs, and .nev file is available, find 'restart acquisition' events
	if ( nevProvided && !restartsProvided ) findRestarts();

	// Process wideband signals (convert them, get sync points, extract timestamps)
	if ( (sync || wideband || timestamps) && ncsProvided )
		try
		{
			extractWideband();
		}
		catch(NLXError e)
		{
			error(e.report());
		};
	// Process video signals (convert them, extract timestamps)
	if ( (spots || timestamps) && nvtProvided )
		try
		{
			// If sync file was not provided among inputs, and .ncs file is available, find sync info
			if ( !sync && !wideband && !timestamps && ncsProvided && !syncsProvided ) findSyncs();
			extractSpots();
		}
		catch(NLXError e)
		{
			error(e.report());
		};
	// Process video signals (synchronize timestamps)
	if ( spots && smiProvided )
		try
		{
			// If sync file was not provided among inputs, and .ncs file is available, find sync info
			if ( !sync && !wideband && !timestamps && ncsProvided && !syncsProvided ) findSyncs();
			syncSpots();
		}
		catch(NLXError e)
		{
			error(e.report());
		};
	// Process events (convert them, extract timestamps)
	if ( (events || timestamps) && nevProvided )
		try
		{
			// If sync file was not provided among inputs, and .ncs file is available, find sync info
			if ( !sync && !wideband && !timestamps && !spots && ncsProvided && !syncsProvided ) findSyncs();
			extractEvents();
		}
		catch(NLXError e)
		{
			error(e.report());
		};
}

/**
	Show file headers
*/
void NLXExtractor::showFileHeaders()
{
	if ( ncsProvided ) ncsFiles.showFileHeaders();
	if ( nvtProvided ) nvtFile.showFileHeader();
	if ( nevProvided ) nevFile.showFileHeader();
}
