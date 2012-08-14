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
// application specific includes
#include "pref.h"

// include files for KDE
#include <klocale.h>
#include <kmessagebox.h>

// include files for QT
#include <qlayout.h>
#include <qlabel.h>

ndManagerPreferences::ndManagerPreferences(QWidget* parent,const char* name, WFlags f)
    : KDialogBase(TreeList, i18n("ndManager Preferences"),
                  Help|Default|Ok|Apply|Cancel,Ok, parent, name, f)
{
  setHelp("settings","ndManager");
 
  QFrame *frame;
  frame = addPage(i18n("First Page"), i18n("Page One Options"));
  m_pageOne = new ndManagerPrefPageOne(frame);

  frame = addPage(i18n("Second Page"), i18n("Page Two Options"));
  m_pageTwo = new ndManagerPrefPageTwo(frame);
  
  // connect interactive widgets and selfmade signals to the enableApply slotDefault
 // connect(prefGeneral->headerCheckBox,SIGNAL(clicked()),this,SLOT(enableApply()));
   
 applyEnable = false;   
}


void ndManagerPreferences::updateDialog() {
 // prefGeneral->setBackgroundColor(configuration().getBackgroundColor());
 
  enableButtonApply(false);   // disable apply button
  applyEnable = false;
}
 

void ndManagerPreferences::updateConfiguration(){
 // configuration().setBackgroundColor(prefGeneral->getBackgroundColor());
                 
  enableButtonApply(false);   // disable apply button
  applyEnable = false;
}


void ndManagerPreferences::slotDefault() {
  if(KMessageBox::warningContinueCancel(this, i18n("This will set the default options "
      "in ALL pages of the preferences dialog! Do you wish to continue?"), i18n("Set default options?"),
      i18n("Set defaults"))==KMessageBox::Continue){
        
   
   enableApply();   // enable apply button
  }
}


void ndManagerPreferences::slotApply() {
  updateConfiguration();      // transfer settings to configuration object
  emit settingsChanged();     // apply the preferences
  enableButtonApply(false);   // disable apply button again
}


void ndManagerPreferences::enableApply() {
    enableButtonApply(true);   // enable apply button
    applyEnable = true;
}





ndManagerPrefPageOne::ndManagerPrefPageOne(QWidget *parent)
    : QFrame(parent)
{
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setAutoAdd(true);

    new QLabel(i18n("Add something here"), this);
}

ndManagerPrefPageTwo::ndManagerPrefPageTwo(QWidget *parent)
    : QFrame(parent)
{
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setAutoAdd(true);

    new QLabel(i18n("Add something here"), this);
}
#include "pref.moc"
