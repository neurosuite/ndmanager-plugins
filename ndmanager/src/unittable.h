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
#ifndef UNITTABLE_H
#define UNITTABLE_H

#include <QTableWidget>
#include <QDebug>

/**
    @author Lynn Hazan <lynn.hazan@myrealbox.com>
*/

class UnitTableItem : public QTableWidgetItem
{
public:
    explicit UnitTableItem(const QString& text)
        : QTableWidgetItem(text)
    {
    }
    static QString key(const QString& txt) {
        QString text = txt;
        float fVal = text.toFloat();
        if (fVal > 0) // true only if txt is a float number
        {
            text.prepend("00000");
            text = text.right(5);
            text.append(".0");
        }
        return text;
    }


    bool operator <(const QTableWidgetItem &other) const
    {
        return key(text()) < key(other.text());
    }
};

class UnitTable : public QTableWidget
{
public:
    explicit UnitTable(QWidget * parent = 0 );
    UnitTable( int numRows, int numCols, QWidget * parent = 0 );

    ~UnitTable();

};

#endif
