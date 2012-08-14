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
// application specific includes
#include "configuration.h"

// include files for KDE
#include <kapplication.h>       // for 'kapp'
#include <kconfig.h>            // for KConfig
#include <klocale.h>            // for i18n()


Configuration::Configuration(){
    read(); // read the settings or set them to the default values
}


void Configuration::read() {
//  KConfig* conf = kapp->config();

 //read general options
 //conf->setGroup("General");
 
}

void Configuration::write() const {  
//  KConfig* conf = kapp->config();
 //write general options
 //conf->setGroup("General");
 
}

Configuration& configuration() {
  //The C++ standard requires that static variables in functions
  //have to be created upon first call of the function.
  static Configuration conf;
  return conf;
}


