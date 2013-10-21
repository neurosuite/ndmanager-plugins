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
#ifndef ANATOMYPAGE_H
#define ANATOMYPAGE_H

//include files for the application
#include <anatomylayout.h>

// include files for QT
#include <qwidget.h>
#include <qpushbutton.h>
#include <qmap.h>
#include <qregexp.h>
#include <qlineedit.h>

#include <QEvent>
#include <QList>

/**
* Class used to represent the information linked to the anatomical groups.
* @author Lynn Hazan
*/
class AnatomyPage : public AnatomyLayout
{
    Q_OBJECT
public:
    /**Constructor.*/
    explicit AnatomyPage(QWidget* parent = 0);
    /**Destructor.*/
    ~AnatomyPage();

    /**Initializes the attribute table, sitting the values of each attribute for each channel.
 * @param attributes reference to the map given the correspondance between the attribute names and a map given for each channel the value of the attribute.
 */
    void setAttributes(const QMap<QString, QMap<int,QString> >& attributes);

    /** Returns a map given the values of each attribute for each channel.
 * @param attributesMap map given the correspondance between the attribute names and a map given for each channel the value of the attribute.
 */
    void getAttributes(QMap<QString, QMap<int,QString> >& attributesMap)const;

    /** Initializes the group table.
 * @param groups map containing the list of channels for each group except the trash group.
 */
    void setGroups(const QMap<int, QList<int> >& groups);

    /**Returns the composition of the anatomical groups.
 * @param groups map containing the list of channels for each group except trash group.
 */
    void getGroups(QMap<int, QList<int> >& groups)const;

    /**Initializes the number of channels used.
 * @param nbChannels number of channels.
 */
    void setNbChannels(int nbChannels);

    /**True if at least one property has been modified, false otherwise.*/
    bool isModified()const{return modified;}
    void setModified(bool b) { modified = b; }
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

    /**Validates the current entry in the attribute table.*/
    void attributeChanged(int row,int column);

    /** Will be called when any properties is modified.*/
    void propertyModified(){modified = true;}

    /**Resets the internal modification status to false.*/
    void resetModificationStatus(){modified = false;}

private:
    int incorrectRow;
    bool isIncorrectRow;
    bool modified;
};



#endif
