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
#include "channelcolorspage.h"

// include files for QT
#include <qwidget.h>
#include <QList>
#include <qcolor.h>
#include <qcolor.h>
#include <qcolordialog.h>
#include <QMouseEvent>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QDebug>

ChannelColorsPage::ChannelColorsPage(QWidget* parent)
    : ChannelColorsLayout(parent)
    ,modified(false)
{
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    colorTable->horizontalHeader()->setResizeMode( QHeaderView::ResizeToContents );
#else
    colorTable->horizontalHeader()->setSectionResizeMode( QHeaderView::ResizeToContents );
#endif
    connect(colorTable, SIGNAL(cellChanged(int,int)),this, SLOT(propertyModified()));
    connect(colorTable, SIGNAL(cellDoubleClicked(int,int)),this, SLOT(propertyModified()));
    connect(colorTable, SIGNAL(modified()),SLOT(propertyModified()));
}

ChannelColorsPage::~ChannelColorsPage(){}

void ChannelColorsPage::getColors(QList<ChannelColors>& colors){
    for(int i = 0; i < colorTable->rowCount();++i) {
        ChannelColors channelColors;
        channelColors.setId(i);
        channelColors.setColor(colorTable->item(i,0)->text());
        channelColors.setGroupColor(colorTable->item(i,1)->text());
        channelColors.setSpikeGroupColor(colorTable->item(i,2)->text());
        colors.append(channelColors);
    }
}

void ChannelColorsPage::setColors(const QList<ChannelColors>& colors){
    QList<ChannelColors>::ConstIterator iterator;
    for(iterator = colors.constBegin(); iterator != colors.constEnd(); ++iterator){
        const int id = (*iterator).getId();
        colorTable->setItem(id,0,new QTableWidgetItem((*iterator).getColor().name()));
        colorTable->setItem(id,1,new QTableWidgetItem((*iterator).getGroupColor().name()));
        colorTable->setItem(id,2,new QTableWidgetItem((*iterator).getSpikeGroupColor().name()));
    }
}

void ChannelColorsPage::setNbChannels(int nbChannels){
    for(int i =0; i<colorTable->rowCount();++i)
        colorTable->removeRow(i);
    colorTable->setRowCount(nbChannels);
}


