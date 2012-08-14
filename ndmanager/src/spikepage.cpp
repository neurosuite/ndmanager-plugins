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
#include "spikepage.h"
#include "tags.h"

// include files for QT
#include <qlabel.h> 

//General C++ include files
#include <iostream>
using namespace std;
using namespace ndmanager;

SpikePage::SpikePage(QWidget* parent, const char *name)
 : SpikeLayout(parent, name),isIncorrectRow(false),incorrectRow(0),incorrectColumn(0),modified(false){
   
 for(int i = 0;i<groupTable->numCols();++i) groupTable->setColumnStretchable(i,true);
 
 //install a filter on the groupTable in order to validate the entries
 groupTable->installEventFilter(this);
 
 connect(addGroupButton,SIGNAL(clicked()),this,SLOT(addGroup()));
 connect(removeGroupButton,SIGNAL(clicked()),this,SLOT(removeGroup()));
 connect(groupTable, SIGNAL(valueChanged(int,int)),this, SLOT(groupChanged(int,int))); 
 connect(groupTable, SIGNAL(currentChanged(int,int)),this, SLOT(slotValidate()));
 connect(groupTable, SIGNAL(pressed(int,int,int,const QPoint&)),this, SLOT(slotValidate()));
 connect(groupTable, SIGNAL(clicked(int,int,int,const QPoint&)),this, SLOT(slotValidate()));
 connect(groupTable, SIGNAL(doubleClicked(int,int,int,const QPoint&)),this, SLOT(slotValidate()));
}


SpikePage::~SpikePage(){}


bool SpikePage::eventFilter(QObject* object,QEvent* event){
 QString name = object->name();
 if(name.find("groupTable") != -1 && isIncorrectRow){
  groupTable->selectRow(incorrectRow);
  //groupTable->selectColumn(incorrectColumn);
  groupTable->setCurrentCell(incorrectRow,incorrectColumn);
  return true;
 }
 else if(name.find("groupTable") != -1 && event->type() == QEvent::Leave){
  if(groupTable->currentRow() != -1){
   int row = groupTable->currentRow();
   int column = groupTable->currentColumn();
   QWidget* widget = groupTable->cellWidget(row,column);
   if(widget != 0 && widget->isA("QLineEdit")){
    QTableItem* item = groupTable->item(row,column);
     if(item == NULL){
     item = new QTableItem(groupTable,QTableItem::WhenCurrent,static_cast<QLineEdit*>(widget)->text());
     item->setWordWrap(true);
     groupTable->setItem(row,column,item);
    }
    else item->setContentFromEditor(widget);
    return true;
   }
   else return QWidget::eventFilter(object,event); 
  } 
  else return QWidget::eventFilter(object,event); 
 } 
 else return QWidget::eventFilter(object,event); 
}

void SpikePage::setGroups(const QMap<int, QValueList<int> >& groups,const QMap<int, QMap<QString,QString> >& information){
 //Clean the groupTable, just in case, before creating empty rows.
 for(int i =0; i<groupTable->numRows();++i) groupTable->removeRow(i);
 groupTable->setNumRows(groups.count());
  
 QMap<int,QValueList<int> >::const_iterator iterator;
 //The iterator gives the keys sorted.
 for(iterator = groups.begin(); iterator != groups.end(); ++iterator){
  QValueList<int> channelIds = iterator.data();
  QValueList<int>::iterator channelIterator;
  
  //create the string containing the channel ids
  QString group;
  for(channelIterator = channelIds.begin(); channelIterator != channelIds.end(); ++channelIterator){
   group.append(QString("%1").arg(*channelIterator));
   group.append(" ");
  }
    
  QTableItem* item = new QTableItem(groupTable,QTableItem::WhenCurrent,group);
  item->setWordWrap(true);
  groupTable->setItem(iterator.key() - 1,0,item);

  QMap<QString,QString> groupInformation = information[iterator.key()];  
  QMap<QString,QString>::Iterator iterator2;
  //The positions of the information in the table are hard coded (for the moment :0) )
  for(iterator2 = groupInformation.begin(); iterator2 != groupInformation.end(); ++iterator2){
   if(iterator2.key() == NB_SAMPLES){
    QTableItem* item = new QTableItem(groupTable,QTableItem::WhenCurrent,iterator2.data());
    item->setWordWrap(true);
    groupTable->setItem(iterator.key() - 1,1,item);
   } 
   if(iterator2.key() == PEAK_SAMPLE_INDEX){
    QTableItem* item = new QTableItem(groupTable,QTableItem::WhenCurrent,iterator2.data());
    item->setWordWrap(true);
    groupTable->setItem(iterator.key() - 1,2,item);    
   } 
   if(iterator2.key() == NB_FEATURES){
    QTableItem* item = new QTableItem(groupTable,QTableItem::WhenCurrent,iterator2.data());
    item->setWordWrap(true);
    groupTable->setItem(iterator.key() - 1,3,item);
   } 
  }
  groupTable->adjustRow(iterator.key() - 1);
 }//end of groups loop
}



void SpikePage::getGroups(QMap<int, QValueList<int> >& groups)const{
 
 int groupId = 1;
 for(int i =0; i<groupTable->numRows();++i){
  QValueList<int> channels;
  QString item = groupTable->text(i,0);
  QString channelList = item.simplifyWhiteSpace();
  if(channelList == " ") continue;
  QStringList channelParts = QStringList::split(" ", channelList);
  for(uint j = 0;j < channelParts.count(); ++j)
   channels.append(channelParts[j].toInt());
  groups.insert(groupId,channels);
  groupId++;
 }
}

void SpikePage::getGroupInformation(QMap<int,  QMap<QString,QString> >& groupInformation)const{
 
 int groupId = 1;
 for(int i =0; i<groupTable->numRows();++i){
  QMap<QString,QString> information;
  QString item = groupTable->text(i,0);
  QString channelList = item.simplifyWhiteSpace();
  if(channelList == " ") continue;
   //The positions of the information in the table are hard coded
  for(int j = 1;j <= groupTable->numCols(); ++j){
   QString infoItem = groupTable->text(i,j).simplifyWhiteSpace();
   if(infoItem == " ") continue;
   if(j == 1)information.insert(NB_SAMPLES,infoItem);
   else if(j == 2)information.insert(PEAK_SAMPLE_INDEX,infoItem);
   else if(j == 3)information.insert(NB_FEATURES,infoItem);
  }
  groupInformation.insert(groupId,information);
  groupId++;
 }
}

void SpikePage::removeGroup(){
  if(isIncorrectRow) return;
  modified = true;
  int nbSelections = groupTable->numSelections();
  if(nbSelections > 0){
   QValueList< QMemArray<int> > rowsToRemove;
   //Look up the rows to be removed
   for(int j = 0; j < nbSelections;++j){
    QTableSelection selection = groupTable->selection(j);
    bool active = selection.isActive();
    if(active){
     int nbRows = selection.bottomRow() - selection.topRow() + 1;
     QMemArray<int> rows(nbRows);
     for(int i = 0; i < nbRows;++i){
       rows[i] = selection.topRow() + i;
     }
     rowsToRemove.append(rows);
    }
   }
   //Actually remove the rows
   QValueList< QMemArray<int> >::iterator iterator;
   for(iterator = rowsToRemove.begin(); iterator != rowsToRemove.end(); ++iterator) groupTable->removeRows(*iterator);
  }  
  emit nbGroupsModified(groupTable->numRows());
 }

#include "spikepage.moc"
