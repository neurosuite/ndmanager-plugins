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
#include <QList>
#include <qcolor.h>
#include <qcolor.h>
#include <qcolordialog.h>
#include <QMouseEvent>
#include <QTableWidget>
#include <QTableWidgetItem>

ChannelColorsPage::ChannelColorsPage(QWidget* parent)
    : ChannelColorsLayout(parent)
    ,modified(false)
{
    colorTable->horizontalHeader()->setResizeMode( QHeaderView::ResizeToContents );

    connect(colorTable, SIGNAL(cellChanged(int,int)),this, SLOT(propertyModified()));
    connect(colorTable, SIGNAL(cellDoubleClicked(int,int)),this, SLOT(propertyModified()));
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

void ChannelColorsPage::mousePressEvent ( QMouseEvent * event )
{
    //KDAB: fix me
    if(event->button() == Qt::MidButton) {
        QTableWidgetItem *item = colorTable->itemAt(event->pos());
        if(item) {
            const QColor color(item->text());

            const QColor result = QColorDialog::getColor(color,0);
            if (result.isValid()) {
                item->setText(result.name());
                modified = true;
            }
        }
    }
}

void ChannelColorsPage::setNbChannels(int nbChannels){
    for(int i =0; i<colorTable->rowCount();++i)
        colorTable->removeRow(i);
    colorTable->setRowCount(nbChannels);
}


