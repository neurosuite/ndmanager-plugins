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
#ifndef CHANNELCOLORSPAGE_H
#define CHANNELCOLORSPAGE_H

//include files for the application
#include "channelcolorslayout.h"
#include "channelcolors.h"

// include files for QT
#include <QList>


/**
@author Lynn Hazan
*/
class ChannelColorsPage : public ChannelColorsLayout
{
    Q_OBJECT
public:
    explicit ChannelColorsPage(QWidget* parent = 0);
    ~ChannelColorsPage();

    /**Sets the number of channels, creating the correspondent number of lines in the table.
 * @param nbChannels number of channels.
 */
    void setNbChannels(int nbChannels);
    
    /** Returns the colors associated with the channels.
 * @param colors list of ChannelColors given the 3 color used by each channel.
 */
    void getColors(QList<ChannelColors>& colors);

    /** Sets the colors associated with the channels.
 * @param colors list of ChannelColors given the 3 color used by each channel.
 */
    void setColors(const QList<ChannelColors> &colors);

    /**True if at least one property has been modified, false otherwise.*/
    bool isModified()const{return modified;}
    void setModified( bool b ) { modified = b; }

public slots:
    /** Will be called when any properties is modified.*/
    void propertyModified(){modified = true;}

    /**Resets the internal modification status to false.*/
    void resetModificationStatus(){modified = false;}
private:
    bool modified;
};

#endif
