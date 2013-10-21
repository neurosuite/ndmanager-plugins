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
#ifndef ACQUISITIONSYSTEMPAGE_H
#define ACQUISITIONSYSTEMPAGE_H

//include files for the application
#include <acquisitionsystemlayout.h>
#include <helper.h>

// include files for QT
#include <qwidget.h>
#include <qcombobox.h>
#include <qspinbox.h>
#include <qpushbutton.h>
#include <qvalidator.h>

/**
@author Lynn Hazan
*/
class AcquisitionSystemPage : public AcquisitionSystemLayout
{
    Q_OBJECT
public:
    explicit AcquisitionSystemPage(QWidget* parent = 0);

    ~AcquisitionSystemPage();
    
    /**Sets the number of channels.*/
    void setNbChannels(int nb);
    /**Sets the sampling rate.*/
    void setSamplingRate(double rate);

    /**Sets the resolution of the acquisition system.*/
    void setResolution(int res);

    /**Sets the initial offset for all the field potentials.*/
    void setOffset(int offset){offsetLineEdit->setText(QString::number(offset));}

    /**Sets the voltage range of the acquisition system in milivolts.
  */
    void setVoltageRange(int value);

    /**Sets the amplification of the acquisition system.
  */
    void setAmplification(int value);

    /**Returns the number of channels.*/
    int getNbChannels() const;

    /**Returns the sampling rate.*/
    double getSamplingRate() const;

    /**Returns the resolution of the acquisition system.*/
    int getResolution()const;
    /**Returns the initial offset for all the field potentials.*/
    int getOffset() const;

    /**Returns the voltage range of the acquisition system in volts.
  */
    int getVoltageRange() const;

    /**Returns the amplification of the acquisition system.
  */
    int getAmplification() const;

    /**True if at least one property has been modified, false otherwise.*/
    bool isModified()const;

    /**Checks if the number of channels has changed.*/
    void checkNbChannels();

public slots:
    /**This function is called when the user presses the return key in the <i>Number of Channels</i> field.*/
    void nbChannelsLineEditReturnPressed();

    /**This function is called when the <i>Number of Channels</i> field lose focus.*/
    void nbChannelsLineEditLostFocus();

    /** Will be called when any properties except the number of channels has been modified.*/
    void propertyModified() {if(!isInit) modified = true;}

    /**Indicates that the initialisation is finished.*/
    void initialisationOver() {isInit = false;}
    
    /**Resets the internal modification status to false.*/
    void resetModificationStatus() { modified = false;}

signals:
    /*Signals that the number of channels has been modified.
   * @param nbChannels the neew number of channels
   */
    void nbChannelsModified(int nbChannels);

private:

    /**This function is called when the user changes the number of channels. It warns him of the consequences and triggers the implied changes.*/
    void nbChannelsChanged();
    
    QDoubleValidator doubleValidator;
    QIntValidator intValidator;
    bool isInit;
    float nbChannels;
    bool reset;
    bool isLostFocus;
    bool isReturnPressed;
    bool modified;
    bool isCheckAsked;
};

#endif
