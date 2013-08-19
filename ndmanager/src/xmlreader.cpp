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
// application specific includes
#include "xmlreader.h"
//include files for QT
#include <qfile.h> 
#include <qstring.h> 
#include <QDomDocument>
#include <QList>
#include <QDebug>

using namespace ndmanager;

XmlReader::XmlReader()
{

}

XmlReader::~XmlReader()
{

}

bool XmlReader::parseFile(const QString& url)
{
    QFile input(url);

    QDomDocument docElement;
    QString errorMsg;
    int errorRow;
    int errorCol;
    if ( !docElement.setContent( &input, &errorMsg, &errorRow, &errorCol ) ) {
        qWarning() << "Unable to load document.Parse error in " << url << ", line " << errorRow << ", col " << errorCol << ": " << errorMsg << endl;
        return false;
    }

    QDomElement element = docElement.documentElement();

    if (element.tagName() == QLatin1String("parameters")) {
        if( element.hasAttribute(DOC__VERSION)) {
            readVersion = element.attribute(DOC__VERSION);
            qDebug()<<" readVersion"<<readVersion;
        }
    }
    documentNode = element;

    return true;
}

void XmlReader::closeFile(){
    readVersion.clear();

}

void XmlReader::getAcquisitionSystemInfo(QMap<QString,double>& acquisitionSystemInfo)const{
    //acquisitionSystemInfo will contain the number of bits, number of channels, the sampling rate, the voltage range, the amplification and the offset.

    QDomNode n = documentNode.firstChild();
    if (!n.isNull()) {
        while(!n.isNull()) {
            QDomElement e = n.toElement(); // try to convert the node to an element.
            if(!e.isNull()) {
                QString tag = e.tagName();
                if (tag == ACQUISITION) {
                    QDomNode acquisition = e.firstChild(); // try to convert the node to an element.
                    while(!acquisition.isNull()) {
                        QDomElement u = acquisition.toElement();
                        if (!u.isNull()) {
                            tag = u.tagName();
                            if (tag == BITS) {
                                int resolution = u.text().toInt();
                                acquisitionSystemInfo.insert(BITS,resolution);
                            } else if (tag == NB_CHANNELS) {
                                int nbChannels = u.text().toInt();
                                acquisitionSystemInfo.insert(NB_CHANNELS,nbChannels);
                            } else if (tag == SAMPLING_RATE) {
                                double samplingRate = u.text().toDouble();
                                acquisitionSystemInfo.insert(SAMPLING_RATE,samplingRate);
                            } else if (tag == VOLTAGE_RANGE) {
                                double samplingRate = u.text().toDouble();
                                acquisitionSystemInfo.insert(VOLTAGE_RANGE,samplingRate);
                            } else if(tag == AMPLIFICATION){
                                int amplification = u.text().toInt();
                                acquisitionSystemInfo.insert(AMPLIFICATION,amplification);
                            } else if(tag == OFFSET){
                                int offset = u.text().toInt();
                                acquisitionSystemInfo.insert(OFFSET,offset);
                            }
                        }
                        acquisition = acquisition.nextSibling();
                    }
                    break;
                }
            }
            n = n.nextSibling();
        }
    }
}

void XmlReader::getAnatomicalDescription(int nbChannels,QMap<int, QList<int> >& anatomicalGroups,QMap<QString, QMap<int,QString> >& attributes){
    //First, everything is put in the trash group with a skip status at false (this correspond to no anatomical group).
    //Then reading for the file, the right information is set.
    QList<int> trashList;
    //the attributes names are hard coded. For the moment therei is only one (skip)
    QMap<int,QString> skipStatus;
    for(int i = 0; i < nbChannels; ++i){
        trashList.append(i);
        skipStatus.insert(i,"0");
    }

    int i = 0;
    QDomNode n = documentNode.firstChild();
    if (!n.isNull()) {
        while(!n.isNull()) {
            QDomElement e = n.toElement(); // try to convert the node to an element.
            if(!e.isNull()) {
                QString tag = e.tagName();
                if (tag == ANATOMY) {
                    QDomNode anatomy = e.firstChild(); // try to convert the node to an element.
                    while(!anatomy.isNull()) {
                        QDomElement u = anatomy.toElement();
                        if (!u.isNull()) {
                            tag = u.tagName();
                            if (tag == CHANNEL_GROUPS) {
                                QDomNode channelGroup = u.firstChild(); // try to convert the node to an element.
                                while(!channelGroup.isNull()) {
                                    QDomElement val = channelGroup.toElement();
                                    if (!val.isNull()) {
                                        tag = val.tagName();
                                        if (tag == GROUP) {
                                            QList<int> channelList;
                                            QDomNode group = val.firstChild(); // try to convert the node to an element.
                                            while(!group.isNull()) {
                                                QDomElement valGroup = group.toElement();
                                                if (!valGroup.isNull()) {
                                                    tag = valGroup.tagName();
                                                    if (tag == CHANNEL) {
                                                        int channelId = valGroup.text().toInt();
                                                        channelList.append(channelId);
                                                        //remove the channel from the trash list as it is part of a group
                                                        trashList.removeAll(channelId);
                                                        if (valGroup.hasAttribute(SKIP)) {
                                                            skipStatus.insert(channelId,valGroup.attribute(SKIP));
                                                        }
                                                    }
                                                }
                                                group = group.nextSibling();
                                            }
                                            anatomicalGroups.insert(i + 1,channelList);
                                            i++;
                                        }
                                    }
                                    channelGroup = channelGroup.nextSibling();
                                }
                            }
                        }
                        anatomy = anatomy.nextSibling();
                    }
                    break;
                }
            }
            n = n.nextSibling();
        }
    }

    if(!trashList.isEmpty())
        anatomicalGroups.insert(0,trashList);
    attributes.insert("Skip",skipStatus);

}


void XmlReader::getSpikeDescription(int nbChannels,QMap<int, QList<int> >& spikeGroups,QMap<int, QMap<QString,QString> >& information){
    //Anatomical goups and spike groups share the trash group. the spikeChannelsGroups already contains the trash group, if any, set after retrieving the anatomical groups information.
    //At first, if a channel is not in the trash group it is put in the undefined group, the -1 (this correspond to no spike group).
    //Then reading for the file, the right information is set.
    QList<int> trashList;
    if(spikeGroups.contains(0))
        trashList = spikeGroups[0];
    QList<int> spikeTrashList;
    for(int i = 0; i < nbChannels; ++i){
        if(!trashList.contains(i))  {
            spikeTrashList.append(i);
        }
    }

    QDomNode n = documentNode.firstChild();
    if (!n.isNull()) {
        while(!n.isNull()) {
            QDomElement e = n.toElement(); // try to convert the node to an element.
            if(!e.isNull()) {
                QString tag = e.tagName();
                if (tag == SPIKE) {
                    QDomNode anatomy = e.firstChild(); // try to convert the node to an element.
                    while(!anatomy.isNull()) {
                        QDomElement u = anatomy.toElement();
                        if (!u.isNull()) {
                            tag = u.tagName();
                            if (tag == CHANNEL_GROUPS) {
                                QDomNode channelGroup = u.firstChild(); // try to convert the node to an element.
                                int i = 0;
                                while(!channelGroup.isNull()) {
                                    QDomElement val = channelGroup.toElement();
                                    if (!val.isNull()) {
                                        tag = val.tagName();
                                        if (tag == GROUP) {
                                            QMap<QString,QString> groupInfo;
                                            QDomNode group = val.firstChild(); // try to convert the node to an element.

                                            while(!group.isNull()) {
                                                QDomElement valGroup = group.toElement();

                                                if (!valGroup.isNull()) {
                                                    tag = valGroup.tagName();
                                                    if (tag == CHANNELS) {
                                                        QDomNode channels = valGroup.firstChild(); // try to convert the node to an element.
                                                        QList<int> channelList;
                                                        while(!channels.isNull()) {
                                                            QDomElement channelsElement = channels.toElement();
                                                            if( channelsElement.tagName() == CHANNEL) {
                                                                int channelId = channelsElement.text().toInt();
                                                                channelList.append(channelId);
                                                                //remove the channel from the spike trash list as it is part of a group
                                                                spikeTrashList.removeAll(channelId);

                                                            }
                                                            channels = channels.nextSibling();
                                                        }
                                                        spikeGroups.insert(i + 1,channelList);
                                                    }else if( tag == NB_SAMPLES) {
                                                        groupInfo.insert(NB_SAMPLES,valGroup.text());
                                                    } else if( tag == PEAK_SAMPLE_INDEX) {
                                                        groupInfo.insert(PEAK_SAMPLE_INDEX,valGroup.text());
                                                    } else if( tag == NB_FEATURES) {
                                                        groupInfo.insert(NB_FEATURES,valGroup.text());
                                                    }
                                                }
                                                group = group.nextSibling();
                                            }
                                            information.insert(i + 1,groupInfo);
                                            ++i;

                                        }
                                    }
                                    channelGroup = channelGroup.nextSibling();
                                }
                            }
                        }
                        anatomy = anatomy.nextSibling();
                    }
                    break;
                }
            }
            n = n.nextSibling();
        }
    }
    //The trash and undefined groups are not store as they are not shown (channels are not in any group are either in the trash group or the undefined group).
    if(spikeGroups.contains(0))
        spikeGroups.remove(0);

}

void XmlReader::getUnits(QMap<int, QStringList >& units) const{

    QDomNode n = documentNode.firstChild();
    if (!n.isNull()) {
        while(!n.isNull()) {
            QDomElement e = n.toElement(); // try to convert the node to an element.
            if(!e.isNull()) {
                QString tag = e.tagName();
                if (tag == UNITS) {
                    QDomNode unit = e.firstChild(); // try to convert the node to an element.
                    int i = 0;
                    while(!unit.isNull()) {
                        QStringList unitInfo;
                        QDomElement u = unit.toElement();
                        if (!u.isNull()) {
                            tag = u.tagName();
                            QDomNode val = u.firstChild();
                            while(!val.isNull()) {
                                QDomElement valElement = val.toElement();
                                if (!valElement.isNull()) {
                                    tag = valElement.tagName();
                                    if (tag == GROUP) {
                                        QString group = valElement.text();
                                        unitInfo.append(group);
                                    } else if (tag == CLUSTER) {
                                        QString cluster = valElement.text();
                                        unitInfo.append(cluster);
                                    } else if (tag ==STRUCTURE ) {
                                        QString structure = valElement.text();
                                        unitInfo.append(structure);
                                    } else if (tag ==TYPE ) {
                                        QString type = valElement.text();
                                        unitInfo.append(type);
                                    }else if (tag == ISOLATION_DISTANCE) {
                                        QString id = valElement.text();
                                        unitInfo.append(id);

                                    }else if (tag == QUALITY) {
                                        QString quality = valElement.text();
                                        unitInfo.append(quality);
                                    }else if (tag == NOTES) {
                                        QString notes = valElement.text();
                                        unitInfo.append(notes);
                                    }
                                }
                                val = val.nextSibling();
                            }
                        }
                        units.insert(i,unitInfo);
                        i++;
                        unit = unit.nextSibling();
                    }
                }
            }
            n = n.nextSibling();
        }
    }
}

void XmlReader::getChannelColors(QList<ChannelColors>& list)const{
    QDomNode n = documentNode.firstChild();
    if (!n.isNull()) {
        while(!n.isNull()) {
            QDomElement e = n.toElement(); // try to convert the node to an element.
            if(!e.isNull()) {
                QString tag = e.tagName();
                if (tag == NEUROSCOPE) {
                    QDomNode channels = e.firstChildElement(CHANNELS); // try to convert the node to an element.
                    if (!channels.isNull()) {
                        QDomNode channelColors = channels.firstChild();
                        while(!channelColors.isNull()) {
                            QDomElement w = channelColors.toElement();
                            if(!w.isNull()) {
                                tag = w.tagName();
                                if (tag == CHANNEL_COLORS) {
                                    QDomNode channelGroup = w.firstChild(); // try to convert the node to an element.
                                    ChannelColors channelColors;
                                    while(!channelGroup.isNull()) {
                                        QDomElement val = channelGroup.toElement();
                                        if (!val.isNull()) {
                                            tag = val.tagName();
                                            if (tag == CHANNEL) {
                                                int channelId = val.text().toInt();
                                                channelColors.setId(channelId) ;
                                            } else if(tag == COLOR) {
                                                QString color = val.text();
                                                channelColors.setColor(color) ;
                                            } else if(tag == ANATOMY_COLOR) {
                                                QString color = val.text();
                                                channelColors.setGroupColor(color) ;
                                            } else if(tag == SPIKE_COLOR){
                                                QString color = val.text();
                                                channelColors.setSpikeGroupColor(color);
                                            }
                                        }
                                        channelGroup =  channelGroup.nextSibling();
                                    }
                                    list.append(channelColors);
                                }
                            }
                            channelColors = channelColors.nextSibling();
                        }
                    }
                }
            }
            n = n.nextSibling();
        }
    }

}

void XmlReader::getChannelDefaultOffset(QMap<int,int>& channelDefaultOffsets)const{
    QDomNode n = documentNode.firstChild();
    if (!n.isNull()) {
        while(!n.isNull()) {
            QDomElement e = n.toElement(); // try to convert the node to an element.
            if(!e.isNull()) {
                QString tag = e.tagName();
                if (tag == NEUROSCOPE) {
                    QDomNode channels = e.firstChildElement(CHANNELS); // try to convert the node to an element.
                    if (!channels.isNull()) {
                        QDomNode channelColors = channels.firstChild();
                        int i = 0;
                        while(!channelColors.isNull()) {
                            QDomElement w = channelColors.toElement();
                            if(!w.isNull()) {
                                tag = w.tagName();
                                if (tag == CHANNEL_OFFSET) {
                                    QDomNode channelGroup = w.firstChild(); // try to convert the node to an element.
                                    int channelId = i;
                                    int offset = 0;

                                    while(!channelGroup.isNull()) {
                                        QDomElement val = channelGroup.toElement();
                                        if (!val.isNull()) {
                                            tag = val.tagName();
                                            if (tag == CHANNEL) {
                                                channelId = val.text().toInt();
                                            } else if(tag == DEFAULT_OFFSET) {
                                                offset = val.text().toInt();
                                            }
                                        }
                                        //the channels must be numbered continuously from 0.
                                        //if(channelId < nbChannels)
                                        channelDefaultOffsets.insert(channelId,offset);
                                        channelGroup =  channelGroup.nextSibling();
                                    }
                                }
                            }
                            channelColors = channelColors.nextSibling();
                            i++;
                        }
                    }
                }
            }
            n = n.nextSibling();
        }
    }
}

void XmlReader::getGeneralInformation(GeneralInformation& generalInformation)const{
    QDomNode n = documentNode.firstChild();
    if (!n.isNull()) {
        while(!n.isNull()) {
            QDomElement e = n.toElement(); // try to convert the node to an element.
            if(!e.isNull()) {
                QString tag = e.tagName();
                if (tag == GENERAL) {
                    QDomNode video = e.firstChild(); // try to convert the node to an element.
                    while(!video.isNull()) {
                        QDomElement u = video.toElement();
                        if (!u.isNull()) {
                            tag = u.tagName();
                            if (tag == DATE) {
                                QString date = u.text();
                                if(!date.isEmpty())
                                    generalInformation.setDate(QDate::fromString(date,Qt::ISODate));
                            } else if(tag == EXPERIMENTERS) {
                                QString experimenters = u.text();
                                generalInformation.setExperimenters(experimenters);
                            } else if(tag == DESCRIPTION) {
                                QString description = u.text();
                                generalInformation.setDescription(description);
                            } else if(tag == NOTES) {
                                QString notes = u.text();
                                generalInformation.setNotes(notes);
                            }
                        }
                        video = video.nextSibling();
                    }
                    break;

                }
            }
            n = n.nextSibling();
        }
    }
}



void XmlReader::getVideoInfo(QMap<QString,double>& videoInformation)const
{
    QDomNode n = documentNode.firstChild();
    if (!n.isNull()) {
        while(!n.isNull()) {
            QDomElement e = n.toElement(); // try to convert the node to an element.
            if(!e.isNull()) {
                QString tag = e.tagName();
                if (tag == VIDEO) {
                    QDomNode video = e.firstChild(); // try to convert the node to an element.
                    while(!video.isNull()) {
                        QDomElement u = video.toElement();
                        if (!u.isNull()) {
                            tag = u.tagName();
                            if (tag == SAMPLING_RATE) {
                                double samplingRate = u.text().toDouble();
                                videoInformation.insert(SAMPLING_RATE,samplingRate);
                            } else if(tag == WIDTH) {
                                int width = u.text().toInt();
                                videoInformation.insert(WIDTH,width);

                            } else if(tag == HEIGHT) {
                                int height = u.text().toInt();
                                videoInformation.insert(HEIGHT,height);
                            }
                        }
                        video = video.nextSibling();
                    }
                    break;
                }
            }
            n = n.nextSibling();
        }
    }
}

double XmlReader::getLfpInformation()const
{
    double lfpSamplingRate = 0;
    QDomNode n = documentNode.firstChild();
    if (!n.isNull()) {
        while(!n.isNull()) {
            QDomElement e = n.toElement(); // try to convert the node to an element.
            if(!e.isNull()) {
                QString tag = e.tagName();
                if (tag == FIELD_POTENTIALS) {
                    QDomNode video = e.firstChild(); // try to convert the node to an element.
                    while(!video.isNull()) {
                        QDomElement u = video.toElement();
                        if (!u.isNull()) {
                            tag = u.tagName();
                            if (tag == LFP_SAMPLING_RATE) {
                                lfpSamplingRate = u.text().toDouble();
                                return lfpSamplingRate;
                            }
                        }
                        video = video.nextSibling();
                    }
                    break;
                }
            }
            n = n.nextSibling();
        }
    }
    return lfpSamplingRate;
}

float XmlReader::getScreenGain() const{
    float gain = 0;

    QDomNode n = documentNode.firstChild();
    if (!n.isNull()) {
        while(!n.isNull()) {
            QDomElement e = n.toElement(); // try to convert the node to an element.
            if(!e.isNull()) {
                QString tag = e.tagName();
                if (tag == NEUROSCOPE) {
                    QDomNode video = e.firstChildElement(MISCELLANEOUS); // try to convert the node to an element.
                    if (!video.isNull()) {
                        QDomNode b = video.firstChild();
                        while(!b.isNull()) {
                            QDomElement w = b.toElement();
                            if(!w.isNull()) {
                                tag = w.tagName();
                                if (tag == SCREENGAIN) {
                                    gain = w.text().toFloat();
                                    return gain;
                                }
                            }
                            b = b.nextSibling();
                        }
                    }
                }
            }
            n = n.nextSibling();
        }
    }

    return gain;
}

QString XmlReader::getTraceBackgroundImage() const{
    QString traceBackgroundImage = 0;
    QDomNode n = documentNode.firstChild();
    if (!n.isNull()) {
        while(!n.isNull()) {
            QDomElement e = n.toElement(); // try to convert the node to an element.
            if(!e.isNull()) {
                QString tag = e.tagName();
                if (tag == NEUROSCOPE) {
                    QDomNode video = e.firstChildElement(MISCELLANEOUS); // try to convert the node to an element.
                    if (!video.isNull()) {
                        QDomNode b = video.firstChild();
                        while(!b.isNull()) {
                            QDomElement w = b.toElement();
                            if(!w.isNull()) {
                                tag = w.tagName();
                                if (tag == TRACE_BACKGROUND_IMAGE) {
                                    traceBackgroundImage = w.text();
                                    return traceBackgroundImage;
                                }
                            }
                            b = b.nextSibling();
                        }
                    }
                }
            }
            n = n.nextSibling();
        }
    }

    return traceBackgroundImage;
}

int XmlReader::getNbSamples() const{
    int nbSamples = 0;
    QDomNode n = documentNode.firstChild();
    if (!n.isNull()) {
        while(!n.isNull()) {
            QDomElement e = n.toElement(); // try to convert the node to an element.
            if(!e.isNull()) {
                QString tag = e.tagName();
                if (tag == NEUROSCOPE) {
                    QDomNode video = e.firstChildElement(SPIKES); // try to convert the node to an element.
                    if (!video.isNull()) {
                        QDomNode b = video.firstChild();
                        while(!b.isNull()) {
                            QDomElement w = b.toElement();
                            if(!w.isNull()) {
                                tag = w.tagName();
                                if (tag == NB_SAMPLES) {
                                    nbSamples = w.text().toInt();
                                    return nbSamples;
                                }
                            }
                            b = b.nextSibling();
                        }
                    }
                }
            }
            n = n.nextSibling();
        }
    }
    return nbSamples;
}

int XmlReader::getPeakSampleIndex()const{
    int index = 0;
    QDomNode n = documentNode.firstChild();
    if (!n.isNull()) {
        while(!n.isNull()) {
            QDomElement e = n.toElement(); // try to convert the node to an element.
            if(!e.isNull()) {
                QString tag = e.tagName();
                if (tag == NEUROSCOPE) {
                    QDomNode video = e.firstChildElement(SPIKES); // try to convert the node to an element.
                    if (!video.isNull()) {
                        QDomNode b = video.firstChild();
                        while(!b.isNull()) {
                            QDomElement w = b.toElement();
                            if(!w.isNull()) {
                                tag = w.tagName();
                                if (tag == PEAK_SAMPLE_INDEX) {
                                    index = w.text().toInt();
                                    return index;
                                }
                            }
                            b = b.nextSibling();
                        }
                    }
                }
            }
            n = n.nextSibling();
        }
    }

    return index;
}

void XmlReader::getNeuroscopeVideoInfo(NeuroscopeVideoInfo& videoInfo) const{

    QDomNode n = documentNode.firstChild();
    if (!n.isNull()) {
        while(!n.isNull()) {
            QDomElement e = n.toElement(); // try to convert the node to an element.
            if(!e.isNull()) {
                QString tag = e.tagName();
                if (tag == NEUROSCOPE) {
                    QDomNode video = e.firstChildElement(VIDEO); // try to convert the node to an element.
                    if (!video.isNull()) {
                        QDomNode b = video.firstChild();
                        while(!b.isNull()) {
                            QDomElement w = b.toElement();
                            if(!w.isNull()) {
                                tag = w.tagName();
                                if (tag == ROTATE) {
                                    int angle = w.text().toInt();
                                    videoInfo.setRotation(angle);
                                } else if (tag == FLIP) {
                                    int orientation = w.text().toInt();
                                    videoInfo.setFlip(orientation);
                                } else if (tag == VIDEO_IMAGE) {
                                    QString backgroundPath = w.text();
                                    videoInfo.setBackgroundImage(backgroundPath);
                                } else if (tag == POSITIONS_BACKGROUND) {
                                    int drawTrajectory = w.text().toInt();
                                    videoInfo.setTrajectory(drawTrajectory);
                                }
                            }
                            b = b.nextSibling();
                        }
                    }
                }
            }
            n = n.nextSibling();
        }
    }
}

void XmlReader::getFilesInformation(QList<FileInformation>& files)const{
    QDomNode n = documentNode.firstChild();
    if (!n.isNull()) {
        while(!n.isNull()) {
            QDomElement e = n.toElement(); // try to convert the node to an element.
            if(!e.isNull()) {
                QString tag = e.tagName();
                if (tag == FILES) {
                    QDomNode unit = e.firstChild(); // try to convert the node to an element.
                    while(!unit.isNull()) {
                        QDomElement u = unit.toElement();
                        if (!u.isNull()) {
                            QDomNode val = u.firstChild(); // ndmanager::FILE
                            FileInformation fileInformation;
                            while(!val.isNull()) {
                                QDomElement valElement = val.toElement();
                                tag = valElement.tagName();
                                if (tag == EXTENSION) {
                                    QString extension = valElement.text();
                                    fileInformation.setExtension(extension);
                                } else if (tag == SAMPLING_RATE) {
                                    double samplingRate = valElement.text().toDouble();
                                    fileInformation.setSamplingRate(samplingRate);
                                } else if (tag == CHANNEL_MAPPING) {
                                    QMap<int, QList<int> > mapping;
                                    //loop on the ORIGINAL_CHANNELS which contain a list of channels
                                    QDomNode channelMapping = valElement.firstChild();
                                    if (!channelMapping.isNull()) {
                                        int channelId = 0;
                                        while(!channelMapping.isNull()) {
                                            QDomElement channelMappingElement = channelMapping.toElement();
                                            QList<int> channelList;
                                            if(channelMappingElement.tagName() == ORIGINAL_CHANNELS) {

                                                QDomNode channel = channelMappingElement.firstChild();
                                                while(!channel.isNull()) {
                                                    QDomElement channelElement = channel.toElement();
                                                    if( channelElement.tagName() == CHANNEL) {
                                                        int channelId = channelElement.text().toInt();
                                                        channelList.append(channelId);
                                                    }
                                                    channel = channel.nextSibling();
                                                }
                                            }
                                            if(!channelList.isEmpty())
                                                mapping.insert(channelId,channelList);
                                            channelId++;
                                            channelMapping = channelMapping.nextSibling();
                                        }
                                    }
                                    fileInformation.setChannelMapping(mapping);
                                }
                                val = val.nextSibling();
                            }
                            files.append(fileInformation);
                        }
                        unit = unit.nextSibling();
                    }
                }
            }
            n = n.nextSibling();
        }
    }
}

void XmlReader::getProgramsInformation(QList<ProgramInformation>& programs) const{
    QDomNode n = documentNode.firstChild();

    if (!n.isNull()) {
        while(!n.isNull()) {
            QDomElement e = n.toElement(); // try to convert the node to an element.
            if(!e.isNull()) {
                QString tag = e.tagName();
                QMap<int, QStringList > parameters;
                if (tag == PROGRAMS) {
                    QDomNode unit = e.firstChild(); // try to convert the node to an element.

                    while(!unit.isNull()) {
                        QDomElement u = unit.toElement();
                        if (!u.isNull()) {
                            ProgramInformation programInformation;
                            int parameterId = 0;
                            if( u.tagName() == PROGRAM) {

                                QDomNode program = u.firstChild();
                                while(!program.isNull()) {
                                    QDomElement p = program.toElement();
                                    QString tag =p.tagName();
                                    if( tag == NAME) {
                                        QString name = p.text();
                                        programInformation.setProgramName(name);
                                    } else if( tag == HELP) {
                                        QString help = p.text();
                                        programInformation.setHelp(help);
                                    } else if(tag ==PARAMETERS ){
                                        QDomNode param = p.firstChild();
                                        QStringList parameterInfo;
                                        while (!param.isNull()) {
                                            QDomNode subparam = param.firstChild();
                                            while (!subparam.isNull()) {
                                                QDomElement pe = subparam.toElement();
                                                if(!pe.isNull() ) {
                                                    QString tag = pe.tagName();
                                                    if(tag == NAME) {
                                                        QString name = pe.text();
                                                        parameterInfo.prepend(name);
                                                    } else if(tag == STATUS ) {
                                                        QString status = pe.text();
                                                        parameterInfo.append(status);

                                                    } else if(tag == VALUE) {
                                                        QString value = pe.text();
                                                        if(parameterInfo.size() == 1)
                                                            parameterInfo.append(value);
                                                        else{
                                                            QStringList::iterator it = parameterInfo.begin();
                                                            parameterInfo.insert(++it,value);
                                                        }
                                                    }
                                                }
                                                subparam = subparam.nextSibling();
                                            }
                                            if(!parameterInfo.isEmpty()) {
                                                parameters.insert(parameterId,parameterInfo);
                                                parameterId++;
                                            }
                                            param = param.nextSibling();
                                        }
                                    }
                                    program= program.nextSibling();
                                }
                            }
                            if(!parameters.isEmpty())
                                programInformation.setParameterInformation(parameters);
                            if(!programInformation.getProgramName().isEmpty())
                                programs.append(programInformation);
                            parameters.clear();
                            parameterId=0;
                        }
                        unit = unit.nextSibling();
                    }

                }
            }
            n = n.nextSibling();
        }
    }
}

void XmlReader::getProgramInformation(ProgramInformation& programInformation) const
{
    QDomNode n = documentNode.firstChild();
    if (!n.isNull()) {
        while(!n.isNull()) {
            QDomElement e = n.toElement(); // try to convert the node to an element.
            if(!e.isNull()) {
                QString tag = e.tagName();
                //ProgramInformation programInformation;
                QMap<int, QStringList > parameters;
                int parameterId = 0;
                if (tag == PROGRAM) {
                    QDomNode program = e.firstChild();
                    while(!program.isNull()) {
                        QDomElement p = program.toElement();
                        QString tag =p.tagName();
                        if( tag == NAME) {
                            QString name = p.text();
                            programInformation.setProgramName(name);
                        } else if( tag == HELP) {
                            QString help = p.text();
                            programInformation.setHelp(help);

                        } else if(tag ==PARAMETERS ){
                            QDomNode parameterElement = p.firstChild();
                            while (!parameterElement.isNull()) {
                                QDomNode parametersNode = parameterElement.firstChild();
                                QStringList parameterInfo;
                                while(!parametersNode.isNull()) {
                                    QDomElement parametersElement = parametersNode.toElement();
                                    if(!parametersElement.isNull() ) {
                                        QString tag = parametersElement.tagName();
                                        if(tag == NAME) {
                                            QString name = parametersElement.text();
                                            parameterInfo.prepend(name);

                                        } else if(tag == STATUS ) {
                                            QString status = parametersElement.text();
                                            parameterInfo.append(status);

                                        } else if(tag == VALUE) {
                                            QString value = parametersElement.text();
                                            if(parameterInfo.size() == 1)
                                                parameterInfo.append(value);
                                            else{
                                                QStringList::iterator it = parameterInfo.begin();
                                                parameterInfo.insert(++it,value);
                                            }

                                        }
                                    }
                                    parametersNode = parametersNode.nextSibling();
                                }
                                parameterElement = parameterElement.nextSibling();

                                if(!parameterInfo.isEmpty()) {
                                    parameters.insert(parameterId,parameterInfo);
                                    parameterId++;
                                }
                            }
                        }
                        program= program.nextSibling();
                    }
                    if(!parameters.isEmpty())
                        programInformation.setParameterInformation(parameters);
                }
            }
            n = n.nextSibling();
        }
    }
}
