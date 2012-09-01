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
//include files for the application
#include "acquisitionsystempage.h"
#include <QMessageBox>

//General C++ include files




AcquisitionSystemPage::AcquisitionSystemPage(QWidget *parent, const char *name)
 : AcquisitionSystemLayout(parent),doubleValidator(this),intValidator(this),isInit(true),reset(false),isLostFocus(false),
 isReturnPressed(false),modified(false),isCheckAsked(false){

 //Set a validator on the line edits, the values have to be integers.
 nbChannelsLineEdit->setValidator(&intValidator);
 samplingRateLineEdit->setValidator(&doubleValidator);
 offsetLineEdit->setValidator(&intValidator);
 voltageRangeLineEdit->setValidator(&intValidator);
 amplificationLineEdit->setValidator(&intValidator);
 
 connect(nbChannelsLineEdit,SIGNAL(returnPressed()),this,SLOT(nbChannelsLineEditReturnPressed()));
 connect(nbChannelsLineEdit,SIGNAL(lostFocus()),this,SLOT(nbChannelsLineEditLostFocus()));
 
 
 connect(resolutionComboBox,SIGNAL(activated(int)),this,SLOT(propertyModified()));
 connect(voltageRangeLineEdit,SIGNAL(textChanged(const QString&)),this,SLOT(propertyModified()));
 connect(amplificationLineEdit,SIGNAL(textChanged(const QString&)),this,SLOT(propertyModified()));
 connect(samplingRateLineEdit,SIGNAL(textChanged(const QString&)),this,SLOT(propertyModified()));
 connect(offsetLineEdit,SIGNAL(textChanged(const QString&)),this,SLOT(propertyModified()));
}


AcquisitionSystemPage::~AcquisitionSystemPage(){}

void AcquisitionSystemPage::checkNbChannels(){
 isCheckAsked = true;
 nbChannelsChanged();
 isCheckAsked = false;
}

void AcquisitionSystemPage::nbChannelsLineEditReturnPressed(){
 isReturnPressed = true;
 nbChannelsChanged();
 isReturnPressed = false;
}

void AcquisitionSystemPage::nbChannelsLineEditLostFocus(){
 isLostFocus = true;
 nbChannelsChanged();
 isLostFocus = false;
}


void AcquisitionSystemPage::nbChannelsChanged(){
 int newNbChannels = nbChannelsLineEdit->text().toInt();
 
 //the return key has been press or the save action has been asked, the message box has triggered an lostFocusEvent
 if(isLostFocus && isReturnPressed || isLostFocus && isCheckAsked) return;
 if(newNbChannels != nbChannels && !isInit && !reset){
  if(QMessageBox::warning(this, tr("Change the number of channels?"), tr("Changing the number of channels "
      "will rest all the groups. Do you wish to continue?"),QMessageBox::Cancel|QMessageBox::Ok
      )==QMessageBox::Cancel){
   reset = true;   
   nbChannelsLineEdit->setText(QString::number(nbChannels));
   reset = false;
  }
  else{
   nbChannels = newNbChannels;
   modified = true;
   emit nbChannelsModified(getNbChannels());
  } 
 }
//  else{
//   nbChannels = newNbChannels;
//   emit nbChannelsModified(getNbChannels()); 
//  }
}



#include "acquisitionsystempage.moc"
