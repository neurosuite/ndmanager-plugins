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
#include "filespage.h"

// include files for QT
#include <qlayout.h>
#include <qtabwidget.h> 
#include <qpushbutton.h> 
//Added by qt3to4:
#include <Q3GridLayout>
#include <Q3VBoxLayout>
#include <Q3ValueList>
#include <QFrame>
#include <Q3PtrList>

//General C++ include files
#include <iostream>
using namespace std;

FilesPage::FilesPage(QWidget* parent, const char *name)
 : QFrame(parent, name){
 
 Q3VBoxLayout* frameLayout = new Q3VBoxLayout(this,0,0);
 tabWidget = new QTabWidget(this);
 frameLayout->addWidget(tabWidget);
 
 //Add the buttons
 QWidget* buttons = new QWidget(this); 
 Q3GridLayout* gridLayout = new Q3GridLayout(buttons,1,1,0,6); 
 frameLayout->addWidget(buttons); 
 
 addButton = new QPushButton(tr("Add File"),buttons);
 addButton->setSizePolicy(QSizePolicy((QSizePolicy::SizeType)0,(QSizePolicy::SizeType)0,0,0,addButton->sizePolicy().hasHeightForWidth()));
 addButton->setMinimumSize(QSize(124,0));
 addButton->setMaximumSize(QSize(124,32767));
 gridLayout->addWidget(addButton,0,1);
 
 removeButton = new QPushButton(tr("Remove File"),buttons);
 removeButton->setSizePolicy(QSizePolicy((QSizePolicy::SizeType)0,(QSizePolicy::SizeType)0,0,0,removeButton->sizePolicy().hasHeightForWidth()));
 removeButton->setMinimumSize(QSize(124,0));
 removeButton->setMaximumSize(QSize(124,32767));
 gridLayout->addWidget(removeButton,0,3);
 
 QSpacerItem* space1 = new QSpacerItem(112,16,QSizePolicy::Expanding,QSizePolicy::Minimum);
 gridLayout->addItem(space1,0,0);
 QSpacerItem* space2 = new QSpacerItem(112,16,QSizePolicy::Expanding,QSizePolicy::Minimum);
 gridLayout->addItem(space2,0,4);
 QSpacerItem* space3 = new QSpacerItem(29,16,QSizePolicy::Fixed,QSizePolicy::Minimum);
 gridLayout->addItem(space3,0,2);

 connect(addButton,SIGNAL(clicked()),this,SLOT(addNewFile()));
 connect(removeButton,SIGNAL(clicked()),this,SLOT(removeFile())); 
}

FilesPage::~FilesPage(){}

void FilesPage::addNewFile(){
 FilePage* filePage = new FilePage();
 tabWidget->addTab(filePage,tr("New File"));
 
 //set the connection
 connect(filePage,SIGNAL(extensionChanged(const QString&,FilePage*)),this,SLOT(changeCaption(const QString&,FilePage*)));
 filePage->initialisationOver();

 //make sure to show the content of the new page
 tabWidget->showPage(filePage);
}

FilePage* FilesPage::addFile(QString title){
 FilePage* filePage = new FilePage();
 tabWidget->addTab(filePage,title);
 
 //set the connection
 connect(filePage,SIGNAL(extensionChanged(const QString&,FilePage*)),this,SLOT(changeCaption(const QString&,FilePage*)));

 return filePage;
}


void FilesPage::removeFile(){
 FilePage* filePage = static_cast<FilePage*>(tabWidget->currentPage());
 tabWidget->removePage(filePage);
  delete filePage;
  emit fileModification(getFileExtensions());
}

void FilesPage::changeCaption(const QString& caption,FilePage* filePage){
 tabWidget->setTabLabel(filePage,caption);
 emit fileModification(getFileExtensions());
}

void FilesPage::getFilePages(Q3PtrList<FilePage>& fileList){
 for(int i = 0; i<tabWidget->count();++i) fileList.append(static_cast<FilePage*>(tabWidget->page(i)));
}

bool FilesPage::isModified()const{
 bool modified = false;
 for(int i = 0; i<tabWidget->count();++i){
  modified = static_cast<FilePage*>(tabWidget->page(i))->isModified();
  if(modified) break;
 }
 
 return modified;
}

void FilesPage::resetModificationStatus(){
 for(int i = 0; i<tabWidget->count();++i){
  static_cast<FilePage*>(tabWidget->page(i))->resetModificationStatus();
 }
}

Q3ValueList<QString> FilesPage::getFileExtensions(){
 Q3ValueList<QString> extensionList;

 for(int i = 0; i<tabWidget->count();++i) extensionList.append(static_cast<FilePage*>(tabWidget->page(i))->getExtension());


 return extensionList;
}

#include "filespage.moc"
