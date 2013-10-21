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
#ifndef SPIKEPAGE_H
#define SPIKEPAGE_H

//include files for the application
#include <spikelayout.h>

// include files for QT
#include <qwidget.h>
#include <qpushbutton.h>
#include <qregexp.h>
#include <qlineedit.h>

#include <QEvent>
#include <QList>


/**
* Class used to represent the information linked to the spike groups.
*@author Lynn Hazan
*/
class SpikePage : public SpikeLayout
{
    Q_OBJECT
public:
    /**Constructor.*/
    explicit SpikePage(QWidget*parent = 0);
    /**Destructor.*/
    ~SpikePage();

    /** Initializes the group table.
 * @param groups map containing the list of channels for each sipke group except the trash and undefined groups.
 * @param information map containing the additional information for each spike group except the trash and undefined groups.
 */
    void setGroups(const QMap<int, QList<int> >& groups,const QMap<int,  QMap<QString,QString> >& information);

    /**Returns the composition of the spike groups.
 * @param groups map containing the list of channels for each group except trash and undefined groups.
 */
    void getGroups(QMap<int, QList<int> >& groups)const;

    /**Returns the additional information for each spike groups.
 * @param groupInformation map containing the additional information for each group except the trash and undefined groups.
 */
    void getGroupInformation(QMap<int,  QMap<QString,QString> >& groupInformation)const;

    /**True if at least one property has been modified, false otherwise.*/
    bool isModified() const {return modified;}
    void setModified(bool b) { modified = b;}

    /**Returns the current number of group of spikes.
*/
    inline int getNbGroups() const {return groupTable->rowCount();}

signals:
    void nbGroupsModified(int nbGroups);

protected:
    /** Event filter to validate the entries in the group table.
 * @param object target object for the event.
 * @param event event sent.
 */
    bool eventFilter(QObject* object,QEvent* event);

public slots:
    /**Adds a new line to the group table.*/
    void addGroup();

    /**Removes the selected lines from the group table.*/
    void removeGroup();

    /**Validates the current entry in the group table.*/
    void slotValidate();

    /**Validates the current entry in the group table.*/
    void groupChanged(int row,int column);
    /** Will be called when any properties is modified.*/
    void propertyModified(){modified = true;}

    /**Resets the internal modification status to false.*/
    void resetModificationStatus(){modified = false;}

private:
    bool isIncorrectRow;
    int incorrectRow;
    int incorrectColumn;
    bool modified;
};

#endif
