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
#include "programspage.h"

// include files for QT
#include <qlayout.h>
#include <qpushbutton.h> 
#include <qlabel.h>
#include <qregexp.h> 

// include files for KDE
#include <klocale.h>        // for tr()
#include <kiconloader.h>    // for KIconLoader
#include <kfiledialog.h>
#include <ktextedit.h> 
#include <kurl.h> 
#include <kmessagebox.h>

//General C++ include files
#include <iostream>
using namespace std;


ProgramsPage::ProgramsPage(bool expertMode,QWidget *parent, const char *name)
 : QFrame(parent, name),expertMode(expertMode){
 
 QVBoxLayout* frameLayout = new QVBoxLayout(this,0,0);
   
 QString message = "Here you can add a new script description or load an existing one from disk.";
 QLabel* description = new QLabel(message,this);
 description->adjustSize();
 description->setAlignment(Qt::AlignCenter);
 description->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
 frameLayout->addWidget(description);
   
 //Add the buttons
 QWidget* buttons = new QWidget(this); 
 QGridLayout* gridLayout = new QGridLayout(buttons,1,1,0,6); 
 frameLayout->addWidget(buttons); 
 
 if(expertMode){
  addButton = new QPushButton(tr("Add"),buttons);
  addButton->setSizePolicy(QSizePolicy((QSizePolicy::SizeType)0,(QSizePolicy::SizeType)0,0,0,addButton->sizePolicy().hasHeightForWidth()));
  addButton->setMinimumSize(QSize(104,0));
  addButton->setMaximumSize(QSize(104,32767));
  gridLayout->addWidget(addButton,0,1); 
 
  loadButton = new QPushButton(tr("Load..."),buttons);
  loadButton->setSizePolicy(QSizePolicy((QSizePolicy::SizeType)0,(QSizePolicy::SizeType)0,0,0,loadButton->sizePolicy().hasHeightForWidth()));
  loadButton->setMinimumSize(QSize(104,0));
  loadButton->setMaximumSize(QSize(104,32767));
  gridLayout->addWidget(loadButton,0,3);
  
  QSpacerItem* space1 = new QSpacerItem(112,16,QSizePolicy::Expanding,QSizePolicy::Minimum);
  gridLayout->addItem(space1,0,0);
  QSpacerItem* space2 = new QSpacerItem(112,16,QSizePolicy::Expanding,QSizePolicy::Minimum);
  gridLayout->addItem(space2,0,4);
  QSpacerItem* space3 = new QSpacerItem(29,16,QSizePolicy::Fixed,QSizePolicy::Minimum);
  gridLayout->addItem(space3,0,2);
  
  connect(addButton,SIGNAL(clicked()),this,SLOT(addProgram()));
 }
 else{
  loadButton = new QPushButton(tr("Load..."),buttons);
  loadButton->setSizePolicy(QSizePolicy((QSizePolicy::SizeType)0,(QSizePolicy::SizeType)0,0,0,loadButton->sizePolicy().hasHeightForWidth()));
  loadButton->setMinimumSize(QSize(104,0));
  loadButton->setMaximumSize(QSize(104,32767));
  gridLayout->addWidget(loadButton,0,1);
  
  QSpacerItem* space1 = new QSpacerItem(112,16,QSizePolicy::Expanding,QSizePolicy::Minimum);
  gridLayout->addItem(space1,0,0);
  QSpacerItem* space2 = new QSpacerItem(112,16,QSizePolicy::Expanding,QSizePolicy::Minimum);
  gridLayout->addItem(space2,0,2);
 }
 
 //Set an icon on the load button
 KIconLoader* loader = KGlobal::iconLoader();
 loadButton->setIconSet(QIconSet(loader->loadIcon("fileopen", KIcon::Small)));
 
 connect(loadButton,SIGNAL(clicked()),this,SLOT(loadProgram()));
}


ProgramsPage::~ProgramsPage(){}


void ProgramsPage::loadProgram(){

 KURL::List programUrls=KFileDialog::getOpenURLs(QString::null,
       QString::null, this, tr("Select the Script(s) to load..."));
 if(programUrls.size() != 0){
   QValueList<KURL>::iterator iterator;
  for(iterator = programUrls.begin();iterator != programUrls.end();++iterator){
   KURL programUrl = static_cast<KURL>(*iterator);
   QString filePath = programUrl.path();
   QFileInfo fileInfo(filePath);
 
   //Check if the file exists
   if(!fileInfo.exists()){
    KMessageBox::error (this,tr("The  file %1 does not exist.").arg(filePath), tr("Error!"));
    return;
   }         

   //Check if the file is an XML file <=> conains the xml declaration
   QFile file(filePath);
   if(!file.open(IO_ReadOnly)){
    QString message = QString("The file %1 is not readable.").arg(filePath);
    KMessageBox::error (this,tr(message), tr("IO Error!"));
   }
   else{
    QTextStream stream(&file);
    QString firstLine = stream.readLine();
    int i = firstLine.find(QRegExp("^<\\?xml version"));
    file.close();
    if(i == -1){
    QString message = QString("The file %1 is not an xml file.").arg(filePath);
    KMessageBox::error (this,tr(message), tr("IO Error!"));
    return;
    }        
   }               
   if(!programUrl.isEmpty())emit programToLoad(programUrl);
  }
 }
}


#include "programspage.moc"
