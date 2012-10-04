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
#ifndef CHANNELOFFSETSPAGE_H
#define CHANNELOFFSETSPAGE_H

//include files for the application
#include <channeloffsetslayout.h>

// include files for QT
#include <qwidget.h>

/**
@author Lynn Hazan
*/
class ChannelOffsetsPage : public ChannelOffsetsLayout
{
    Q_OBJECT
public:
    ChannelOffsetsPage(QWidget* parent = 0);
    ~ChannelOffsetsPage();

    /**Sets the number of channels, creating the correspondent number of lines in the table.
 * @param nbChannels number of channels.
 */
    void setNbChannels(int nbChannels){
        this->nbChannels = nbChannels;
        for(int i =0; i<offsetTable->rowCount();++i)
            offsetTable->removeRow(i);
        offsetTable->setRowCount(nbChannels);
    }
    
    /** Returns the offsets associated with the channels.
 * @param offsets map given the channel default offsetsr.
 */
    void getOffsets(QMap<int,int>& offsets);

    /**Sets the offsets associated with the channels.
 * @param offsets map given the channel default offsets.
 */
    void setOffsets(QMap<int,int>& offsets);

    /**True if at least one property has been modified, false otherwise.*/
    bool isModified()const{return modified;}

public slots: 
    /** Will be called when any properties is modified.*/
    void propertyModified(){modified = true;}

    /**Resets the internal modification status to false.*/
    void resetModificationStatus(){modified = false;}

private:
    int nbChannels;
    bool modified;
};

#endif
