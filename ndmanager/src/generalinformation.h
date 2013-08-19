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
#ifndef GENERALINFORMATION_H
#define GENERALINFORMATION_H

// include files for QT
#include <qdatetime.h>
#include <qstring.h> 

/**
* Class storing the information contained in the General Information part of the parameter file.
@author Lynn Hazan
*/
class GeneralInformation{
public:

    GeneralInformation(){
        date = QDate::currentDate();
    }

    ~GeneralInformation(){}

    /**Sets the date.*/
    void setDate(const QDate& date){this->date = date;}

    /**Sets the experimenters.*/
    void setExperimenters(const QString& experimenters){this->experimenters = experimenters;}

    /**Sets the description.*/
    void setDescription(const QString& description){this->description = description;}

    /**Sets the notes.*/
    void setNotes(const QString& notes){this->notes = notes;}

    /**Gets the date.*/
    QDate getDate() const{return date;}

    /**Gets the experimenters.*/
    QString getExperimenters() const {return experimenters;}

    /**Gets the description.*/
    QString getDescription() const {return description;}

    /**Gets the notes.*/
    QString getNotes() const {return notes;}

private:
    QDate date;
    QString description;
    QString notes;
    QString experimenters;
};

#endif
