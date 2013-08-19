/***************************************************************************
 *   Copyright (C) 2004 by Lynn Hazan                                      *
 *   lynn.hazan@myrealbox.com                                              *
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
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef DESCRIPTIONWRITER_H
#define DESCRIPTIONWRITER_H
//include files for the application
#include "programinformation.h"

//include files for QT
#include <qdom.h> 
/**
This class writes a program description to disk.
@author Lynn Hazan
*/
class DescriptionWriter
{
public:
    DescriptionWriter();
    ~DescriptionWriter();

    /**Writes the xml tree to a description file given by @p url.
  * @param url url of the file to write to.
  * @return true if the description file could be write to disk, false otherwise.
  */
    bool writeTofile(const QString& url);

    /** Creates the elements containing the information for a program used to process the data link to the current parameter file.
 * @param programInformation a ProgramInformation containing the program parameter description.
 */
    void setProgramInformation(const ProgramInformation &programInformation);

private:

    /**The description document.*/
    QDomDocument doc;

    /**The root element.*/
    QDomElement root;

    /**The element containing the information about the program.*/
    QDomElement program;

};

#endif
