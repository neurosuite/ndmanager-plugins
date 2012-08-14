/***************************************************************************
 *   Copyright (C) 2004 by Lynn Hazan                                      *
 *   lynn.hazan@myrealbox.com                                              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                 void addAttributes(QMap<QString QMap<int,QString> > attributes);  *
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
#include "anatomypage.h"

// include files for QT
#include <qvaluelist.h>
#include <qstringlist.h>

AnatomyPage::AnatomyPage(QWidget* parent, const char *name)
 : AnatomyLayout(parent, name),nbChannels(0),isIncorrectRow(false),incorrectRow(0),modified(false){
  
 for(int i = 0;i<attributesTable->numCols();++i) attributesTable->setColumnStretchable(i,true);
 for(int i = 0;i<groupTable->numCols();++i) groupTable->setColumnStretchable(i,true);
 
 //install a filter on the groupTable in order to validate the entries
 groupTable->installEventFilter(this);
 
 connect(addGroupButton,SIGNAL(clicked()),this,SLOT(addGroup()));
 connect(removeGroupButton,SIGNAL(clicked()),this,SLOT(removeGroup()));
 connect(groupTable, SIGNAL(currentChanged(int,int)),this, SLOT(slotValidate()));
 connect(groupTable, SIGNAL(valueChanged(int,int)),this, SLOT(groupChanged(int,int)));
 connect(groupTable, SIGNAL(pressed(int,int,int,const QPoint&)),this, SLOT(slotValidate()));
 connect(groupTable, SIGNAL(clicked(int,int,int,const QPoint&)),this,SLOT(slotValidate()));
 connect(groupTable, SIGNAL(doubleClicked(int,int,int,const QPoint&)),this,SLOT(slotValidate()));
  
 connect(attributesTable, SIGNAL(valueChanged(int,int)),this, SLOT(attributeChanged(int,int)));
}

AnatomyPage::~AnatomyPage(){}

bool AnatomyPage::eventFilter(QObject* object,QEvent* event){
 QString name = object->name();
  
 if(name.find("groupTable") != -1 && isIncorrectRow){
  groupTable->selectRow(incorrectRow);
  groupTable->setCurrentCell(incorrectRow,0);
  return true;
 }
 else if(name.find("groupTable") != -1 && event->type() == QEvent::Leave){
  if(groupTable->currentRow() != -1){
   int row = groupTable->currentRow();
   int column = groupTable->currentColumn();
   QWidget* widget = groupTable->cellWidget(row,column);
   if(widget != 0 && widget->isA("QLineEdit")){
    QTableItem* item = groupTable->item(row,column);
    item->setContentFromEditor(widget);
    return true;
   }
   else return QWidget::eventFilter(object,event); 
  } 
  else return QWidget::eventFilter(object,event); 
 }
 else return QWidget::eventFilter(object,event); 
}

void AnatomyPage::setAttributes(const QMap<QString, QMap<int,QString> >& attributes){
 QHeader* header = attributesTable->horizontalHeader();
 for(int i =0; i<attributesTable->numCols();++i){
  QMap<int,QString> values = attributes[header->label(i)];//the headers have been set in the ui file and the corresponding entries in the map in the xmlreader
  //insert the values in the table and set the line headers
  for(int j = 0;j<nbChannels;++j){
  
   QTableItem* item = new QTableItem(attributesTable,QTableItem::WhenCurrent,values[j]);
   item->setWordWrap(true);
   attributesTable->setItem(j,i,item);
   attributesTable->verticalHeader()->setLabel(j,QString("%1").arg(j));
  } 
  attributesTable->adjustColumn(i);
 }
}

void AnatomyPage::getAttributes(QMap<QString, QMap<int,QString> >& attributesMap)const{
  
 QHeader* header = attributesTable->horizontalHeader();
 for(int i =0; i<attributesTable->numCols();++i){
  QMap<int,QString> values;
  for(int j = 0;j<nbChannels;++j){  
   QString attribut = attributesTable->text(j,i).simplifyWhiteSpace();   
   if(attribut != " ") values.insert(j,attribut);
  } 
  attributesMap.insert(header->label(i),values);
 }
}

void AnatomyPage::setGroups(const QMap<int, QValueList<int> >& groups){
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
  groupTable->adjustRow(iterator.key() - 1);
 }//end of groups loop
}

void AnatomyPage::getGroups(QMap<int, QValueList<int> >& groups)const{
 
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

 void AnatomyPage::removeGroup(){
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
 }


#include "anatomypage.moc"
