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
#ifndef CHANNELCOLORSPAGE_H
#define CHANNELCOLORSPAGE_H

//include files for the application
#include <channelcolorslayout.h>
#include "channelcolors.h"

// include files for QT
#include <qwidget.h>
#include <q3table.h>
//Added by qt3to4:
#include <Q3ValueList>

// include files for KDE
/**
@author Lynn Hazan
*/
class ChannelColorsPage : public ChannelColorsLayout
{
Q_OBJECT
public:
  ChannelColorsPage(QWidget* parent = 0, const char *name = 0);
  ~ChannelColorsPage();

 /**Sets the number of channels, creating the correspondent number of lines in the table.
 * @param nbChannels number of channels.
 */  
  inline void setNbChannels(int nbChannels){
   this->nbChannels = nbChannels;
   for(int i =0; i<colorTable->numRows();++i) colorTable->removeRow(i);
   colorTable->setNumRows(nbChannels);
 };
    
 /** Returns the colors associated with the channels.
 * @param colors list of ChannelColors given the 3 color used by each channel.
 */
 void getColors(Q3ValueList<ChannelColors>& colors);
 
 /** Sets the colors associated with the channels.
 * @param colors list of ChannelColors given the 3 color used by each channel.
 */ 
 void setColors(Q3ValueList<ChannelColors>& colors);
 
 /**True if at least one property has been modified, false otherwise.*/
 inline bool isModified()const{return modified;};
 
public slots:
 /**Will be called when the user wants to change a channel color.
 * @param row row in the table corresponding to the channel color to modified.
 * @param column rcolumn in the table corresponding to the type of channel color to modified.
 * @param button mouse button which has been pressed..  
 */
 void chooseColor(int row,int column,int button);
 
 /** Will be called when any properties is modified.*/
 inline void propertyModified(){modified = true;};
 
 /**Resets the internal modification status to false.*/
 inline void resetModificationStatus(){modified = false;};
  
private:
 int nbChannels; 
 bool modified;
};

#endif
