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

#ifndef _NDMANAGERPREF_H_
#define _NDMANAGERPREF_H_

#include <kdialogbase.h>
#include <qframe.h>

class ndManagerPrefPageOne;
class ndManagerPrefPageTwo;

class ndManagerPreferences : public KDialogBase
{
    Q_OBJECT
public:
  ndManagerPreferences(QWidget* parent,const char* name=0, WFlags f=0);
  
  /** Transfers the settings from the configuration object to the dialog.*/
  void updateDialog();
   
  /** Transfers the settings from the dialog to the configuration object.*/
  void updateConfiguration();
   
  /** */
  inline bool isApplyEnable(){return applyEnable;};

 public slots:
   /**Will be called when the "Default" button has been clicked.*/
   void slotDefault();
   /**Will be called when the "Apply" button has been clicked.*/
   void slotApply();
   /**Will be called whenever a setting was changed.*/
   void enableApply();

 signals:
   /// Will be emitted when the new settings should be applied.
   void settingsChanged();

private:
  ndManagerPrefPageOne* m_pageOne;
  ndManagerPrefPageTwo* m_pageTwo;
  bool applyEnable;
};

class ndManagerPrefPageOne : public QFrame
{
    Q_OBJECT
public:
    ndManagerPrefPageOne(QWidget *parent = 0);
};

class ndManagerPrefPageTwo : public QFrame
{
    Q_OBJECT
public:
    ndManagerPrefPageTwo(QWidget *parent = 0);
};

#endif // _NDMANAGERPREF_H_
