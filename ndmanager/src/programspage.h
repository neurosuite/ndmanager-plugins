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
#ifndef PROGRAMSPAGE_H
#define PROGRAMSPAGE_H

//include files for the application

// include files for QT
#include <qwidget.h>
#include <QFrame>

//forward declaration
class QPushButton;

/**
@author Lynn Hazan
*/
class ProgramsPage : public QFrame
{
    Q_OBJECT
public:
    /** Constructor
 * @param expertMode true if the file is opened in expert mode, false otherwise.
 * @param parent the parent QWidget.
 * @param name name of the widget (can be used for introspection).
 */
    explicit ProgramsPage(bool expertMode,QWidget *parent = 0);
    ~ProgramsPage();

signals:
    void addNewProgram();
    void programToLoad(const QString& programUrl);

public slots:
    void addProgram(){emit addNewProgram();}
    void loadProgram();
    
private:
    QPushButton* addButton;
    QPushButton* loadButton;
    QFrame* descriptionFrame;

    /**True if the file is opened in expert mode, false otherwise*/
    bool expertMode;
};

#endif
