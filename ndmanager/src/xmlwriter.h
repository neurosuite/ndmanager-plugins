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
#ifndef XMLWRITER_H
#define XMLWRITER_H




//include files for the application
#include "generalinformation.h"
#include "fileinformation.h"
#include "programinformation.h"
#include "neuroscopevideoinfo.h"
#include "channelcolors.h"

//include files for QT
#include <QList>
#include <qmap.h>
#include <qdom.h> 


/**
This class writes the parameter file to disk.

@author Lynn Hazan
*/
class XmlWriter{
public:
    /**Constructor.*/
    XmlWriter();
    /**Destructor.*/
    ~XmlWriter();

    /**Writes the xml tree to a parameter file given by @p url.
  * @param url url of the file to write to.
  * @return true if the parameter file could be write to disk, false otherwise.
  */
    bool writeTofile(const QString& url);

    /** Creates the element containing all the general information.
  * @param generalInformation reference to ageneralInformation a GeneralInformation containing all the information.
  */
    void setGeneralInformation(GeneralInformation& generalInformation);

    /**
  * Creates the elements related to the acquisition system.
  * @param acquisitionSystemInfo reference to a map acquisitionSystemInfo providing the acquisition system information.
  */
    void setAcquisitionSystemInformation(const QMap<QString, double> &acquisitionSystemInfo);

    /**
  * Creates the elements containing the video information.
  * @param videoInformation reference to the map providing the video system information.
  */
    void setVideoInformation(const QMap<QString,double>& videoInformation);

    /**
  * Creates the element related to the field potentials.
  * @param lfpSamplingRate local field potential sampling rate in hertz.
  */
    void setLfpInformation(double lfpSamplingRate);

    /**
 *  Creates the elements containing the information for the additional files derive from the original raw data file (.dat file).
 * @param files a reference to a list of FileInformation.
 */
    void setFilesInformation(const QList<FileInformation> &files);

    /**
  * Creates the elements related to the anatomical description.
  * @param anatomicalGroups reference to a map providing information about the anatomical group to which each channel belongs.
  * @param attributes reference to a map providing the correspondance between the attribute names and a map providing for each channel the value of the attribute.
  */
    void setAnatomicalDescription(QMap<int, QList<int> >& anatomicalGroups,QMap<QString, QMap<int,QString> >& attributes);

    /**
  * Creates the elements related to the spike detection.
  * @param spikeGroups reference to the map providing the correspondance between the spike group ids and the channel ids.
  * @param information reference to the map providing the correspondance between the spike group ids and a map providing the name and the value of an additional information.
  */
    void setSpikeDetectionInformation(QMap<int, QList<int> >& spikeGroups,QMap<int, QMap<QString,QString> >& information);


    /**
  * Creates the elements containing NeuroScope miscellaneous information.
  * @param screenGain screen gain in millivolts by centimeters used to display the field potentiels.
  * @param traceBackgroundImage background image for the trace view in NeuroScope.
  */
    void setMiscellaneousInformation(float screenGain,const QString& traceBackgroundImage);

    /**
  * Creates the elements containing the video information used in NeuroScope.
  * @param videoInfo reference to a NeuroscopeVideoInfo.
  */
    void setNeuroscopeVideoInformation(NeuroscopeVideoInfo& videoInfo);
    
    /**
  * Creates the elements containing the spike information used in NeuroScope.
  * @param nbSamples number of samples in a spike.
  * @param peakSampleIndex sample index corresponding to the peak of the spike.
  */
    void setNeuroscopeSpikeInformation(int nbSamples,int peakSampleIndex);

    /**
  * Creates the elements related to the channels colors used in NeuroScope.
  * @param colorList reference to a list of ChannelColors (color use to display the channel,
  * color of the anatomical group to which the channel belongs,
  * color of the spike group to which the channel belongs).
  * @param channelDefaultOffsets reference to a map providing channel default offsets.
  */
    void setChannelDisplayInformation(const QList<ChannelColors>& colorList,const QMap<int,int>& channelDefaultOffsets);
    
    /** Creates the elements containing the information for all the programs used to process the data link to the current parameter file.
 * @param programs a reference to a list of ProgramInformation.
 */
    void setProgramsInformation(const QList<ProgramInformation>& programs);

    /** Creates the elements containing the information for units.
 * @param units a reference to a map providing the units information.
 */
    void setUnitsInformation(const QMap<int, QStringList >& units);

private:

    /**The session document.*/
    QDomDocument doc;

    /**The root element.*/
    QDomElement root;

    /**The general inforamtion element.*/
    QDomElement generalInfo;

    /**The acquisition system element.*/
    QDomElement acquisitionSystem;

    /**The element containing the video information.*/
    QDomElement video;

    /**The local field potential sampling rate element.*/
    QDomElement lfp;

    /**The files element contains the extra files.*/
    QDomElement files;

    /**The anatomical description element.*/
    QDomElement anatomicalDescription;

    /**The spike detection element.*/
    QDomElement spikeDetection;

    /**The miscellaneous element in the neuroscope element.*/
    QDomElement miscellaneous;
    
    /**The spikes element contained in the neuroscope element.*/
    QDomElement spikes;

    /**The element containing the video information in the neuroscope element.*/
    QDomElement neuroscopeVideo;

    /**The element containing the information about the channel colors.*/
    QDomElement channels;

    /**The element containing the information about the channel default offsets.*/
    QDomElement channelDefaultOffsets;

    /**The element containing the information about the programs.*/
    QDomElement programs;

    /**The element containing the information about the units.*/
    QDomElement units;

    static const QString parameterVersion;
};

#endif
