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
#include "unittableitem.h"
//Added by qt3to4:
#include <QPixmap>


UnitTableItem::UnitTableItem ( Q3Table * table, EditType et ): Q3TableItem(table,et)
{
}

UnitTableItem::UnitTableItem ( Q3Table * table, EditType et, const QString & text ): Q3TableItem(table,et,text)
{
}

UnitTableItem::UnitTableItem ( Q3Table * table, EditType et, const QString & text, const QPixmap & p ): Q3TableItem(table,et,text,p)
{
}

UnitTableItem::~UnitTableItem()
{
}

QString UnitTableItem::key () const{
    QString txt = text();
    float fVal = txt.toFloat();
    if (fVal > 0) // true only if txt is a float number
    {
        txt.prepend("00000");
        txt = txt.right(5);
        txt.append(".0");
    }
    return txt;
}

