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
#ifndef NEUROSCOPEVIDEOINFO_H
#define NEUROSCOPEVIDEOINFO_H

// include files for QT
#include <qstring.h>

/**
* Class storing the video information used by NeuroScope.
@author Lynn Hazan
*/
class NeuroscopeVideoInfo{
public:
    NeuroscopeVideoInfo()
        :flip(0), rotation(0),trajectory(0)
    {
    }

    ~NeuroscopeVideoInfo(){}

    /**Returns the video image rotation angle (0,90,180,270). The angle is counted counterclockwise.
  * @return rotation angle.
  */
    int getRotation()const{return rotation;}

    /**Returns the video image flip orientation. 0 stands for none, 1 for vertical and 2 for horizontal.
  * @return flip orientation.
  */
    int getFlip()const{return flip;}

    /**All the positions contained in a position file can be used to create a background image for the PositionView.
  * The value return by this function tells if such background has to be created.
  * @return 1 if the all the positions contain in the position file have to be drawn on the background, 0 otherwise.
   */
    int getTrajectory()const{return trajectory;}

    /**Gets the url of the background image.
  * @return the url of the background image, an empty string if no background has been choosen and a dash if the entry does not exist in the file.
  */
    QString getBackgroundImage()const{return backgroundImage;}

    /**Sets the video image rotation angle (0,90,180,270). The angle is counted counterclockwise.
  * @param angle the rotation angle.
  */
    void setRotation(int angle){rotation = angle;}

    /**Sets the video image flip orientation. 0 stands for none, 1 for vertical and 2 for horizontal.
  * @param flip flip orientation.
  */
    void setFlip(int flip){this->flip = flip;}

    /**All the positions contained in a position file can be used to create a background image for the PositionView.
  * The value set by this function tells if such background has to be created.
  * @param trajectory 1 if the all the positions contain in the position file have to be drawn on the background, 0 otherwise.
   */
    void setTrajectory(int trajectory){this->trajectory = trajectory;}

    /**Sets the url of the background image.
  * @param background the url of the background image.
  */
    void setBackgroundImage(const QString& background){backgroundImage = background;}
    
private:
    int flip;
    int rotation;
    int trajectory;
    QString backgroundImage;
};

#endif
