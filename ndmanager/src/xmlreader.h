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
#ifndef XMLREADER_H
#define XMLREADER_H

//include files for the application
#include <channelcolorslayout.h>
#include "channelcolors.h"
#include "generalinformation.h"
#include "neuroscopevideoinfo.h"
#include "tags.h"
#include "fileinformation.h"
#include "programinformation.h"




//include files for QT
#include <QList> 
#include <QDomNode>

/**
@author Lynn Hazan
*/
class XmlReader{
public:
    XmlReader();
    ~XmlReader();

    /**Opens and parses the file with the @p url.
  * @param url url of the file to open.
  * @return true if the file was correctly parse, false othewise.
  */
    bool parseFile(const QString& url);

    /**Closes the currently open file.*/
    void closeFile();

    /** Gets all the information related to the acquisition system.
 * @param acquisitionSystemInfo map given the acquisition system information.
 */
    void getAcquisitionSystemInfo(QMap<QString,double>& acquisitionSystemInfo)const;

    /**
  * Gets the anatomical description: the groups and channel attributes.
  * @param nbChannels total number of channels.
  * @param anatomicalGroups reference to the map given the correspondance between the anatomical group ids and the channel ids.
  * @param attributes reference to the map given the correspondance between the attribute names and a map given for each channel the value of the attribute.
  */
    void getAnatomicalDescription(int nbChannels,QMap<int, QList<int> >& anatomicalGroups,QMap<QString, QMap<int,QString> >& attributes);

    /**
  * Gets the spike description: the groups and other type of information specific to a spike group (as the number of features).
  * @param nbChannels total number of channels.
  * @param spikeGroups reference to the map given the correspondance between the spike group ids and the channel ids.
  * @param information reference to the map given the correspondance between the spike group ids and a map given the name and the value of an additional information.
  */
    void getSpikeDescription(int nbChannels,QMap<int, QList<int> >& spikeGroups,QMap<int, QMap<QString,QString> >& information);

    /** Gets a GeneralInformation containing all the general information.
 * If a piece of information is not present, empty string is used as the value.
 * @param generalInformation a GeneralInformation to be filled.
 */
    void getUnits(QMap< int,QStringList >& units)const;
    /** Gets a GeneralInformation containing all the general information.
 * If a piece of information is not present, empty string is used as the value.
 * @param generalInformation a GeneralInformation to be filled.
 */
    void getGeneralInformation(GeneralInformation& generalInformation)const;

    /** Gets the video system information. If a piece of information is not present, a zero is return as the value.
 * @param videoInformation map given the video system information.
 */
    void getVideoInfo(QMap<QString,double>& videoInformation)const;

    /**
 * Returns the local field potential sampling rate in hertz.
 * @return local field potential sampling rate in hertz.
 */
    double getLfpInformation()const;

    //files

    /**
 * Gets the information for the additional files derive from the original raw data file (.dat file).
 * @param files a list of FileInformation to be filled.
 */
    void getFilesInformation(QList<FileInformation>& files)const;

    //NeuroScope related information

    /** Gets the list of ChannelColors, class given the color for each channel.
 * @param list list of ChannelColors.
 */
    void getChannelColors(QList<ChannelColors>& list)const;

    /**Returns the screen gain in milivolts by centimeters used to display the field potentiels in NeuroScope,
 * or zero if the element could not be found in the file.
 * @return the screen gain.
 */
    float getScreenGain() const;

    /**Returns the background image used for the trace view in NeuroScope.*/
    QString getTraceBackgroundImage() const;

    /** Gets all the video information specific to NeuroScope.
 * @param videoInfo a NeuroscopeVideoInfo.
 */
    void getNeuroscopeVideoInfo(NeuroscopeVideoInfo& videoInfo) const;

    /** Gets the map of Channel default offsets.
 * @param channelDefaultOffsets map given the channel default offsets.
 */
    void getChannelDefaultOffset(QMap<int,int>& channelDefaultOffsets)const;

    /**Returns the number of samples in a spike use by NeuroScope,
  * or zero if the element could not be found in the file.
  * @note NeuroScope currently uses a unique value for all the spike groups.
  * @return number of samples.
  */
    int getNbSamples()const;

    /**Returns the sample index corresponding to the peak of the spike use by NeuroScope,
  * or zero if the element could not be found in the file.
  * @note NeuroScope currently uses a unique value for all the spike groups.
  * @return index.
  */
    int getPeakSampleIndex()const;

    //programs

    /** Gets the information for all the programs used to process the data link to the current parameter file.
 * @param programs a list of ProgramInformation to be filled.
 */
    void getProgramsInformation(QList<ProgramInformation>& programs) const;

    /** Gets the information for one program used to process the data link to the current parameter file.
 * @param programInformation a ProgramInformation to be filled with the information concerning the program.
 */
    void getProgramInformation(ProgramInformation& programInformation) const;

private:
    QString readVersion;

    QDomNode documentNode;
};

#endif
