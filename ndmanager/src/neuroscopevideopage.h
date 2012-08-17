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
#ifndef NEUROSCOPEVIDEOPAGE_H
#define NEUROSCOPEVIDEOPAGE_H

//include files for the application
#include <neuroscopevideolayout.h>

// include files for QT
#include <qwidget.h>
#include <qvalidator.h>
#include <qcombobox.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qpixmap.h>
#include <qimage.h>
#include <q3groupbox.h>
#include <qcheckbox.h> 
#include <QFileDialog>

// include files for KDE


//General C++ include files
#include <iostream>
using namespace std;


/**
@author Lynn Hazan
*/
class NeuroscopeVideoPage : public NeuroscopeVideoLayout
{
Q_OBJECT
public:
    NeuroscopeVideoPage(QWidget* parent = 0, const char *name = 0);
    ~NeuroscopeVideoPage();
  
  /**Sets the background image.*/
  inline void setBackgroundImage(QString image){  
   backgroundLineEdit->setText(image);
   if(image != ""){
    backgroungImage.load(image);
    if(backgroungImage != NULL){
     //flip and rotation values should have been set before any call to this function.
     updateDisplayedImage();
    }
   }
   else{
    QPixmap pixmap;
    pixmap.resize(width,height);
    pixmap.fill(black);
    backgroundPixmap->setPixmap(pixmap); 
   }
  };
  
  /**All the positions contained in a position file can be used to create a background image for the PositionView.
  * This function sets if such background has to be created.
  */  
  inline void setPositionsBackground(bool draw){checkBoxBackground->setChecked(draw);};

  /**Sets the video image rotation angle.*/
  inline void setRotation(int angle){
   switch(angle){
      case 0:
        rotateComboBox->setCurrentItem(0);
        break;
      case 90:
        rotateComboBox->setCurrentItem(1);
        break;
      case 180:
        rotateComboBox->setCurrentItem(2);
        break;
      case 270:
        rotateComboBox->setCurrentItem(3);
        break;
      default:
        rotateComboBox->setCurrentItem(0);
        break;
   }
  };

  /**Sets the video image flip orientation.
  * 0 stands for none, 1 for vertical and 2 for horizontal.
  */
  inline void setFlip(int orientation){
   switch(orientation){
      case 0:
        filpComboBox->setCurrentItem(0);
        break;
      case 1:
        filpComboBox->setCurrentItem(1);
        break;
      case 2:
        filpComboBox->setCurrentItem(2);
        break;
      default:
        filpComboBox->setCurrentItem(0);
        break;
   }
  };
  
  /**Returns the background image.*/
  inline QString getBackgroundImage()const{return backgroundLineEdit->text();};

  /**All the positions contained in a position file can be used to create a background image for the PositionView.
  * The value return by this function tells if such background has to be created.
  * @return true if the all the positions contain in the position file have to be drawn on the background, false otherwise.
  */  
  inline bool getPositionsBackground()const{return checkBoxBackground->isChecked();};

    
  /**Returns the video image rotation angle.*/
  inline int getRotation()const{
   switch(rotateComboBox->currentItem()){
      case 0:
        return 0;
      case 1:
        return 90;
      case 2:
        return 180;
      case 3:
        return 270;
      default:
        return 0;
   }
  };

  /**Returns the video image flip orientation.
  * 0 stands for none, 1 for vertical and 2 for horizontal.
  */
  inline int getFlip()const{
   switch(filpComboBox->currentItem()){
      case 0:
        return 0;
      case 1:
        return 1;
      case 2:
        return 2;
      default:
        return 0;
   }
  };

 /**True if at least one property has been modified, false otherwise.*/
 inline bool isModified()const{return modified;}

 /**Indicates that the initialisation is finished.*/
 inline void initialisationOver(){isInit = false;}
 
private slots:
  inline void updateBackgroundImage(){
      QString image = QFileDialog::getOpenFileName(this, tr("Select the background image..."));

   if(image != "")
       setBackgroundImage(image);
  }

  inline void updateBackgroundImage(const QString& image){
   if(image != "")  setBackgroundImage(image);
   else{
    QPixmap pixmap;
    pixmap.resize(width,height);
    pixmap.fill(black);
    backgroundPixmap->setPixmap(pixmap);
   }
  };
  
  void updateDisplayedImage();
  
 /** Will be called when any properties.*/
 inline void propertyModified(){if(!isInit) modified = true;}
   
 /**Resets the internal modification status to false.*/
 inline void resetModificationStatus(){modified = false;}
 
 private:
  QImage backgroungImage;  
  int height;
  int width;
  bool modified;
  bool isInit;
};

#endif
