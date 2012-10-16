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
//Added by qt3to4:
#include <QEvent>
#include <QVector>
#include <QList>




using namespace ndmanager;

SpikePage::SpikePage(QWidget* parent)
    : SpikeLayout(parent),
      isIncorrectRow(false),
      incorrectRow(0),
      incorrectColumn(0),
      modified(false)
{
    //KDAB
    /*

    for(int i = 0;i<groupTable->numCols();++i)
        groupTable->setColumnStretchable(i,true);


    connect(groupTable, SIGNAL(valueChanged(int,int)),this, SLOT(groupChanged(int,int)));
    connect(groupTable, SIGNAL(currentChanged(int,int)),this, SLOT(slotValidate()));
    */

    //install a filter on the groupTable in order to validate the entries
    groupTable->installEventFilter(this);


    connect(groupTable, SIGNAL(currentCellChanged(int,int,int,int)),this, SLOT(slotValidate()));
    connect(addGroupButton,SIGNAL(clicked()),this,SLOT(addGroup()));
    connect(removeGroupButton,SIGNAL(clicked()),this,SLOT(removeGroup()));
    connect(groupTable, SIGNAL(cellPressed(int,int)),this, SLOT(slotValidate()));
    connect(groupTable, SIGNAL(cellClicked(int,int)),this, SLOT(slotValidate()));
    connect(groupTable, SIGNAL(cellDoubleClicked(int,int)),this, SLOT(slotValidate()));

}


SpikePage::~SpikePage(){}


bool SpikePage::eventFilter(QObject* object,QEvent* event){
    QString name = object->objectName();
    if(name.indexOf("groupTable") != -1 && isIncorrectRow){
        groupTable->selectRow(incorrectRow);
        //groupTable->selectColumn(incorrectColumn);
        groupTable->setCurrentCell(incorrectRow,incorrectColumn);
        return true;
    }
    else if(name.indexOf("groupTable") != -1 && event->type() == QEvent::Leave){
        if(groupTable->currentRow() != -1){
            int row = groupTable->currentRow();
            int column = groupTable->currentColumn();
            QWidget* widget = groupTable->cellWidget(row,column);
            if(widget != 0 && widget->metaObject()->className() == ("QLineEdit")){
                groupTable->setCellWidget(row,column,widget);
                return true;
            }
            else return QWidget::eventFilter(object,event);
        }
        else return QWidget::eventFilter(object,event);
    }
    else return QWidget::eventFilter(object,event);
}

void SpikePage::setGroups(const QMap<int, QList<int> >& groups,const QMap<int, QMap<QString,QString> >& information){
    //Clean the groupTable, just in case, before creating empty rows.
    groupTable->clearContents();
    groupTable->setRowCount(groups.count());

    QMap<int,QList<int> >::const_iterator iterator;
    //The iterator gives the keys sorted.
    for(iterator = groups.begin(); iterator != groups.end(); ++iterator){
        QList<int> channelIds = iterator.data();
        QList<int>::iterator channelIterator;

        //create the string containing the channel ids
        QString group;
        for(channelIterator = channelIds.begin(); channelIterator != channelIds.end(); ++channelIterator){
            group.append(QString::number(*channelIterator));
            group.append(" ");
        }

        groupTable->setItem(iterator.key() - 1,0,new QTableWidgetItem(group));


        QMap<QString,QString> groupInformation = information[iterator.key()];
        QMap<QString,QString>::Iterator iterator2;
        //The positions of the information in the table are hard coded (for the moment :0) )
        for(iterator2 = groupInformation.begin(); iterator2 != groupInformation.end(); ++iterator2){
            if(iterator2.key() == NB_SAMPLES){
                groupTable->setItem(iterator.key() - 1,1,new QTableWidgetItem(iterator2.data()));
            }
            if(iterator2.key() == PEAK_SAMPLE_INDEX){
                groupTable->setItem(iterator.key() - 1,2,new QTableWidgetItem(iterator2.data()));
            }
            if(iterator2.key() == NB_FEATURES){
                groupTable->setItem(iterator.key() - 1,3,new QTableWidgetItem(iterator2.data()));
            }
        }
        //groupTable->adjustRow(iterator.key() - 1);
    }//end of groups loop
}



void SpikePage::getGroups(QMap<int, QList<int> >& groups)const{
    int groupId = 1;
    for(int i =0; i<groupTable->rowCount();++i){
        QList<int> channels;
        QString item = groupTable->item(i,0)->text();
        QString channelList = item.simplified();
        if(channelList == " ")
            continue;
        QStringList channelParts = channelList.split(" ", QString::SkipEmptyParts);
        for(uint j = 0;j < channelParts.count(); ++j)
            channels.append(channelParts[j].toInt());
        groups.insert(groupId,channels);
        groupId++;
    }
}

void SpikePage::getGroupInformation(QMap<int,  QMap<QString,QString> >& groupInformation)const{
    int groupId = 1;
    for(int i =0; i<groupTable->rowCount();++i){
        QMap<QString,QString> information;
        const QString item = groupTable->item(i,0)->text();
        QString channelList = item.simplified();
        if(channelList == " ")
            continue;
        //The positions of the information in the table are hard coded
        for(int j = 1;j <= groupTable->columnCount(); ++j){
            QString infoItem = groupTable->item(i,j)->text().simplified();
            if(infoItem == " ")
                continue;
            if(j == 1)
                information.insert(NB_SAMPLES,infoItem);
            else if(j == 2)
                information.insert(PEAK_SAMPLE_INDEX,infoItem);
            else if(j == 3)
                information.insert(NB_FEATURES,infoItem);
        }
        groupInformation.insert(groupId,information);
        groupId++;
    }
}

void SpikePage::removeGroup(){
    #ifdef KDAB_PENDING
    if(isIncorrectRow) return;
    modified = true;
    int nbSelections = groupTable->numSelections();
    if(nbSelections > 0){
        QList< QVector<int> > rowsToRemove;
        //Look up the rows to be removed
        for(int j = 0; j < nbSelections;++j){
            Q3TableSelection selection = groupTable->selection(j);
            bool active = selection.isActive();
            if(active){
                int nbRows = selection.bottomRow() - selection.topRow() + 1;
                QVector<int> rows(nbRows);
                for(int i = 0; i < nbRows;++i){
                    rows[i] = selection.topRow() + i;
                }
                rowsToRemove.append(rows);
            }
        }
        //Actually remove the rows
        QList< QVector<int> >::iterator iterator;
        for(iterator = rowsToRemove.begin(); iterator != rowsToRemove.end(); ++iterator)
            groupTable->removeRows(*iterator);
    }
    emit nbGroupsModified(groupTable->numRows());
#endif
}

void SpikePage::groupChanged(int row,int column){
    #ifdef KDAB_PENDING
    modified = true;
    QString group = groupTable->text(row,column);

    if(isIncorrectRow){
        QWidget* widget = groupTable->cellWidget(incorrectRow,incorrectColumn);
        QString incorrectGroup;
        if(widget != 0 && widget->metaObject()->className() == ("QLineEdit"))
            incorrectGroup = static_cast<QLineEdit*>(widget)->text();
        else if(widget == 0)
            incorrectGroup = groupTable->item(incorrectRow,incorrectColumn)->text();
        if(incorrectGroup.contains(QRegExp("[^\\d\\s]")) != 0){
            groupTable->selectRow(incorrectRow);
            groupTable->setCurrentCell(incorrectRow,incorrectColumn);
            return;
        }
    }

    isIncorrectRow = false;
    incorrectRow = 0;
    incorrectColumn = column;
    groupTable->adjustRow(row);

    //the group entry should only contain digits and whitespaces
    if(group.contains(QRegExp("[^\\d\\s]")) != 0){
        isIncorrectRow = true;
        incorrectRow = row;
        incorrectColumn = column;
        groupTable->selectRow(incorrectRow);
        groupTable->setCurrentCell(incorrectRow,incorrectColumn);
    }
#endif
}

void SpikePage::slotValidate(){
    modified = true;
    if(isIncorrectRow){
        groupTable->selectRow(incorrectRow);
        // groupTable->selectColumn(incorrectColumn);
        groupTable->setCurrentCell(incorrectRow,incorrectColumn);
    }
}

void SpikePage::addGroup(){
    if(isIncorrectRow)
        return;
    modified = true;
    groupTable->insertRow(groupTable->rowCount());
    for(int i = 0;i<groupTable->columnCount();++i){
        groupTable->setItem(groupTable->rowCount()-1,i,new QTableWidgetItem());
        groupTable->update();
    }
    emit nbGroupsModified(groupTable->rowCount());
}


#include "spikepage.moc"
