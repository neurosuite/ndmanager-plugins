/***************************************************************************
 *   Copyright (C) 2006 by Michael Zugaro                                  *
 *   michael.zugaro@college-de-france.fr                                   *
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
#ifndef UNITLISTPAGE_H
#define UNITLISTPAGE_H

//include files for the application
#include <unitlistlayout.h>
#include "unittable.h"

// include files for QT
#include <qwidget.h>
#include <qpushbutton.h>
#include <qmap.h>
#include <qregexp.h>
#include <qlineedit.h>

#include <QEvent>
#include <QList>

/**
* Class used to represent the information linked to the list of units.
* @author Michael Zugaro
*/
class UnitListPage : public UnitListLayout
{
    Q_OBJECT

public:
    /**Constructor.*/
    explicit UnitListPage(QWidget* parent = 0);
    /**Destructor.*/
    ~UnitListPage();
    /** Initializes the unit table.
        * @param units map containing the info for each unit.
        */
    void setUnits(const QMap<int, QStringList >& units);
    /**Returns the composition of the unit table.
        * @param units map containing the info for each unit.
        */
    void getUnits(QMap<int, QStringList >& units)const;
    /**Initializes the number of units classified.
        * @param nbUnits number of units.
        */
    void setNbUnits(int nbUnits);

    /**True if at least one property has been modified, false otherwise.*/
    inline bool isModified()const{return modified;}

protected:
    /** Event filter to validate the entries in the unit table.
        * @param object target object for the event.
        * @param event event sent.
        */
    bool eventFilter(QObject* object,QEvent* event);

public slots:
    /**Adds a new line to the unit table.*/
    void addUnit();

    /**Removes the selected lines from the unit table.*/
    void removeUnit();
    /**selection moved to a different entry in the unit table.*/
    void currentChanged();

    /**Validates an entry in the unit table.*/
    void unitChanged(int row,int column);

    /** Will be called when any properties is modified.*/
    void propertyModified(){modified = true;}
    /**Resets the internal modification status to false.*/
    void resetModificationStatus(){modified = false;}

private:
    bool	isIncorrect;
    int	incorrectRow,incorrectColumn;
    bool	modified;
};



#endif
