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
#include "neuroscopevideopage.h"

//QT includes
#include <qicon.h>

#include <QPixmap>

NeuroscopeVideoPage::NeuroscopeVideoPage(QWidget* parent)
    : NeuroscopeVideoLayout(parent),height(100),width(100),modified(false),isInit(true){

    connect(backgroundButton,SIGNAL(clicked()),this,SLOT(updateBackgroundImage()));
    connect(backgroundLineEdit,SIGNAL(textChanged(QString)),this,SLOT(updateBackgroundImage(QString)));
    connect(rotateComboBox,SIGNAL(activated(int)),this,SLOT(updateDisplayedImage()));
    connect(filpComboBox,SIGNAL(activated(int)),this,SLOT(updateDisplayedImage()));

    connect(backgroundLineEdit,SIGNAL(textChanged(QString)),this,SLOT(propertyModified()));
    connect(rotateComboBox,SIGNAL(activated(int)),this,SLOT(propertyModified()));
    connect(filpComboBox,SIGNAL(activated(int)),this,SLOT(propertyModified()));
    connect(checkBoxBackground,SIGNAL(clicked()),this,SLOT(propertyModified()));

    //Set an icon on the backgroundButton button

    backgroundButton->setIcon(QIcon(":/shared-icons/folder-open"));
}


NeuroscopeVideoPage::~NeuroscopeVideoPage(){}

void NeuroscopeVideoPage::updateDisplayedImage(){ 
    if(!backgroungImage.isNull()){
        //apply first the rotation and then the flip
        QImage rotatedImage = backgroungImage;
        QPixmap pixmap;

        int angle = getRotation();
        if(angle != 0){
            QTransform rot;
            //KDE counts clockwise, to have a counterclock-wise rotation 90 and 270 are inverted
            if(angle == 90)
                rot.rotate(90);
            else if(angle == 180)
                rot.rotate(180);
            else if(angle == 270)
                rot.rotate(270);
            rotatedImage = backgroungImage.transformed(rot);
        }

        //Scale
        QImage flippedImage = rotatedImage;
        // 0 stands for none, 1 for vertical flip and 2 for horizontal flip.
        int flip = getFlip();
        if(flip != 0){
            bool horizontal;
            bool vertical;
            if(flip == 1){
                horizontal = false;
                vertical = true;
            } else {
                horizontal = true;
                vertical = false;
            }
            flippedImage = rotatedImage.mirrored(horizontal,vertical);
        }
        if(pixmap.convertFromImage(flippedImage))
            backgroundPixmap2->setPixmap(pixmap);
    }
}

