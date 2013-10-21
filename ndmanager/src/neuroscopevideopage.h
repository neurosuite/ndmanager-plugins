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
#include <qcheckbox.h> 
#include <QFileDialog>

/**
@author Lynn Hazan
*/
class NeuroscopeVideoPage : public NeuroscopeVideoLayout
{
    Q_OBJECT
public:
    explicit NeuroscopeVideoPage(QWidget* parent = 0);
    ~NeuroscopeVideoPage();

    /**Sets the background image.*/
    void setBackgroundImage(const QString &image){
        backgroundLineEdit->setText(image);
        if(!image.isEmpty()){
            backgroungImage.load(image);
            if(!backgroungImage.isNull()){
                //flip and rotation values should have been set before any call to this function.
                updateDisplayedImage();
            }
        } else {
            QPixmap pixmap;
            pixmap = pixmap.copy(0, 0, width,height);
            pixmap.fill(Qt::black);
            backgroundPixmap2->setPixmap(pixmap);
        }
    }

    /**All the positions contained in a position file can be used to create a background image for the PositionView.
  * This function sets if such background has to be created.
  */
    void setPositionsBackground(bool draw){checkBoxBackground->setChecked(draw);}

    /**Sets the video image rotation angle.*/
    void setRotation(int angle){
        switch(angle){
        case 0:
            rotateComboBox->setCurrentIndex(0);
            break;
        case 90:
            rotateComboBox->setCurrentIndex(1);
            break;
        case 180:
            rotateComboBox->setCurrentIndex(2);
            break;
        case 270:
            rotateComboBox->setCurrentIndex(3);
            break;
        default:
            rotateComboBox->setCurrentIndex(0);
            break;
        }
    }

    /**Sets the video image flip orientation.
  * 0 stands for none, 1 for vertical and 2 for horizontal.
  */
    void setFlip(int orientation){
        switch(orientation){
        case 0:
            filpComboBox->setCurrentIndex(0);
            break;
        case 1:
            filpComboBox->setCurrentIndex(1);
            break;
        case 2:
            filpComboBox->setCurrentIndex(2);
            break;
        default:
            filpComboBox->setCurrentIndex(0);
            break;
        }
    }

    /**Returns the background image.*/
    inline QString getBackgroundImage()const{return backgroundLineEdit->text();}

    /**All the positions contained in a position file can be used to create a background image for the PositionView.
  * The value return by this function tells if such background has to be created.
  * @return true if the all the positions contain in the position file have to be drawn on the background, false otherwise.
  */
    inline bool getPositionsBackground()const{return checkBoxBackground->isChecked();}

    
    /**Returns the video image rotation angle.*/
    inline int getRotation()const{
        switch(rotateComboBox->currentIndex()){
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
    }

    /**Returns the video image flip orientation.
  * 0 stands for none, 1 for vertical and 2 for horizontal.
  */
    inline int getFlip()const{
        switch(filpComboBox->currentIndex()){
        case 0:
            return 0;
        case 1:
            return 1;
        case 2:
            return 2;
        default:
            return 0;
        }
    }

    /**True if at least one property has been modified, false otherwise.*/
    inline bool isModified()const{return modified;}

    /**Indicates that the initialisation is finished.*/
    void initialisationOver(){isInit = false;}

private slots:
    void updateBackgroundImage(){
        QString image = QFileDialog::getOpenFileName(this, tr("Select the background image..."));

        if(!image.isEmpty())
            setBackgroundImage(image);
    }

    void updateBackgroundImage(const QString& image){
        if(!image.isEmpty())
            setBackgroundImage(image);
        else{
            QPixmap pixmap;
            pixmap = pixmap.copy(0, 0, width,height);
            pixmap.fill(Qt::black);
            backgroundPixmap2->setPixmap(pixmap);
        }
    }

    void updateDisplayedImage();

    /** Will be called when any properties.*/
    void propertyModified(){if(!isInit) modified = true;}

    /**Resets the internal modification status to false.*/
    void resetModificationStatus(){modified = false;}

private:
    QImage backgroungImage;
    int height;
    int width;
    bool modified;
    bool isInit;
};

#endif
