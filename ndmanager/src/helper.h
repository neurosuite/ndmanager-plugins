/***************************************************************************
 *   Copyright (C) 2006 by Lynn Hazan   *
 *   lynn.hazan@myrealbox.com   *
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
#ifndef HELPER_H
#define HELPER_H

// include files for QT
#include <qstring.h>
#include <qregexp.h>


/**
    @author Lynn Hazan <lynn.hazan@myrealbox.com>
*/
namespace  Helper{
    static QString doubleToString(double value){

        QString stringValue = QString::fromLatin1("%1").arg(value,0,'g',14);
        /* QRegExp regexp("(?:([0-9]*[.][0-9]*[1-9])0*$)|(?:([0-9]*)[.]0*$)");
    return stringValue.replace(regexp,"\\1\\2");*/
        return stringValue;
    }
}

#endif
