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
#ifndef ANATOMYPAGE_H
#define ANATOMYPAGE_H

//include files for the application
#include <anatomylayout.h>

// include files for QT
#include <qwidget.h>
#include <qpushbutton.h>
#include <qmap.h>
#include <qregexp.h>
#include <q3header.h> 
#include <qlineedit.h>
//Added by qt3to4:
#include <QEvent>
#include <QList>




//General C++ include files




/**
* Class used to represent the information linked to the anatomical groups.
* @author Lynn Hazan
*/
class AnatomyPage : public AnatomyLayout
{
Q_OBJECT
public:
  /**Constructor.*/
  AnatomyPage(QWidget* parent = 0);
 /**Destructor.*/
 ~AnatomyPage();

 /**Initializes the attribute table, sitting the values of each attribute for each channel.
 * @param attributes reference to the map given the correspondance between the attribute names and a map given for each channel the value of the attribute.
 */
 void setAttributes(const QMap<QString, QMap<int,QString> >& attributes); 
 
 /** Returns a map given the values of each attribute for each channel.
 * @param attributesMap map given the correspondance between the attribute names and a map given for each channel the value of the attribute.
 */
 void getAttributes(QMap<QString, QMap<int,QString> >& attributesMap)const;
 
 /** Initializes the group table.
 * @param groups map containing the list of channels for each group except the trash group.
 */
 void setGroups(const QMap<int, QList<int> >& groups);
 
 /**Returns the composition of the anatomical groups.
 * @param groups map containing the list of channels for each group except trash group.
 */ 
 void getGroups(QMap<int, QList<int> >& groups)const;
 
 /**Initializes the number of channels used.
 * @param nbChannels number of channels.
 */
 void setNbChannels(int nbChannels){
  this->nbChannels = nbChannels;
  for(int i =0; i<attributesTable->numRows();++i) attributesTable->removeRow(i);
  attributesTable->setNumRows(nbChannels);
 }
 
  /**True if at least one property has been modified, false otherwise.*/
 inline bool isModified()const{return modified;}

protected:
 /** Event filter to validate the entries in the group table.
 * @param object target object for the event.
 * @param event event sent.
 */
 bool eventFilter(QObject* object,QEvent* event);
 
public slots:
 /**Adds a new line to the group table.*/
 void addGroup(){  
  if(isIncorrectRow) return;
  modified = true;
  groupTable->insertRows(groupTable->numRows());
  //Use of the the 3 parameter constructor to be qt 3.1 compatible
  Q3TableItem* item = new Q3TableItem(groupTable,Q3TableItem::WhenCurrent,"");
  item->setWordWrap(true);
  groupTable->setItem(groupTable->numRows() - 1,0,item);
 }
 
 /**Removes the selected lines from the group table.*/
 void removeGroup();
 
 /**Validates the current entry in the group table.*/
 void slotValidate(){ 
  modified = true;
  if(isIncorrectRow){ 
   groupTable->selectRow(incorrectRow);
   groupTable->setCurrentCell(incorrectRow,0);
  }
 }
 
  /**Validates the current entry in the group table.*/
 void groupChanged(int row,int column){
  modified = true;
  QString group = groupTable->text(row,column); 

  if(isIncorrectRow){
    QWidget* widget = groupTable->cellWidget(incorrectRow,0);
    QString incorrectGroup;
    if(widget != 0 && widget->metaObject()->className() == ("QLineEdit")) incorrectGroup = static_cast<QLineEdit*>(widget)->text();
    else if(widget == 0) incorrectGroup = groupTable->item(incorrectRow,0)->text();
    if(incorrectGroup.contains(QRegExp("[^\\d\\s]")) != 0){
     groupTable->selectRow(incorrectRow);
     groupTable->setCurrentCell(incorrectRow,0); 
     return;
    } 
   }
   
   isIncorrectRow = false;
   incorrectRow = 0;
   groupTable->adjustRow(row);   
   
  //the group entry should only contain digits and whitespaces
  if(group.contains(QRegExp("[^\\d\\s]")) != 0){
   isIncorrectRow = true;
   incorrectRow = row;
   groupTable->selectRow(incorrectRow);
   groupTable->setCurrentCell(incorrectRow,0);
  }   
 }
 
  /**Validates the current entry in the attribute table.*/
 void attributeChanged(int row,int column){
  modified = true;
  //hard coded that the skip attribut is an int, later it can be made more dynamic
  Q3Header* header = attributesTable->horizontalHeader();
  QString attributName = header->label(column);
  if(attributName == "Skip"){
   bool ok;
   QString attribut = attributesTable->text(row,column);
   attribut.toInt(&ok);
   if(ok) attributesTable->adjustRow(row);
   else {
    attributesTable->selectRow(row);
    attributesTable->setCurrentCell(row,column);
   }
  }
  else attributesTable->adjustRow(row);
 }
 
  /** Will be called when any properties is modified.*/
  void propertyModified(){modified = true;}

 /**Resets the internal modification status to false.*/
 void resetModificationStatus(){modified = false;}
   
private:
 int nbChannels; 
 bool isIncorrectRow;
 int incorrectRow;
 bool modified;
};



#endif
