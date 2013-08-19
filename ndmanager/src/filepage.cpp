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
#include "filepage.h"

// include files for QT
#include <qpushbutton.h>

#include <QEvent>
#include <QVector>
#include <QList>
#include <QTableWidgetItem>

FilePage::FilePage(QWidget *parent)
    : FileLayout(parent),
      isIncorrectRow(false),
      modified(false),
      isInit(true)
{
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    mappingTable->horizontalHeader()->setResizeMode( QHeaderView::ResizeToContents );
#else
    mappingTable->horizontalHeader()->setSectionResizeMode( QHeaderView::ResizeToContents );
#endif
    connect(extensionLineEdit,SIGNAL(returnPressed()),this,SLOT(changeCaption()));
    connect(extensionLineEdit,SIGNAL(editingFinished()),this,SLOT(changeCaption()));

    connect(addChannelButton,SIGNAL(clicked()),this,SLOT(addChannel()));
    connect(removeChannelButton,SIGNAL(clicked()),this,SLOT(removeChannel()));

    connect(extensionLineEdit,SIGNAL(textChanged(QString)),this,SLOT(propertyModified()));
    connect(samplingRateLineEdit,SIGNAL(textChanged(QString)),this,SLOT(propertyModified()));

    //install a filter on the mappingTable in order to validate the entries
    mappingTable->installEventFilter(this);

    //For validation
    connect(mappingTable, SIGNAL(cellChanged(int,int)),this, SLOT(mappingChanged(int,int)));
    connect(mappingTable, SIGNAL(cellPressed(int,int)),this, SLOT(slotValidate()));
    connect(mappingTable, SIGNAL(cellClicked(int,int)),this, SLOT(slotValidate()));
    connect(mappingTable, SIGNAL(cellDoubleClicked(int,int)),this, SLOT(slotValidate()));

}


FilePage::~FilePage(){
    extensionLineEdit->clearFocus();
    samplingRateLineEdit->clearFocus();
}

bool FilePage::eventFilter(QObject* object,QEvent* event){
    const QString name = object->objectName();
    if(name.indexOf("mappingTable") != -1 && isIncorrectRow){
        mappingTable->editItem(mappingTable->item(incorrectRow,0));
        return true;
    }
    else if(name.indexOf("mappingTable") != -1 && event->type() == QEvent::Leave){
        if(mappingTable->currentRow() != -1){
            int row = mappingTable->currentRow();
            int column = mappingTable->currentColumn();
            QWidget* widget = mappingTable->cellWidget(row,column);
            if(widget != 0 && widget->metaObject()->className() == ("QLineEdit")){
                mappingTable->editItem(mappingTable->item(row,column));
                return true;
            }
            else return QWidget::eventFilter(object,event);
        }
        else return QWidget::eventFilter(object,event);
    }
    else
        return QWidget::eventFilter(object,event);
}

void FilePage::addChannel(){
    if(isIncorrectRow)
        return;
    modified = true;

    mappingTable->setRowCount(mappingTable->rowCount()+1);
    mappingTable->setItem(mappingTable->rowCount()-1,0,new QTableWidgetItem());
    mappingTable->setVerticalHeaderItem(mappingTable->rowCount()-1,new QTableWidgetItem(QString::number(mappingTable->rowCount()-1)));
}

void FilePage::removeChannel(){
    if(isIncorrectRow)
        return;
    modified = true;

    QList<QTableWidgetSelectionRange> range = mappingTable->selectedRanges ();
    if(!range.isEmpty()) {
        QList<int> rowsToRemove;
        Q_FOREACH(const QTableWidgetSelectionRange& selection, range) {
            int nbRows = selection.bottomRow() - selection.topRow() + 1;
            for(int i = 0; i < nbRows;++i){
                rowsToRemove.append(selection.topRow() + i);
            }
        }
        qSort(rowsToRemove);
        //Actually remove the rows
        for(int i = rowsToRemove.count()-1; i>=0;--i) {
            mappingTable->removeRow(rowsToRemove.at(i));
        }

    }
}

void FilePage::setChannelMapping(const QMap<int, QList<int> >& channels){
    mappingTable->clearContents();
    mappingTable->setRowCount(channels.count());

    QMap<int,QList<int> >::const_iterator iterator;
    //The iterator gives the keys sorted.
    for(iterator = channels.begin(); iterator != channels.end(); ++iterator){
        QList<int> channelIds = iterator.value();
        QList<int>::iterator channelIterator;

        //create the string containing the channel ids
        QString newChannel;
        for(channelIterator = channelIds.begin(); channelIterator != channelIds.end(); ++channelIterator){
            newChannel.append(QString::number(*channelIterator));
            newChannel.append(" ");
        }

        mappingTable->setItem(iterator.key() - 1,0,new QTableWidgetItem(newChannel));

        mappingTable->resizeColumnToContents(iterator.key() - 1);
    }//end of groups loop
}

QMap<int, QList<int> > FilePage::getChannelMapping()const{
    QMap<int, QList<int> > channelMapping;

    int channelId = 1;
    for(int i =0; i<mappingTable->rowCount();++i){
        QList<int> channels;
        const QString item = mappingTable->item(i,0)->text();
        const QString channelList = item.simplified();
        if(channelList == " " || channelList.isEmpty())
            continue;

        QStringList channelParts = channelList.split(" ", QString::SkipEmptyParts);
        for(uint j = 0;j < channelParts.count(); ++j)
            channels.append(channelParts.at(j).toInt());
        channelMapping.insert(channelId,channels);
        channelId++;
    }

    return channelMapping;
}

void FilePage::slotValidate(){
    modified = true;
    if(isIncorrectRow){
        mappingTable->selectRow(incorrectRow);
        mappingTable->editItem(mappingTable->item(incorrectRow,0));
    }
}

/**Validates the current entry in the mapping table.*/
void FilePage::mappingChanged(int row,int column){
    modified = true;
    QString channel = mappingTable->item(row,column)->text();
    //the group entry should only contain digits and whitespaces
    if(channel.contains(QRegExp("[^\\d\\s]")) != 0){
        isIncorrectRow = true;
        incorrectRow = row;
        mappingTable->selectRow(row);
    }
    else{
        if(isIncorrectRow){
            QString incorrectMapping = mappingTable->item(incorrectRow,0)->text();
            if(incorrectMapping.contains(QRegExp("[^\\d\\s]")) != 0)
                return;
        }
        isIncorrectRow = false;
        mappingTable->resizeColumnToContents(column);
    }
}


void FilePage::changeCaption(){
    if(extensionLineEdit->text() != extension){
        extension = extensionLineEdit->text();
        emit extensionChanged(extension,this);
    }
}

