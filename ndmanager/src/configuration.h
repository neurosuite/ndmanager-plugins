/***************************************************************************
 *   Copyright (C) 2004 by Lynn Hazan                                      *
 *   lynn.hazan@myrealbox.com                                              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef CONFIGURATION_H
#define CONFIGURATION_H

// include files for QT

/**
@author Lynn Hazan
*/
class Configuration{
public:
    
 /** Reads the configuration data from the application config file.
 * If a property does not already exist in the config file it will be
 * set to a default value.*/
 void read();
 /** Writes the configuration data to the application config file.*/
 void write() const;

 Configuration();
 inline ~Configuration(){};
 Configuration(const Configuration&);

 friend Configuration& configuration();

};

// Returns a reference to the application configuration object.
Configuration& configuration();

#endif
