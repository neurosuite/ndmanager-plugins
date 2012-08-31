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


// include files for QT
#include <qlayout.h>
#include <qlabel.h>
//Added by qt3to4:
#include <QFrame>
#include <Q3HBoxLayout>
#include <QMessageBox>

ndManagerPreferences::ndManagerPreferences(QWidget* parent,const char* name, Qt::WFlags f)
    : KDialogBase(TreeList, tr("ndManager Preferences"),
                  Help|Default|Ok|Apply|Cancel,Ok, parent, name, f)
{
    setHelp("settings","ndManager");

    QFrame *frame;
    frame = addPage(tr("First Page"), tr("Page One Options"));
    m_pageOne = new ndManagerPrefPageOne(frame);

    frame = addPage(tr("Second Page"), tr("Page Two Options"));
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
    if(QMessageBox::warning(this, tr("Set default options?"), tr("This will set the default options "
                                                   "in ALL pages of the preferences dialog! Do you wish to continue?"))==QMessageBox::Ok){
        

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
    Q3HBoxLayout *layout = new Q3HBoxLayout(this);
    layout->setAutoAdd(true);

    new QLabel(tr("Add something here"), this);
}

ndManagerPrefPageTwo::ndManagerPrefPageTwo(QWidget *parent)
    : QFrame(parent)
{
    Q3HBoxLayout *layout = new Q3HBoxLayout(this);
    layout->setAutoAdd(true);

    new QLabel(tr("Add something here"), this);
}
#include "pref.moc"
