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
#include "channeloffsetspage.h"

// include files for QT
#include <qwidget.h>
#include <QList>

// include files for KDE

//General C++ include files
#include <iostream>
using namespace std;

ChannelOffsetsPage::ChannelOffsetsPage(QWidget* parent, const char* name)
 : ChannelOffsetsLayout(parent),nbChannels(0),modified(false){
  for(int i = 0;i<offsetTable->numCols();++i) offsetTable->setColumnStretchable(i,true);
  
  connect(offsetTable, SIGNAL(valueChanged(int,int)),this, SLOT(propertyModified()));
  connect(offsetTable, SIGNAL(doubleClicked(int,int,int,const QPoint&)),this, SLOT(propertyModified()));
 
}

ChannelOffsetsPage::~ChannelOffsetsPage(){}

void ChannelOffsetsPage::getOffsets(QMap<int,int>& offsets){
 for(int i =0; i<nbChannels;++i){
  offsets.insert(i,offsetTable->text(i,0).toInt());
 }
}
 
 void ChannelOffsetsPage::setOffsets(QMap<int,int>& offsets){  
  QMap<int,int>::Iterator iterator;
  //The iterator gives the keys sorted.
  for(iterator = offsets.begin(); iterator != offsets.end(); ++iterator){
   int channelId = iterator.key();
   offsetTable->verticalHeader()->setLabel(channelId,QString("%1").arg(channelId));   
   Q3TableItem* item = new Q3TableItem(offsetTable,Q3TableItem::OnTyping,QString("%1").arg(iterator.data()));
   item->setWordWrap(true);
   offsetTable->setItem(channelId,0,item);
  }
 }
    

#include "channeloffsetspage.moc"
