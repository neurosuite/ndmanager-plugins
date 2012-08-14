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
#include "neuroscopevideopage.h"

//QT includes
#include <qiconset.h>

// include files for KDE
#include <kiconloader.h>
#include <kimageeffect.h>

NeuroscopeVideoPage::NeuroscopeVideoPage(QWidget* parent, const char *name)
 : NeuroscopeVideoLayout(parent, name),height(100),width(100),modified(false),isInit(true){
  
 connect(backgroundButton,SIGNAL(clicked()),this,SLOT(updateBackgroundImage()));
 connect(backgroundLineEdit,SIGNAL(textChanged(const QString&)),this,SLOT(updateBackgroundImage(const QString&)));
 connect(rotateComboBox,SIGNAL(activated(int)),this,SLOT(updateDisplayedImage()));
 connect(filpComboBox,SIGNAL(activated(int)),this,SLOT(updateDisplayedImage()));

 connect(backgroundLineEdit,SIGNAL(textChanged(const QString&)),this,SLOT(propertyModified()));
 connect(rotateComboBox,SIGNAL(activated(int)),this,SLOT(propertyModified()));
 connect(filpComboBox,SIGNAL(activated(int)),this,SLOT(propertyModified()));
 connect(checkBoxBackground,SIGNAL(clicked()),this,SLOT(propertyModified()));

  //Set an icon on the backgroundButton button
 KIconLoader* loader = KGlobal::iconLoader();
 backgroundButton->setIconSet(QIconSet(loader->loadIcon("fileopen", KIcon::Small)));
}


NeuroscopeVideoPage::~NeuroscopeVideoPage(){}

void NeuroscopeVideoPage::updateDisplayedImage(){ 
 if(backgroungImage != NULL){
   //apply first the rotation and then the flip
  QImage rotatedImage = backgroungImage;
  QPixmap pixmap;
  
  int angle = getRotation();
  if(angle != 0){
   KImageEffect::RotateDirection rotationAngle;
   //KDE counts clockwise, to have a counterclock-wise rotation 90 and 270 are inverted
   if(angle == 90) rotationAngle = KImageEffect::Rotate270;
   if(angle == 180) rotationAngle = KImageEffect::Rotate180;
   if(angle == 270) rotationAngle = KImageEffect::Rotate90;
    rotatedImage = KImageEffect::rotate(backgroungImage,rotationAngle);
  }

  QImage flippedImage = rotatedImage;
  // 0 stands for none, 1 for vertical flip and 2 for horizontal flip.
  int flip = getFlip();
  if(flip != 0){
   bool horizontal;
   bool vertical;
   if(flip == 1){
    horizontal = false;
    vertical = true;
   }
   else{
    horizontal = true;
    vertical = false;
   }
   flippedImage = rotatedImage.mirror(horizontal,vertical);
  }
  if(pixmap.convertFromImage(flippedImage)) backgroundPixmap->setPixmap(pixmap); 
 }
}

#include "neuroscopevideopage.moc"
