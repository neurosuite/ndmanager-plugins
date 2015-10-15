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
#ifndef SONBLOCKHEADER_H
#define SONBLOCKHEADER_H

#include <stdint.h>

#include "sonerror.h"
#include "sonchannelinfo.h"

class SONFile;

/**
@author Michael Zugaro
*/
class SONBlockHeaders
{
	friend class SONExtractor;

	public:
		/**
			file offset coeff (file format version >= 9)
		*/
		static const int diskBlock;

	public:
		/**
			Default constructor (necessary to create arrays of SONBlockHeaders)
			@see init
		*/
   	SONBlockHeaders();
		/**
			Constructor
			@param	file	input file
			@param	channel	channel number
		*/
   	SONBlockHeaders(SONFile *file,uint16_t channel);
		/**
			Initialization (necessary to create arrays of SONBlockHeaders)
			@param	file	input file
			@param	channel	channel number
		*/
   	virtual inline void init(SONFile *file,uint16_t channel);
		/**
			Destructor
		*/
		virtual ~SONBlockHeaders();
		/**
			Read all block headers
		*/
		virtual void read() throw (SONError);
		/**
			Read next channel block header
		*/
		virtual void readNext() throw (SONError);
		/**
			Get total number of samples
		*/
		virtual inline int64_t getNSamples() const;
		/**
			Get number of blocks
			@return	number of blocks
		*/
		virtual inline uint32_t getNBlocks() const;
		/**
			Get block pointer
			@param	block	block number
			@return	block offset in file
		*/
		virtual inline int64_t getBlockOffset(uint32_t block) const;
		/**
			Get number of block samples
			@param	i	block number
			@return	number of samples in block
		*/
		virtual inline uint16_t getNSamplesInBlock(uint16_t i) const;

	private:

		SONFile			*file;
		uint16_t			channel;
		uint32_t			nBlocks;
		uint32_t			currentBlock;
		int64_t			*blockOffset;
		int32_t			nBlockOffset;
		int64_t			*nextBlockOffset;
		int32_t			*start;
		int32_t			*stop;
		uint16_t			*nSamplesInBlock;
		int64_t			nSamples;
};

void SONBlockHeaders::init(SONFile *file,uint16_t channel)
{
	this->file = file;
	this->channel = channel;
}

int64_t SONBlockHeaders::getNSamples() const
{
	return nSamples;
}

int64_t SONBlockHeaders::getBlockOffset(uint32_t block) const
{
	// This takes into account differences between file format versions (pre vs post 9)
	if ( block > nBlocks ) return 0;
	else return blockOffset[block];
}

uint32_t SONBlockHeaders::getNBlocks() const
{
	// This takes into account differences between file format versions (pre vs post 9)
	return nBlocks;
}

uint16_t SONBlockHeaders::getNSamplesInBlock(uint16_t i) const
{
	if ( i > nBlocks ) return 0;
	else return nSamplesInBlock[i];
}

#endif
