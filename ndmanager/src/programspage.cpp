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
//Added by qt3to4:
#include <Q3GridLayout>
#include <Q3TextStream>
#include <Q3VBoxLayout>
#include <Q3ValueList>
#include <Q3Frame>

// include files for KDE
#include <kiconloader.h>    // for KIconLoader

#include <ktextedit.h> 



//General C++ include files
#include <iostream>
using namespace std;


ProgramsPage::ProgramsPage(bool expertMode,QWidget *parent, const char *name)
 : Q3Frame(parent, name),expertMode(expertMode){
 
 Q3VBoxLayout* frameLayout = new Q3VBoxLayout(this,0,0);
   
 QString message = "Here you can add a new script description or load an existing one from disk.";
 QLabel* description = new QLabel(message,this);
 description->adjustSize();
 description->setAlignment(Qt::AlignCenter);
 description->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
 frameLayout->addWidget(description);
   
 //Add the buttons
 QWidget* buttons = new QWidget(this); 
 Q3GridLayout* gridLayout = new Q3GridLayout(buttons,1,1,0,6); 
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
 loadButton->setIconSet(QIcon(loader->loadIcon("fileopen", KIcon::Small)));
 
 connect(loadButton,SIGNAL(clicked()),this,SLOT(loadProgram()));
}


ProgramsPage::~ProgramsPage(){}


void ProgramsPage::loadProgram(){

    QStringList programUrls=QFileDialog::getOpenFileNames(this, tr("Select the Script(s) to load..."));
 if(programUrls.size() != 0){
   Q3ValueList<QString>::iterator iterator;
  for(iterator = programUrls.begin();iterator != programUrls.end();++iterator){
   QString programUrl = static_cast<QString>(*iterator);
   QString filePath = programUrl.path();
   QFileInfo fileInfo(filePath);
 
   //Check if the file exists
   if(!fileInfo.exists()){
    KMessageBox::error (this,tr("The  file %1 does not exist.").arg(filePath), tr("Error!"));
    return;
   }         

   //Check if the file is an XML file <=> conains the xml declaration
   QFile file(filePath);
   if(!file.open(QIODevice::ReadOnly)){
    QString message = QString("The file %1 is not readable.").arg(filePath);
    KMessageBox::error (this,tr(message), tr("IO Error!"));
   }
   else{
    Q3TextStream stream(&file);
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
