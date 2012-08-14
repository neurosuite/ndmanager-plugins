/***************************************************************************
 *   Copyright (C) 2005-2008 by Michael Zugaro                             *
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
#ifndef SONCHANNEL_H
#define SONCHANNEL_H

#include <string>
#include "sonchannelinfo.h"

/**
@author Michael Zugaro
*/
class SONChannel{

	protected:

		/**
			Block header size (in bytes)
		*/
		static const int blockHeaderSize;
		/**
			marker size (in bytes)
		*/
		static const int markerSize;

	protected:
		/**
			Constructor (protected because this is an abstract class)
			@param	file		input file
			@param	channel	channel ID
		*/
   	SONChannel(SONFile *file,uint16_t channel);
		/**
			Destructor (protected because this is an abstract class)
		*/
		~SONChannel();

	protected:

		/**
			Embedding file object
		*/
		SONFile				*file;
		/**
			Channel ID
		*/
		int					channel;
};

#endif
