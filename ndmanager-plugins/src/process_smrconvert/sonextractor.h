/***************************************************************************
 *   Copyright (C) 2004-2008 by Michael Zugaro                             *
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
#ifndef SONEXTRACTOR_H
#define SONEXTRACTOR_H

#include <iostream>
#include <fstream>
#include <string>
#include <stdint.h>

#include "sonerror.h"

using namespace std;

/**
@author Michael Zugaro
*/
class SONExtractor
{
	public:
		/**
			Constructor
			@param	file	session file
			@param	header	convert header?
			@param	dat	convert to dat?
			@param	spk	convert to spk/res?
			@param	evt	convert to evt?
			@param	force	force overwrite?
			@param	reverse	reverse wideband data?
			@param	nChannelsListed	number of ADC channels to extract or exclude
			@param	channelList	list of ADC channels to extract or exclude
			@param	include	include (true) or exclude (false) listed channels
		*/
		SONExtractor(SONFile *file,bool header,bool dat,bool spk,bool evt,bool force,bool reverse,uint16_t nChannelsListed = 0,uint16_t channelList[] = NULL,bool include = true);
		/**
			Destructor
		*/
		virtual ~SONExtractor();
		/**
			Make sure .dat output file do not exist (unless forcing overwrite)
		*/
		virtual void checkDat() throw(SONError);
		/**
			Make sure .spk/.res output files do not exist (unless forcing overwrite)
		*/
		virtual void checkSpk() throw(SONError);
		/**
			Make sure .evt output file do not exist (unless forcing overwrite)
		*/
		virtual void checkEvt() throw(SONError);
		/**
			Extract to dat
		*/
		virtual void extractDat() throw(SONError);
		/**
			Extract to spk/res
		*/
		virtual void extractSpk() throw(SONError);
		/**
			Extract to evt
		*/
		virtual void extractEvt() throw(SONError);
		/**
			Extract
		*/
		virtual void extract() throw(SONError);

	private:

		SONFile		*file;
		string		basename;
		string		smrName;
		string		datName;
		string		spkName;
		string		resName;
		string		evtName;
		ifstream		smrFile;
		ofstream		datFile;
		fstream		spkFile;
		fstream		resFile;
		fstream		evtFile;
		bool			header;
		bool			dat;
		bool			spk;
		bool			evt;
		bool			force;
		bool			reverse;
		uint16_t		nADCChannels;
		uint16_t		ADCChannels[1000];
		uint16_t		nADCMarkChannels;
		uint16_t		ADCMarkChannels[1000];
		uint16_t		nEventChannels;
		uint16_t		eventChannels[1000];
		uint16_t		nChannelsListed;
		uint16_t		*channelList;
		bool			include;
};

#endif
