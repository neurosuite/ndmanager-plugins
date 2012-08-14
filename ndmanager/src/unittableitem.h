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
#ifndef UNITTABLEITEM_H
#define UNITTABLEITEM_H

#include <qtable.h>

/**
	@author Lynn Hazan <lynn.hazan@myrealbox.com>
*/
class UnitTableItem : public QTableItem
{
public:
	UnitTableItem ( QTable * table, EditType et );
	UnitTableItem ( QTable * table, EditType et, const QString & text );
	UnitTableItem ( QTable * table, EditType et, const QString & text, const QPixmap & p );

	QString key () const;
			
    ~UnitTableItem();

};

#endif
