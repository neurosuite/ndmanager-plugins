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
#ifndef PROGRAMINFORMATION_H
#define PROGRAMINFORMATION_H

// include files for QT
#include <qmap.h>

#include <QStringList>
#include <QString>
/**
* Class storing the information describing a program to be used to process the raw data.
@author Lynn Hazan
*/
class ProgramInformation{
public:
    /**Constructor.*/
    ProgramInformation(){}
    /**Destructor.*/
    ~ProgramInformation(){}

    /**Sets the name of the program.
  * @param name name of the program.
  */
    void setProgramName(const QString& name);
    
    /**Sets the help available on the program.
  * @param help help about the program.
  */
    void setHelp(const QString& help);

    /** Initializes the parameter table.
 * @param parameters map containing the list of parameters.
 * Each entry is a list containing the name of the parameter its status (optional, mandatory or dynamic) and its value if any.
 */
    void setParameterInformation(const QMap<int, QStringList >& parameters);

    /**Gets the name of the program.*/
    QString getProgramName() const;

    /**Gets the help about the program.*/
    QString getHelp() const;

    /**Returns the information for each parameter.
 * @return map containing the list of parameters. Each entry contains the name of the parameter, its value and its status (optional, mandatory or dynamic).
 */
    QMap<int, QStringList > getParameterInformation() const;

private:
    QString name;
    QMap<int, QStringList > parameters;
    QString help;
};

#endif
