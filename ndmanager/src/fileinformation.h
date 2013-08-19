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
#ifndef FILEINFORMATION_H
#define FILEINFORMATION_H

// include files for QT
#include <qmap.h>
#include <QString>
#include <QList>
/**
* Class storing the information describing an additional file (filter and/or subset of the raw data file).
@author Lynn Hazan
*/
class FileInformation
{
public:
    /**Constructor.*/
    FileInformation() : samplingRate(0) {}
    /**Destructor.*/
    ~FileInformation(){}

    /**Gets the file sampling rate.*/
    inline double getSamplingRate()const{return samplingRate;}

    /**Gets the file extension.
  * @return the file extension.
  */
    QString getExtension()const{return extension;}

    /**Gets the mapping between the original channels and the ones in the file.
  * @return the channel mapping map.
  */
    inline QMap<int, QList<int> > getChannelMapping(){return channelMapping;}

    /**Sets the sampling rate.*/
    void setSamplingRate(double rate){samplingRate = rate;}

    /**Sets the file extension.
 * @param ext file extension.
 */
    void setExtension(const QString& ext){extension = ext;}

    /**Sets the mapping between the original channels and the ones in the file.
 * @param mapping the channel mapping map.
 */
    void setChannelMapping(const QMap<int, QList<int> >& mapping){channelMapping = mapping;}

private:
    double samplingRate;
    QString extension;
    QMap<int, QList<int> > channelMapping;
};

#endif
