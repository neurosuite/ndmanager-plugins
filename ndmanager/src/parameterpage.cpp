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
#include "parameterpage.h"

// include files for QT
#include <qwidget.h>
#include <q3table.h>
#include <qpushbutton.h>
#include <qlineedit.h>
//Added by qt3to4:
#include <QEvent>
#include <Q3MemArray>
#include <QList>

ParameterPage::ParameterPage(bool expertMode,QWidget *parent, const char *name)
 : ParameterLayout(parent),valueModified(false),descriptionModified(false){
 
 status<<tr("Mandatory")<<tr("Optional")<<tr("Dynamic");
  
 for(int i = 0;i<parameterTable->numCols();++i) parameterTable->setColumnStretchable(i,true);

 ddList.append(2);
 
 //If the export mode is not set, only the value column is editable
 if(!expertMode){
  parameterTable->setColumnReadOnly(0,true);
  parameterTable->setColumnReadOnly(2,true); 
  
  addButton->setEnabled(false);
  removeButton->setEnabled(false);
  nameLineEdit->setReadOnly(true);
 }
 else{
  connect(addButton,SIGNAL(clicked()),this,SLOT(addParameter())); 
  connect(removeButton,SIGNAL(clicked()),this,SLOT(removeParameter())); 
  connect(nameLineEdit,SIGNAL(returnPressed()),this,SLOT(changeCaption()));
  connect(nameLineEdit,SIGNAL(lostFocus()),this,SLOT(changeCaption()));
 }  
 
  //install a filter on the groupTable in order to validate the entries
 parameterTable->installEventFilter(this);

 
 connect(parameterTable, SIGNAL(valueChanged(int,int)),this, SLOT(propertyModified(int,int)));//does not seem to work (see the hack with the eventfiler)
}


ParameterPage::~ParameterPage(){}

bool ParameterPage::eventFilter(QObject* object,QEvent* event){
 QString name = object->name();
 
 //hack, if the event is KeyRelease this means that there was a modification 
 if(name.find("parameterTable") != -1 && event->type() == QEvent::KeyRelease){
  if(parameterTable->currentColumn() == 1) valueModified = true;
  else descriptionModified = true;

  return true;
 }
 else return QWidget::eventFilter(object,event); 
}

void ParameterPage::propertyModified(int, int column){
 if(column == 1) valueModified = true;
 else descriptionModified = true;
}

QMap<int, QList<QString> > ParameterPage::getParameterInformation(){
 QMap<int, QList<QString> > parameterInformation;
  
 int paramNb = 1;
 for(int i =0; i<parameterTable->numRows();++i){
  QList<QString> information;
  QString item = parameterTable->text(i,0);
  QString name = item.simplified();
  if(name == " ") continue;
  information.append(name);
  for(int j = 1;j < parameterTable->numCols(); ++j){
   Q3TableItem* item = parameterTable->item(i,j);
   QString text;
   if(ddList.contains(j)) text = static_cast<Q3ComboTableItem*>(item)->currentText();
   else text = parameterTable->text(i,j);
   information.append(text.simplified());
  }
   
  parameterInformation.insert(paramNb,information);
  paramNb++;
 }

 return parameterInformation;
}

void ParameterPage::setParameterInformation(QMap<int, QList<QString> >& parameters){
 //Clean the parameterTable, just in case, before creating empty rows.
 for(int i =0; i<parameterTable->numRows();++i) parameterTable->removeRow(i);
 parameterTable->setNumRows(parameters.count());
 
 QMap<int,QList<QString> >::Iterator iterator;
 //The iterator gives the keys sorted.
 for(iterator = parameters.begin(); iterator != parameters.end(); ++iterator){
  QList<QString> parameterInfo = iterator.data();
   
  for(uint i=0;i<parameterInfo.count();++i){ 
   if(ddList.contains(i)){
    Q3ComboTableItem* comboStatus = new Q3ComboTableItem(parameterTable,status);
    comboStatus->setCurrentItem(parameterInfo[i]);
    parameterTable->setItem(iterator.key(),i,comboStatus);
   }
   else{
    Q3TableItem* item = new Q3TableItem(parameterTable,Q3TableItem::OnTyping,parameterInfo[i]);
    item->setWordWrap(true);
    parameterTable->setItem(iterator.key(),i,item); 
   }
   
   parameterTable->adjustColumn(i); 
  }
 }//end of parameters loop
}


void ParameterPage::addParameter(){
 descriptionModified = true;
 parameterTable->insertRows(parameterTable->numRows());
 
 //Use of the the 3 parameter constructor to be qt 3.1 compatible 
 Q3TableItem* name = new Q3TableItem(parameterTable,Q3TableItem::OnTyping,"");//WhenCurrent
 name->setWordWrap(true);
 parameterTable->setItem(parameterTable->numRows() - 1,0,name); 
 
 //Use of the the 3 parameter constructor to be qt 3.1 compatible 
 Q3TableItem* value = new Q3TableItem(parameterTable,Q3TableItem::OnTyping,"");
 value->setWordWrap(true);
 parameterTable->setItem(parameterTable->numRows() - 1,1,value); 
 
 //Add the comboxItem in the status column
 Q3ComboTableItem* comboStatus = new Q3ComboTableItem(parameterTable,status);
 parameterTable->setItem(parameterTable->numRows() - 1,2,comboStatus);
}

void ParameterPage::removeParameter(){
 descriptionModified = true;
 int nbSelections = parameterTable->numSelections();
 if(nbSelections > 0){
  QList< Q3MemArray<int> > rowsToRemove;
  //Look up the rows to be removed
  for(int j = 0; j < nbSelections;++j){
   Q3TableSelection selection = parameterTable->selection(j);
   bool active = selection.isActive();
   if(active){
    int nbRows = selection.bottomRow() - selection.topRow() + 1;
    Q3MemArray<int> rows(nbRows);
    for(int i = 0; i < nbRows;++i){
      rows[i] = selection.topRow() + i;
    }
    rowsToRemove.append(rows);
   }
  }
  //Actually remove the rows
  QList< Q3MemArray<int> >::iterator iterator;
  for(iterator = rowsToRemove.begin(); iterator != rowsToRemove.end(); ++iterator) parameterTable->removeRows(*iterator);
 }  
}



#include "parameterpage.moc"
