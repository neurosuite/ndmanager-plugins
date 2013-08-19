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
#include "miscellaneouspage.h"

MiscellaneousPage::MiscellaneousPage(QWidget* parent)
    : NeuroscopeMiscLayout(parent),doubleValidator(this),modified(false),isInit(true){

    //Set a validator on the line edits, the values have to be integers or doubles.
    screenGainLineEdit->setValidator(&doubleValidator);

    connect(traceBackgroundButton,SIGNAL(clicked()),this,SLOT(updateTraceBackgroundImage()));
    connect(traceBackgroundLineEdit,SIGNAL(textChanged(QString)),this,SLOT(updateTraceBackgroundImage(QString)));

    connect(screenGainLineEdit,SIGNAL(textChanged(QString)),this,SLOT(propertyModified()));
    connect(traceBackgroundLineEdit,SIGNAL(textChanged(QString)),this,SLOT(propertyModified()));

    //Set an icon on the backgroundButton button

    traceBackgroundButton->setIcon(QIcon(":/shared-icons/folder-open"));

}

MiscellaneousPage::~MiscellaneousPage(){}


