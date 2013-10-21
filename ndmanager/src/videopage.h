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
#ifndef VIDEOPAGE_H
#define VIDEOPAGE_H

//include files for the application
#include <videolayout.h>
#include <helper.h>

// include files for QT
#include <qwidget.h>
#include <qlineedit.h>
#include <qvalidator.h>




/**
@author Lynn Hazan
*/
class VideoPage : public VideoLayout
{
    Q_OBJECT
public:
    explicit VideoPage(QWidget* parent = 0);
    ~VideoPage();
    
    /**Sets the sampling rate.*/
    void setSamplingRate(double rate){samplingRateLineEdit->setText(Helper::doubleToString(rate));}

    /**Sets the video image width.*/
    void setWidth(int width){widthLineEdit->setText(QString::number(width));}

    /**Sets the video image height.*/
    void setHeight(int height){heightLineEdit->setText(QString::number(height));}

    /**Returns the sampling rate.*/
    double getSamplingRate() const{return samplingRateLineEdit->text().toDouble();}

    /**Returns the video image width.*/
    int getWidth()const{return widthLineEdit->text().toInt();}

    /**Returns the video image height.*/
    int getHeight()const{return heightLineEdit->text().toInt();}

    /**True if at least one property has been modified, false otherwise.*/
    bool isModified()const{return modified;}

    /**Indicates that the initialisation is finished.*/
    void initialisationOver(){isInit = false;}

public slots:

    /** Will be called when any properties is modified.*/
    void propertyModified(){if(!isInit) modified = true;}
    
    /**Resets the internal modification status to false.*/
    void resetModificationStatus(){modified = false;}

private:
    QIntValidator intValidator;
    QDoubleValidator doubleValidator;
    bool modified;
    bool isInit;
};

#endif
