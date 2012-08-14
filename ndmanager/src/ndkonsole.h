/***************************************************************************
 *   Copyright (C) 2004 by Lynn Hazan                                      *
 *   lynn.hazan@myrealbox.com                                              *
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
#ifndef NDKONSOLE_H
#define NDKONSOLE_H

// include files for Qt
#include <qwidget.h>
#include <qlayout.h>
 
//Include files for KDE
#include <kparts/part.h> 


/**
 Based on KateConsole 
 Copyright (C) 2001 Christoph Cullmann <cullmann@kde.org>
 Copyright (C) 2002 Joseph Wenninger <jowenn@kde.org>
 Copyright (C) 2002 Anders Lund <anders.lund@lund.tdcadsl.dk>
 modified by Lynn Hazan.
*/
class NdKonsole : public QWidget
{
Q_OBJECT
public:

 /**Information retun after a call to addKonsole*/
  enum {OK=0,NO_KPART=1,PART_LOADING_ERROR=2};

  NdKonsole(QWidget *parent = 0, const char *name = 0);

  ~NdKonsole();
  
  bool runCommand(QString command) const;

public slots:
 /** Adds a Konsole and a LauncherPage.
 * @param url url of the currently open parameter file.
 * @return message indicating if the konsole could be added.
 */
 int loadConsoleIfNeeded(const KURL url);
     
signals:
 void beingDestroyed();
  
protected:
 void focusInEvent(QFocusEvent*) {if (part) part->widget()->setFocus();};
 virtual void showEvent(QShowEvent* event);
  
 private:
  KParts::ReadOnlyPart* part;
  QVBoxLayout* layout; 
  bool isPart;
  QString directory;
   
 // Only needed for Konsole
 private slots:
 
 void notifySize (int,int) {};
 void changeColumns (int) {};
 void changeTitle(int,const QString&) {};
 void slotDestroyed(QObject* part);  
    
};

#endif
