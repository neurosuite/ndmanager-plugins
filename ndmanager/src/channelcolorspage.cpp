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
#include "channelcolorspage.h"

// include files for QT
#include <qwidget.h>
#include <qvaluelist.h>
#include <qcolor.h>
#include <qcolor.h>
#include <qcolordialog.h>

//General C++ include files
#include <iostream>
using namespace std;



ChannelColorsPage::ChannelColorsPage(QWidget* parent, const char *name)
 : ChannelColorsLayout(parent, name),nbChannels(0),modified(false){
  for(int i = 0;i<colorTable->numCols();++i) colorTable->setColumnStretchable(i,true);
  
  connect(colorTable, SIGNAL(pressed(int,int,int,const QPoint&)),this, SLOT(chooseColor(int,int,int)));
  connect(colorTable, SIGNAL(valueChanged(int,int)),this, SLOT(propertyModified()));
  connect(colorTable, SIGNAL(doubleClicked(int,int,int,const QPoint&)),this, SLOT(propertyModified()));
}

ChannelColorsPage::~ChannelColorsPage(){}

void ChannelColorsPage::getColors(QValueList<ChannelColors>& colors){
 for(int i =0; i<nbChannels;++i){
  ChannelColors channelColors;
  channelColors.setId(i);
  channelColors.setColor(colorTable->text(i,0));
  channelColors.setGroupColor(colorTable->text(i,1));
  channelColors.setSpikeGroupColor(colorTable->text(i,2));
  colors.append(channelColors);
 }
}
 
 void ChannelColorsPage::setColors(QValueList<ChannelColors>& colors){  
  QValueList<ChannelColors>::iterator iterator;
  for(iterator = colors.begin(); iterator != colors.end(); ++iterator){
   int id = (*iterator).getId();
   colorTable->verticalHeader()->setLabel(id,QString("%1").arg(id));   
   QTableItem* itemColor = new QTableItem(colorTable,QTableItem::OnTyping,(*iterator).getColor().name());
   itemColor->setWordWrap(true);
   colorTable->setItem(id,0,itemColor);

   QTableItem* itemGroupColor = new QTableItem(colorTable,QTableItem::OnTyping,(*iterator).getGroupColor().name());
   itemGroupColor->setWordWrap(true);
   colorTable->setItem(id,1,itemGroupColor);

   QTableItem* itemSpikeGroupColor = new QTableItem(colorTable,QTableItem::OnTyping,(*iterator).getSpikeGroupColor().name());
   itemSpikeGroupColor->setWordWrap(true);
   colorTable->setItem(id,2,itemSpikeGroupColor);
  }
 }
    
void ChannelColorsPage::chooseColor(int row,int column,int button){
 if(button == Qt::MidButton){
  //Get the color associated with the item
  QColor color(colorTable->text(row,column));
 
  QColor result = QColorDialog::getColor(color,0);
  if (result.isValid()) {
   colorTable->setText(row,column,result.name());
   modified = true;
  }
 }
} 
    
#include "channelcolorspage.moc"
