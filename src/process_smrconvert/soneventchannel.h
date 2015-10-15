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
#ifndef SONEVENTCHANNEL_H
#define SONEVENTCHANNEL_H

#include "sonchannel.h"
#include "sonerror.h"

/**
@author Michael Zugaro
*/
class SONEventChannel : public SONChannel
{
	public:
		/**
			Constructor
			@param	file	input file
			@param	channel channel ID
		*/
		SONEventChannel(SONFile *file,uint16_t channel);
		/**
			Destructor
		*/
		virtual ~SONEventChannel();
		/**
			Read Event data
		*/
		virtual void read() throw (SONError);
		/**
			Write event data

			@param	out	output stream (.evt file)
		*/
		virtual void write(ostream& out) const throw (SONError);

	private:

		int32_t			nEvents;
		int32_t			*time;
		char				*markers;
};

#endif
