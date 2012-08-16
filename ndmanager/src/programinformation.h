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
#ifndef PROGRAMINFORMATION_H
#define PROGRAMINFORMATION_H

// include files for QT
#include <qmap.h>
//Added by qt3to4:
#include <Q3ValueList>

/**
* Class storing the information describing a program to be used to process the raw data.
@author Lynn Hazan
*/
class ProgramInformation{
public:
 /**Constructor.*/
 inline ProgramInformation(){};
 /**Destructor.*/
 inline ~ProgramInformation(){};

  /**Sets the name of the program.
  * @param name name of the program.
  */
  inline void setProgramName(QString name){this->name = name;};
    
  /**Sets the help available on the program.
  * @param help help about the program.
  */
  inline void setHelp(QString help){this->help = help;};
 
 /** Initializes the parameter table.
 * @param parameters map containing the list of parameters. 
 * Each entry is a list containing the name of the parameter its status (optional, mandatory or dynamic) and its value if any.
 */
 inline void setParameterInformation(QMap<int, Q3ValueList<QString> >& parameters){
  this->parameters = parameters;
 };
 
 /**Gets the name of the program.*/
 inline QString getProgramName() const{return name;};

 /**Gets the help about the program.*/
 inline QString getHelp() const{return help;};
 
 /**Returns the information for each parameter.
 * @return map containing the list of parameters. Each entry contains the name of the parameter, its value and its status (optional, mandatory or dynamic).
 */
 QMap<int, Q3ValueList<QString> > getParameterInformation(){return parameters;};
 
 private:
  QString name;
  QMap<int, Q3ValueList<QString> > parameters;
  QString help;
};

#endif
