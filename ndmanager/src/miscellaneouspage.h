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
#ifndef MISCELLANEOUSPAGE_H
#define MISCELLANEOUSPAGE_H

//include files for the application
#include <neuroscopemisclayout.h>

// include files for QT
#include <qwidget.h>
#include <qlineedit.h>
#include <qvalidator.h>
#include <qpushbutton.h>
#include <QFileDialog>

/**
@author Lynn Hazan
*/
class MiscellaneousPage : public NeuroscopeMiscLayout
{
    Q_OBJECT
public:
    explicit MiscellaneousPage(QWidget* parent = 0);
    ~MiscellaneousPage();

    
    /**Sets the screen gain in milivolts by centimeters used to display the field potentiels..
  */
    void setScreenGain(float gain){
        screenGainLineEdit->setText(QString::number(gain));
    }

    /**Sets the background image used for the trace view.*/
    void setTraceBackgroundImage(const QString& image){
        traceBackgroundLineEdit->setText(image);
    }

    /**Returns the screen gain in milivolts by centimeters used to display the field potentiels.
  */
    float getScreenGain() const{
        return screenGainLineEdit->text().toFloat();
    }

    /**Returns the background image used for the trace view.*/
    QString getTraceBackgroundImage()const{return traceBackgroundLineEdit->text();}


    /**True if at least one property has been modified, false otherwise.*/
    bool isModified()const{return modified;}

    /**Indicates that the initialisation is finished.*/
    void initialisationOver(){isInit = false;}
    
public slots:

    /** Will be called when any properties is modified.*/
    void propertyModified(){if(!isInit) modified = true;}

    /**Resets the internal modification status to false.*/
    void resetModificationStatus(){modified = false;}

private slots:
    void updateTraceBackgroundImage(){
        const QString image = QFileDialog::getOpenFileName(this, tr("Select the background image..."));
        if( !image.isEmpty() )
          setTraceBackgroundImage(image);
    }

    void updateTraceBackgroundImage(const QString& image){
        setTraceBackgroundImage(image);
    }

private:
    QDoubleValidator doubleValidator;
    bool modified;
    bool isInit;
};

#endif
