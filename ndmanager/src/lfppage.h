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
#ifndef LFPPAGE_H
#define LFPPAGE_H

//include files for the application
#include <lfplayout.h>
#include <helper.h>

// include files for QT
#include <qwidget.h>
#include <qlineedit.h>
#include <qvalidator.h>



/**
@author Lynn Hazan
*/
class LfpPage : public LfpLayout
{
    Q_OBJECT
public:
    explicit LfpPage(QWidget* parent = 0);
    ~LfpPage();

    /**Sets the sampling rate.*/
    void setSamplingRate(double rate){samplingRateLineEdit->setText(Helper::doubleToString(rate));}

    /**Returns the sampling rate.*/
    double getSamplingRate() const{return samplingRateLineEdit->text().toDouble();}

    /**True if at least one property has been modified, false otherwise.*/
    bool isModified() const {return modified;}

    /**Indicates that the initialisation is finished.*/
    void initialisationOver(){isInit = false;}
    
public slots:

    /** Will be called when any properties is modified.*/
    void propertyModified(){if(!isInit) modified = true;}

    /**Resets the internal modification status to false.*/
    void resetModificationStatus(){modified = false;}

private:
    QDoubleValidator doubleValidator;
    bool modified;
    bool isInit;
};

#endif
