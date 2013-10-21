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
//include files for the application
#include "acquisitionsystempage.h"
#include <QMessageBox>

AcquisitionSystemPage::AcquisitionSystemPage(QWidget *parent)
    : AcquisitionSystemLayout(parent),
      doubleValidator(this),
      intValidator(this),
      isInit(true),
      reset(false),
      isLostFocus(false),
      isReturnPressed(false),
      modified(false),
      isCheckAsked(false)
{

    //Set a validator on the line edits, the values have to be integers.
    nbChannelsLineEdit->setValidator(&intValidator);
    samplingRateLineEdit->setValidator(&doubleValidator);
    offsetLineEdit->setValidator(&intValidator);
    voltageRangeLineEdit->setValidator(&intValidator);
    amplificationLineEdit->setValidator(&intValidator);

    connect(nbChannelsLineEdit,SIGNAL(returnPressed()),this,SLOT(nbChannelsLineEditReturnPressed()));
    connect(nbChannelsLineEdit,SIGNAL(editingFinished()),this,SLOT(nbChannelsLineEditLostFocus()));


    connect(resolutionComboBox,SIGNAL(activated(int)),this,SLOT(propertyModified()));
    connect(voltageRangeLineEdit,SIGNAL(textChanged(QString)),this,SLOT(propertyModified()));
    connect(amplificationLineEdit,SIGNAL(textChanged(QString)),this,SLOT(propertyModified()));
    connect(samplingRateLineEdit,SIGNAL(textChanged(QString)),this,SLOT(propertyModified()));
    connect(offsetLineEdit,SIGNAL(textChanged(QString)),this,SLOT(propertyModified()));
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
    if(isLostFocus && isReturnPressed || isLostFocus && isCheckAsked)
        return;
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


void AcquisitionSystemPage::setNbChannels(int nb){
    nbChannels = nb;
    nbChannelsLineEdit->setText(QString::number(nb));
}

/**Sets the sampling rate.*/
void AcquisitionSystemPage::setSamplingRate(double rate){
    samplingRateLineEdit->setText(Helper::doubleToString(rate));
}

/**Sets the resolution of the acquisition system.*/
void AcquisitionSystemPage::setResolution(int res){
    switch(res){
    case 12:
        resolutionComboBox->setCurrentIndex(0);
        break;
    case 14:
        resolutionComboBox->setCurrentIndex(1);
        break;
    case 16:
        resolutionComboBox->setCurrentIndex(2);
        break;
    case 32:
        resolutionComboBox->setCurrentIndex(3);
        break;
    default:
        resolutionComboBox->setCurrentIndex(2);
        break;
    }
}

/**Returns the resolution of the acquisition system.*/
int AcquisitionSystemPage::getResolution()const{
    switch(resolutionComboBox->currentIndex()){
    case 0:
        return 12;
    case 1:
        return 14;
    case 2:
        return 16;
    case 3:
        return 32;
    default:
        return 16;
    }
}

void AcquisitionSystemPage::setVoltageRange(int value){
    voltageRangeLineEdit->setText(QString::number(value));
}

/**Sets the amplification of the acquisition system.
*/
void AcquisitionSystemPage::setAmplification(int value){
    amplificationLineEdit->setText(QString::number(value));
}

/**Returns the number of channels.*/
int AcquisitionSystemPage::getNbChannels() const
{
    return nbChannelsLineEdit->text().toInt();
}

/**Returns the sampling rate.*/
double AcquisitionSystemPage::getSamplingRate() const
{
    return samplingRateLineEdit->text().toDouble();
}

int AcquisitionSystemPage::getOffset() const
{
    return offsetLineEdit->text().toInt();
}

/**Returns the voltage range of the acquisition system in volts.
*/
int AcquisitionSystemPage::getVoltageRange() const
{
    return voltageRangeLineEdit->text().toInt();
}

/**Returns the amplification of the acquisition system.
*/
int AcquisitionSystemPage::getAmplification() const
{
    return amplificationLineEdit->text().toInt();
}

/**True if at least one property has been modified, false otherwise.*/
bool AcquisitionSystemPage::isModified()const
{
    return modified;
}

