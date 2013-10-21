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
#ifndef CHANNELCOLORS_H
#define CHANNELCOLORS_H

//include files for the application

// include files for QT
#include <qwidget.h>
#include <qstring.h>
#include <qcolor.h>

/**
  *Class storing the channel color information.
  *@author Lynn Hazan
  */

class ChannelColors {
public:

    /*Constructor*/
    ChannelColors(){
        id = 0;
        color = QColor(Qt::black);
    }

    /**Constructor
  * @param id chanel id.
  * @param color channel color.
  */
    ChannelColors(int id,const QString& color):id(id){
        setColor(color);
    }

    ~ChannelColors(){}

    /**Sets the channel id.
  * @param channelId channel id.
  */
    void setId(int channelId){id = channelId;}

    /**Sets the color used to display the channel.
  * @param colorName name of the color in the format "#RRGGBB".
  */
    void setColor(const QString& colorName){color = QColor(colorName);}

    /**Sets the group color (anatomical group) for the channel.
  * @param colorName name of the color in the format "#RRGGBB".
  */
    void setGroupColor(const QString& colorName){groupColor = QColor(colorName);}

    /**Sets the spike group color for the channel.
  * @param colorName name of the color in the format "#RRGGBB".
  */
    void setSpikeGroupColor(const QString& colorName){spikeGroupColor = QColor(colorName);}

    /**Gets the channel id.
  * @return channel id.
  */
    int getId() const{return id;}

    /**Gets the color used to display the channel.
  * @return name of the color in the format "#RRGGBB".
  */
    QColor getColor() const{return color;}

    /**Gets the group color (anatomical group) for the channel.
  * @return name of the color in the format "#RRGGBB".
  */
    QColor getGroupColor() const{return groupColor;}

    /**Gets the spike group color for the channel.
  * @return name of the color in the format "#RRGGBB".
  */
    QColor getSpikeGroupColor() const{return spikeGroupColor;}
    
private:
    /**Channel id*/
    int id;

    /**Color used to display the channel.*/
    QColor color;

    /**Group color (anatomical group) for the channel.*/
    QColor groupColor;

    /**Spike group color for the channel.*/
    QColor spikeGroupColor;
};

#endif
