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
#ifndef MISCELLANEOUSPAGE_H
#define MISCELLANEOUSPAGE_H

//include files for the application
#include <neuroscopemisclayout.h>

// include files for QT
#include <qwidget.h>
#include <qlineedit.h>
#include <qvalidator.h>
#include <qpushbutton.h>

// include files for KDE
#include <kfiledialog.h>


/**
@author Lynn Hazan
*/
class MiscellaneousPage : public NeuroscopeMiscLayout
{
Q_OBJECT
public:
    MiscellaneousPage(QWidget* parent = 0, const char *name = 0);
    ~MiscellaneousPage();

    
  /**Sets the screen gain in milivolts by centimeters used to display the field potentiels..
  */
  inline void setScreenGain(float gain){
   screenGainLineEdit->setText(QString("%1").arg(gain));
  };

  /**Sets the background image used for the trace view.*/
  inline void setTraceBackgroundImage(QString image){  
   traceBackgroundLineEdit->setText(image);
  };
  
 /**Returns the screen gain in milivolts by centimeters used to display the field potentiels.
  */
  inline float getScreenGain() const{
   return screenGainLineEdit->text().toFloat();
  };
  
  /**Returns the background image used for the trace view.*/
  inline QString getTraceBackgroundImage()const{return traceBackgroundLineEdit->text();}


  /**True if at least one property has been modified, false otherwise.*/
  inline bool isModified()const{return modified;}
  
  /**Indicates that the initialisation is finished.*/
  inline void initialisationOver(){isInit = false;}
    
public slots:
 
 /** Will be called when any properties is modified.*/
 inline void propertyModified(){if(!isInit) modified = true;}
  
 /**Resets the internal modification status to false.*/
 inline void resetModificationStatus(){modified = false;}
 
private slots:
  inline void updateTraceBackgroundImage(){
      QString image = KFileDialog::getOpenFileName(QString(),
                                                   QString(), this, tr("Select the background image..."));

   setTraceBackgroundImage(image);
  };

  inline void updateTraceBackgroundImage(const QString& image){
   setTraceBackgroundImage(image);
  };
   
 private:
  QDoubleValidator doubleValidator;  
  bool modified;
  bool isInit;
};

#endif
