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
#include "videopage.h"

VideoPage::VideoPage(QWidget* parent, const char *name)
 : VideoLayout(parent, name),intValidator(this),doubleValidator(this),modified(false),isInit(true){
 
 //Set a validator on the line edits, the values have to be integers.
 samplingRateLineEdit->setValidator(&doubleValidator);
 widthLineEdit->setValidator(&intValidator);
 heightLineEdit->setValidator(&intValidator);

 connect(widthLineEdit,SIGNAL(textChanged(const QString&)),this,SLOT(propertyModified()));
 connect(heightLineEdit,SIGNAL(textChanged(const QString&)),this,SLOT(propertyModified()));
 connect(samplingRateLineEdit,SIGNAL(textChanged(const QString&)),this,SLOT(propertyModified()));

}


VideoPage::~VideoPage()
{
}


#include "videopage.moc"
