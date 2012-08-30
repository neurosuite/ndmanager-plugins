/***************************************************************************
 *   Copyright (C) 2006 by Lynn Hazan   *
 *   lynn.hazan@myrealbox.com   *
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
#ifndef UNITTABLE_H
#define UNITTABLE_H

#include <q3table.h>

/**
    @author Lynn Hazan <lynn.hazan@myrealbox.com>
*/
class UnitTable : public Q3Table
{
public:
    UnitTable(QWidget * parent = 0, const char * name = 0 );
    UnitTable( int numRows, int numCols, QWidget * parent = 0, const char * name = 0 );

    ~UnitTable();

    virtual void columnClicked(int col);

};

#endif
