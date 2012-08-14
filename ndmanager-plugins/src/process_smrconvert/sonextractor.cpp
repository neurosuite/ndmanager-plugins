/***************************************************************************
 *                                                                         *
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

#include "sonfile.h"
#include "sonextractor.h"
#include "sonfileheader.h"
#include "sonchannelinfo.h"
#include "sonadcchannels.h"
#include "sonadcmarkchannel.h"
#include "soneventchannel.h"

extern bool verbose;

SONExtractor::SONExtractor(SONFile *file,bool header,bool dat,bool spk,bool evt,bool force,bool reverse,uint16_t nChannelsListed,uint16_t channelList[],bool include) :
file(file),
basename(""),
datName(""),
spkName(""),
resName(""),
evtName(""),
header(header),
dat(dat),
spk(spk),
evt(evt),
force(force),
reverse(reverse),
nADCChannels(0),
nADCMarkChannels(0),
nEventChannels(0),
nChannelsListed(nChannelsListed),
channelList(channelList),
include(include)
{
	string filename = file->getName();
	const char *slash = strrchr(filename.c_str(),'/');
	if ( slash ) filename = slash+1;
	int dot = filename.find_last_of('.');
	basename = filename.substr(0,dot);
	datName = basename+".dat";
}

SONExtractor::~SONExtractor()
{
}

void SONExtractor::checkDat() throw(SONError)
{
	if ( !force )
	{
		datFile.open(datName.c_str(),ios::in);
		bool exists = datFile.good();
		datFile.close();
		datFile.clear();
		if ( exists ) throw(SONError(SONError::WRITE_ERROR,"'" + datName + "' exists. Aborting."));
	}
}

void SONExtractor::checkSpk() throw(SONError)
{
	if ( !force )
		for ( uint16_t i = 0 ; i < nADCMarkChannels ; ++i )
		{
			char n[256];
			sprintf(n,"%d",i+1);
			spkName = basename + ".spk." + n;
			spkFile.open(spkName.c_str(),ios::in);
			bool exists = spkFile.good();
			spkFile.close();
			spkFile.clear();
			if ( exists ) throw(SONError(SONError::WRITE_ERROR,"'" + spkName + "' exists. Aborting."));
			resName = basename + ".res." + n;
			resFile.open(resName.c_str(),ios::in);
			exists = resFile.good();
			resFile.close();
			resFile.clear();
			if ( exists ) throw(SONError(SONError::WRITE_ERROR,"'" + resName + "' exists. Aborting."));
		}
}

void SONExtractor::checkEvt() throw(SONError)
{
	if ( !force )
		for ( uint16_t i = 0 ; i < nEventChannels ; ++i )
		{
			char n[256];
			sprintf(n,"%d",i+1);
			evtName = basename + ".evt." + n;
			evtFile.open(evtName.c_str(),ios::in);
			bool exists = evtFile.good();
			evtFile.close();
			evtFile.clear();
			if ( exists ) throw(SONError(SONError::WRITE_ERROR,"'" + evtName + "' exists. Aborting."));
		}
}

void SONExtractor::extractDat() throw(SONError)
{
	SONADCChannels		channels(file,nADCChannels,ADCChannels);

	cout << "...extracting info for " << nADCChannels << " ADC channels: " << flush;
	if ( verbose ) cout << endl;

	datFile.open(datName.c_str(),ios::out);
	if ( !datFile.good() ) throw SONError(SONError::WRITE_ERROR,"cannot write to '" + datName + "'.");
	// Read block headers and first data buffer (write it too)
	channels.read();
	channels.write(datFile,reverse);

	cout << endl << "...extracting data for " << nADCChannels << " ADC channels: ";
	int totalNMarks = 50,nMarks = 0,nMarksToAdd = 0;
	cout << "0% ";
	for ( int i = 0 ; i < totalNMarks ; ++i) cout << '.';
	cout << " 100%\b\b\b\b\b";
	for ( int i = 0 ; i < totalNMarks ; ++i) cout << '\b';
	cout << flush;

	while ( !channels.done() )
	{
		channels.read();
		channels.write(datFile,reverse);
		if ( !verbose )
		{
			nMarksToAdd = (int) (channels.percentDone() * totalNMarks - nMarks);
			for ( int i = 0 ; i < nMarksToAdd ; ++i) cout << '#';
			cout << flush;
			nMarks += nMarksToAdd;
		}
	}
	datFile.close();
	cout << endl;
}

void SONExtractor::extractEvt() throw(SONError)
{
	cout << "...extracting data for " << nEventChannels << " event channels: " << flush;

	for ( uint16_t i = 0 ; i < nEventChannels ; ++i )
	{
		SONEventChannel		channel(file,eventChannels[i]);

		string zeros;
		uint16_t c = eventChannels[i];
		char n[256];
		sprintf(n,"%d",c);
		if ( c < 10 ) zeros = "e0";
		else if ( c < 100 ) zeros = "e";
		else zeros = "";

		evtName = basename + "." + zeros + n +".evt";

		try
		{
			channel.read();
			evtFile.open(evtName.c_str(),ios::out);
			if ( !evtFile.good() ) throw SONError(SONError::WRITE_ERROR,"cannot write to '" + evtName + "'.");
			channel.write(evtFile);
			cout << eventChannels[i] << " " << flush;
		}
		catch (SONError error)
		{
			if ( error.getType() != SONError::EMPTY_CHANNEL ) throw;
			cout << "(" << eventChannels[i] << ")" << " " << flush;
		}

		evtFile.close();
	}
	cout << endl;
}

void SONExtractor::extractSpk() throw(SONError)
{
	cout << "...extracting data for " << nADCMarkChannels << " waveform channels: " << flush;

	for ( uint16_t i = 0 ; i < nADCMarkChannels ; ++i )
	{
		SONADCMarkChannel		channel(file,ADCMarkChannels[i]);

		char n[256];
		sprintf(n,"%d",i+1);
		spkName = basename + ".spk." + n;
		resName = basename + ".res." + n;

		resFile.open(resName.c_str(),ios::out);
		if ( !resFile.good() ) throw SONError(SONError::WRITE_ERROR,"cannot write to '" + resName + "'.");
		spkFile.open(spkName.c_str(),ios::out);
		if ( !spkFile.good() ) throw SONError(SONError::WRITE_ERROR,"cannot write to '" + spkName + "'.");
		channel.read();
		channel.write(resFile,spkFile,reverse);
		cout << ADCMarkChannels[i] << " " << flush;
		spkFile.close();
		resFile.close();
	}
	cout << endl;
}

void SONExtractor::extract() throw(SONError)
{
	// Open input file, and read file header + channel info
	file->open();
	file->init();

	// Output file header to stdout
	if ( header )
	{
		cout << endl << "--- FILE HEADER ---------------------------------------" << endl << endl;
		cout << *file->getFileHeader();
	}
	// Collect and output info
	uint16_t nChannels = file->getNChannels();
	float commonSamplingFrequency = -1;
	bool frequencyError = false;

	for ( uint16_t i = 0 ; i < nChannels; ++i )
	{
		if ( file->isADCChannel(i) )
		{
			// Test whether this channel should be extracted or excluded
			bool found = false;
			for ( uint16_t j = 0 ; j < nChannelsListed ; ++j ) if ( channelList[j] == i ) { found = true; break; }
			if ( (found && !include) || (!found && include) ) continue;
			else ADCChannels[nADCChannels++] = i;
		}
		else if ( file->isADCMarkChannel(i) ) ADCMarkChannels[nADCMarkChannels++] = i;
		else if ( file->isEventChannel(i) ) eventChannels[nEventChannels++] = i;
		else continue;

		if ( header )
		{
			cout << endl << "--- CHANNEL INFO ------------------------------------" << endl << endl;
			cout << *file->getChannelInfo(i);
			if ( file->isADCChannel(i) )
			{
				float channelFrequency = file->getSamplingFrequency(i);
				cout << "=> Sampling frequency " << channelFrequency << " Hz" << endl;
				if ( commonSamplingFrequency == -1 )
					commonSamplingFrequency = channelFrequency;
				else if ( channelFrequency != commonSamplingFrequency ) frequencyError = true;
			}
			else if ( file->isADCMarkChannel(i) ) cout << "=> Sampling frequency " << file->getSamplingFrequency(i) << " Hz" << endl;
		}
	}
	if ( frequencyError )
	{
		string error = "ADC Channels have different sampling frequencies";
		throw SONError(SONError::READ_ERROR,error);
	}
	if ( header )
	{
		cout << endl;
		cout << "--- SUMMARY -----------------------------------------" << endl << endl;
		cout << "Event Channels              " << nEventChannels << endl;
		cout << "ADC Channels                " << nADCChannels << " (";
		for ( uint16_t i = 0 ; i < nADCChannels ; ++i ) cout << ADCChannels[i] << " ";
		cout << "\b)" << endl;
		if ( nADCChannels >= 1 )
			cout << "ADC Sampling Rate           " << commonSamplingFrequency << endl;
		cout << "ADC Mark Channels           " << nADCMarkChannels << endl << endl;
		cout << "-----------------------------------------------------" << endl << endl;
	}

	// Overwrite existing output files?
	if ( dat ) checkDat();
	if ( spk ) checkSpk();
	if ( evt ) checkEvt();

	// Extract
	if ( dat ) extractDat();
	if ( spk ) extractSpk();
	if ( evt ) extractEvt();
	file->close();
}
