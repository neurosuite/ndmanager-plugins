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
#include "channeloffsetspage.h"

// include files for QT
#include <qwidget.h>
#include <QList>

ChannelOffsetsPage::ChannelOffsetsPage(QWidget* parent)
    : ChannelOffsetsLayout(parent),nbChannels(0),modified(false)
{
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    offsetTable->horizontalHeader()->setResizeMode( QHeaderView::ResizeToContents );
#else
    offsetTable->horizontalHeader()->setSectionResizeMode( QHeaderView::ResizeToContents );
#endif
    connect(offsetTable, SIGNAL(itemChanged(QTableWidgetItem*)),this, SLOT(propertyModified()));
    connect(offsetTable, SIGNAL(itemDoubleClicked(QTableWidgetItem*)),this, SLOT(propertyModified()));

}

ChannelOffsetsPage::~ChannelOffsetsPage(){}

void ChannelOffsetsPage::getOffsets(QMap<int,int>& offsets){
    for(int i =0; i<nbChannels;++i){
        offsets.insert(i,offsetTable->item(i,0)->text().toInt());
    }
}

void ChannelOffsetsPage::setOffsets(const QMap<int,int>& offsets){
    QMap<int,int>::ConstIterator iterator;
    QStringList lst;
    //The iterator gives the keys sorted.
    for(iterator = offsets.constBegin(); iterator != offsets.constEnd(); ++iterator){
        int channelId = iterator.key();
        lst<<(channelId,QString::number(channelId));
        QTableWidgetItem *item = new QTableWidgetItem(QString::number(iterator.value()));
        offsetTable->setItem(channelId,0,item);
    }
    offsetTable->setVerticalHeaderLabels(lst);
}



void ChannelOffsetsPage::setNbChannels(int nbChannels){
    this->nbChannels = nbChannels;

    for(int i =0; i<offsetTable->rowCount();++i)
        offsetTable->removeRow(i);

    offsetTable->setRowCount(nbChannels);
}
