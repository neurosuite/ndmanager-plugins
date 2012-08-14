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
// application specific includes
#include "xmlreader.h"

// include files for KDE
#include <kurl.h>
#include <kio/netaccess.h>

//include files for QT
#include <qfile.h> 
#include <qstring.h> 

//General C++ include files
#include <iostream>
using namespace std;

using namespace ndmanager;

XmlReader::XmlReader():readVersion(""){}
XmlReader::~XmlReader(){}

bool XmlReader::parseFile(const KURL& url){  

 // Init libxml
 xmlInitParser();

 // Load XML document
 doc = xmlParseFile(url.path());
 if(doc == NULL) return false;
 
 // Create xpath evaluation context
 xpathContex = xmlXPathNewContext(doc);
 if(xpathContex == NULL){
  xmlFreeDoc(doc);
  return false;   
 }

 //Read the document version
 xmlNodePtr rootElement = xmlDocGetRootElement(doc);
 xmlChar* versionTag = xmlCharStrdup(DOC__VERSION);
 if(rootElement != NULL){
  xmlChar* sVersion = xmlGetProp(rootElement,versionTag);//get the attribute with the name versionTag
  if(sVersion != NULL) readVersion = QString((char*)sVersion);
  xmlFree(sVersion);
 }
  xmlFree(versionTag);  
 
 return true;
}

void XmlReader::closeFile(){
 //Cleanup
 xmlXPathFreeContext(xpathContex);
 xmlFreeDoc(doc);
 readVersion = "";
 
 //Shutdown libxml
 xmlCleanupParser();
}

void XmlReader::getAcquisitionSystemInfo(QMap<QString,double>& acquisitionSystemInfo)const{
 //acquisitionSystemInfo will contain the number of bits, number of channels, the sampling rate, the voltage range, the amplification and the offset.

 xmlXPathObjectPtr result;
 xmlChar* searchPath = xmlCharStrdup("//" + ACQUISITION);
 
 //Evaluate xpath expression
 result = xmlXPathEvalExpression(searchPath,xpathContex);
 if(result != NULL){ 
  xmlNodeSetPtr nodeset = result->nodesetval;  
  if(!xmlXPathNodeSetIsEmpty(nodeset)){
   xmlNodePtr child;
   for(child = nodeset->nodeTab[0]->children;child != NULL;child = child->next){
    //skip the carriage return (text node named text and containing /n)
    if(child->type == XML_TEXT_NODE) continue;
 
    if(QString((char*)child->name) == BITS){
     xmlChar* sResolution = xmlNodeListGetString(doc,child->children, 1);
     int resolution = QString((char*)sResolution).toInt();
     xmlFree(sResolution);
     acquisitionSystemInfo.insert(BITS,resolution);
    }
    if(QString((char*)child->name) == NB_CHANNELS){
     xmlChar* sNbChannels = xmlNodeListGetString(doc,child->children, 1);
     int nbChannels = QString((char*)sNbChannels).toInt();
     xmlFree(sNbChannels);
     acquisitionSystemInfo.insert(NB_CHANNELS,nbChannels);
    }
    if(QString((char*)child->name) == SAMPLING_RATE){
     xmlChar* sSamplingRate = xmlNodeListGetString(doc,child->children, 1);
     double samplingRate = QString((char*)sSamplingRate).toDouble();
     xmlFree(sSamplingRate);
     acquisitionSystemInfo.insert(SAMPLING_RATE,samplingRate);
    }  
    if(QString((char*)child->name) == VOLTAGE_RANGE){
     xmlChar* sRange = xmlNodeListGetString(doc,child->children, 1);
     int range = QString((char*)sRange).toInt();
     xmlFree(sRange);
     acquisitionSystemInfo.insert(VOLTAGE_RANGE,range);
    }    
    if(QString((char*)child->name) == AMPLIFICATION){
     xmlChar* sAmplification = xmlNodeListGetString(doc,child->children, 1);
     int amplification = QString((char*)sAmplification).toInt();
     xmlFree(sAmplification);
     acquisitionSystemInfo.insert(AMPLIFICATION,amplification);
    }    
    if(QString((char*)child->name) == OFFSET){
     xmlChar* sOffset = xmlNodeListGetString(doc,child->children, 1);
     int offset = QString((char*)sOffset).toInt();
     xmlFree(sOffset);
     acquisitionSystemInfo.insert(OFFSET,offset);
    }            
   }
  }
 }  
 
 xmlFree(searchPath); 
 xmlXPathFreeObject(result);
}

void XmlReader::getAnatomicalDescription(int nbChannels,QMap<int, QValueList<int> >& anatomicalGroups,QMap<QString, QMap<int,QString> >& attributes){
 //First, everything is put in the trash group with a skip status at false (this correspond to no anatomical group).
 //Then reading for the file, the right information is set.
 QValueList<int> trashList;
 //the attributes names are hard coded. For the moment therei is only one (skip)
 QMap<int,QString> skipStatus;
 for(int i = 0; i < nbChannels; ++i){
  trashList.append(i);
  skipStatus.insert(i,"0"); 
 } 
 
 xmlXPathObjectPtr result;
 xmlChar* searchPath = xmlCharStrdup("//" + ANATOMY + "/" + CHANNEL_GROUPS + "/" + GROUP);

 //Evaluate xpath expression
 result = xmlXPathEvalExpression(searchPath,xpathContex);
 if(result != NULL){
  xmlNodeSetPtr nodeset = result->nodesetval;
  if(!xmlXPathNodeSetIsEmpty(nodeset)){
   //loop on all the GROUP.
   int nbGroups = nodeset->nodeNr;
   for(int i = 0; i < nbGroups; ++i){
    QValueList<int> channelList;
    xmlNodePtr child;
    for(child = nodeset->nodeTab[i]->children;child != NULL;child = child->next){
     //skip the carriage return (text node named text and containing /n)
     if(child->type == XML_TEXT_NODE) continue;
      
     if(QString((char*)child->name) == CHANNEL){
      xmlChar* sId = xmlNodeListGetString(doc,child->children, 1);
      int channelId = QString((char*)sId).toInt();
      xmlFree(sId);
      channelList.append(channelId);
      //remove the channel from the trash list as it is part of a group
      trashList.remove(channelId);
            
      //Look up for the SKIP attribute
      xmlChar* skipTag = xmlCharStrdup(SKIP);
      xmlChar* sSkip = xmlGetProp(child,skipTag);
      if(sSkip != NULL) skipStatus.insert(channelId,QString((char*)sSkip));//replace the default value 
      xmlFree(skipTag); 
      xmlFree(sSkip);      
     }
    }
    anatomicalGroups.insert(i + 1,channelList);
   }
  }
 }

 if(trashList.size() != 0) anatomicalGroups.insert(0,trashList);
 attributes.insert("Skip",skipStatus);
  
 xmlFree(searchPath); 
 xmlXPathFreeObject(result);
}


void XmlReader::getSpikeDescription(int nbChannels,QMap<int, QValueList<int> >& spikeGroups,QMap<int, QMap<QString,QString> >& information){
 //Anatomical goups and spike groups share the trash group. the spikeChannelsGroups already contains the trash group, if any, set after retrieving the anatomical groups information.
 //At first, if a channel is not in the trash group it is put in the undefined group, the -1 (this correspond to no spike group).
 //Then reading for the file, the right information is set.
 QValueList<int> trashList;
 if(spikeGroups.contains(0)) trashList = spikeGroups[0];
 QValueList<int> spikeTrashList;
 for(int i = 0; i < nbChannels; ++i){
  if(!trashList.contains(i)) spikeTrashList.append(i);
 }

 xmlXPathObjectPtr result;
 xmlChar* searchPath = xmlCharStrdup("//" + SPIKE + "/" + CHANNEL_GROUPS + "/" + GROUP);
 
 //Evaluate xpath expression
 result = xmlXPathEvalExpression(searchPath,xpathContex);
 if(result != NULL){
  xmlNodeSetPtr nodeset = result->nodesetval;
  if(!xmlXPathNodeSetIsEmpty(nodeset)){
   //loop on all the GROUP.
   int nbGroups = nodeset->nodeNr;
   for(int i = 0; i < nbGroups; ++i){
    xmlNodePtr child;
    QMap<QString,QString> groupInfo;
    for(child = nodeset->nodeTab[i]->children;child != NULL;child = child->next){
     //skip the carriage return (text node named text and containing /n)
     if(child->type == XML_TEXT_NODE) continue;
      
     if(QString((char*)child->name) == CHANNELS){
      //loop on the channels
      xmlNodePtr channels;
      QValueList<int> channelList;
      for(channels = child->children;channels != NULL;channels = channels->next){
       //skip the carriage return (text node named text and containing /n)
       if(channels->type == XML_TEXT_NODE) continue;
      
       if(QString((char*)channels->name) == CHANNEL){
        xmlChar* sId = xmlNodeListGetString(doc,channels->children, 1);
        int channelId = QString((char*)sId).toInt();
        xmlFree(sId);      
        channelList.append(channelId);
        //remove the channel from the spike trash list as it is part of a group
        spikeTrashList.remove(channelId);
       }
      }
      spikeGroups.insert(i + 1,channelList);
     }
     
     
     if(QString((char*)child->name) == NB_SAMPLES){
      xmlChar* sNbSamples = xmlNodeListGetString(doc,child->children, 1);
      groupInfo.insert(NB_SAMPLES,QString((char*)sNbSamples));
      xmlFree(sNbSamples);
     }
     
     if(QString((char*)child->name) == PEAK_SAMPLE_INDEX){
      xmlChar* sindex = xmlNodeListGetString(doc,child->children, 1);
      groupInfo.insert(PEAK_SAMPLE_INDEX,QString((char*)sindex));
      xmlFree(sindex);
     }
    
     if(QString((char*)child->name) == NB_FEATURES){
      xmlChar* sNbFeatures = xmlNodeListGetString(doc,child->children, 1);
      groupInfo.insert(NB_FEATURES,QString((char*)sNbFeatures));
      xmlFree(sNbFeatures);
     }
    }
    information.insert(i + 1,groupInfo);
   }
  }
 }

 //The trash and undefined groups are not store as they are not shown (channels are not in any group are either in the trash group or the undefined group).
 if(spikeGroups.contains(0)) spikeGroups.remove(0);

 xmlFree(searchPath); 
 xmlXPathFreeObject(result); 
}

void XmlReader::getUnits(QMap<int, QValueList<QString> >& units) const{
 
 xmlXPathObjectPtr result;
 xmlChar* searchPath = xmlCharStrdup("//" + UNITS + "/" + UNIT);

 //Evaluate xpath expression
 result = xmlXPathEvalExpression(searchPath,xpathContex);
 if(result != NULL){
  xmlNodeSetPtr nodeset = result->nodesetval;
  if(!xmlXPathNodeSetIsEmpty(nodeset)){
   //loop on all the UNIT.
   int nbUnits = nodeset->nodeNr;

   for(int i = 0; i < nbUnits; ++i){
    QValueList<QString> unitInfo;
    xmlNodePtr child;
    for(child = nodeset->nodeTab[i]->children;child != NULL;child = child->next){
     //skip the carriage return (text node named text and containing /n)
     if(child->type == XML_TEXT_NODE) continue;
      
     if(QString((char*)child->name) == GROUP){
      xmlChar* sGroup = xmlNodeListGetString(doc,child->children, 1);
		QString group = QString((char*)sGroup);
      xmlFree(sGroup);  
      unitInfo.append(group);            
     }

	  if(QString((char*)child->name) == CLUSTER){
      xmlChar* sCluster = xmlNodeListGetString(doc,child->children, 1);
		QString cluster = QString((char*)sCluster);
      xmlFree(sCluster);  
      unitInfo.append(cluster);            
     }	

	  if(QString((char*)child->name) == STRUCTURE){
		  xmlChar* sStructure = xmlNodeListGetString(doc,child->children, 1);
		  QString structure = QString((char*)sStructure);
		  xmlFree(sStructure);  
		  unitInfo.append(structure);            
	  }	

	  if(QString((char*)child->name) == TYPE){
		  xmlChar* sType = xmlNodeListGetString(doc,child->children, 1);
		  QString type = QString((char*)sType);
		  xmlFree(sType);  
		  unitInfo.append(type);            
	  }	

	  if(QString((char*)child->name) == ISOLATION_DISTANCE){
      xmlChar* sID = xmlNodeListGetString(doc,child->children, 1);
		QString id = QString((char*)sID);
      xmlFree(sID);  
      unitInfo.append(id);            
     }	

	  if(QString((char*)child->name) == QUALITY){
      xmlChar* sQuality = xmlNodeListGetString(doc,child->children, 1);
		QString quality = QString((char*)sQuality);
      xmlFree(sQuality);  
      unitInfo.append(quality);            
     }	

	  if(QString((char*)child->name) == NOTES){
      xmlChar* sNotes = xmlNodeListGetString(doc,child->children, 1);
		QString notes = QString((char*)sNotes);
      xmlFree(sNotes);  
      unitInfo.append(notes);            
     }	

    }
	 units.insert(i,unitInfo);	
   }
  }
 }
  
 xmlFree(searchPath); 
 xmlXPathFreeObject(result);
}

void XmlReader::getChannelColors(QValueList<ChannelColors>& list)const{
 xmlXPathObjectPtr result;
 xmlChar* searchPath = xmlCharStrdup("//" + CHANNELS + "/" + CHANNEL_COLORS);

 //Evaluate xpath expression
 result = xmlXPathEvalExpression(searchPath,xpathContex);
 if(result != NULL){
  xmlNodeSetPtr nodeset = result->nodesetval;
  if(!xmlXPathNodeSetIsEmpty(nodeset)){
   //loop on all the CHANNEL_COLORS.
   int nbChannels = nodeset->nodeNr;
   for(int i = 0; i < nbChannels; ++i){
    ChannelColors channelColors;
    xmlNodePtr child;
    for(child = nodeset->nodeTab[i]->children;child != NULL;child = child->next){
     //skip the carriage return (text node named text and containing /n)
     if(child->type == XML_TEXT_NODE) continue;
      
     if(QString((char*)child->name) == CHANNEL){
      xmlChar* sId = xmlNodeListGetString(doc,child->children, 1);
      int channelId = QString((char*)sId).toInt();
      xmlFree(sId);
      channelColors.setId(channelId) ;
     }
     if(QString((char*)child->name) == COLOR){
      xmlChar* sColor = xmlNodeListGetString(doc,child->children, 1);
      QString color = QString((char*)sColor);
      xmlFree(sColor);      
      channelColors.setColor(color) ;
     }
     if(QString((char*)child->name) == ANATOMY_COLOR){
      xmlChar* sColor = xmlNodeListGetString(doc,child->children, 1);
      QString color = QString((char*)sColor);
      xmlFree(sColor);
      channelColors.setGroupColor(color) ;
     }
     if(QString((char*)child->name) == SPIKE_COLOR){
      xmlChar* sColor = xmlNodeListGetString(doc,child->children, 1);
      QString color = QString((char*)sColor);
      xmlFree(sColor);
      channelColors.setSpikeGroupColor(color) ;
     }     
    }
    list.append(channelColors);
   }
  }
 }

 xmlFree(searchPath); 
 xmlXPathFreeObject(result);
}

void XmlReader::getChannelDefaultOffset(QMap<int,int>& channelDefaultOffsets)const{
 xmlXPathObjectPtr result;
 xmlChar* searchPath = xmlCharStrdup("//" + CHANNELS + "/" + CHANNEL_OFFSET);

 //Evaluate xpath expression
 result = xmlXPathEvalExpression(searchPath,xpathContex);
 if(result != NULL){
  xmlNodeSetPtr nodeset = result->nodesetval;
  if(!xmlXPathNodeSetIsEmpty(nodeset)){
   //loop on all the CHANNEL_OFFSET.
   int nbChannels = nodeset->nodeNr;
   for(int i = 0; i < nbChannels; ++i){
    int channelId = i;
    int offset = 0;
    xmlNodePtr child;
    for(child = nodeset->nodeTab[i]->children;child != NULL;child = child->next){
     //skip the carriage return (text node named text and containing /n)
     if(child->type == XML_TEXT_NODE) continue;
      
     if(QString((char*)child->name) == CHANNEL){
      xmlChar* sId = xmlNodeListGetString(doc,child->children, 1);
      channelId = QString((char*)sId).toInt();
      xmlFree(sId);
     }
     if(QString((char*)child->name) == DEFAULT_OFFSET){
      xmlChar* sOffset = xmlNodeListGetString(doc,child->children, 1);
      offset =  QString((char*)sOffset).toInt();
      xmlFree(sOffset);
     }
     //the channels must be numbered continuously from 0.
     if(channelId < nbChannels) channelDefaultOffsets.insert(channelId,offset);
    }
   }
  }
 }

 xmlFree(searchPath); 
 xmlXPathFreeObject(result);
}

void XmlReader::getGeneralInformation(GeneralInformation& generalInformation)const{
 xmlXPathObjectPtr result;
 xmlChar* searchPath = xmlCharStrdup("//" + GENERAL);
 
 //Evaluate xpath expression
 result = xmlXPathEvalExpression(searchPath,xpathContex);
 if(result != NULL){
  xmlNodeSetPtr nodeset = result->nodesetval;
  if(!xmlXPathNodeSetIsEmpty(nodeset)){
   xmlNodePtr child;
   for(child = nodeset->nodeTab[0]->children;child != NULL;child = child->next){
    //skip the carriage return (text node named text and containing /n)
    if(child->type == XML_TEXT_NODE) continue;
     
    if(QString((char*)child->name) == DATE){
     xmlChar* sDate = xmlNodeListGetString(doc,child->children, 1);
     QString date = QString((char*)sDate);
     xmlFree(sDate);
     if(date != "") generalInformation.setDate(QDate::fromString(date,Qt::ISODate));     
    }
    if(QString((char*)child->name) == EXPERIMENTERS){
     xmlChar* sExperimenters = xmlNodeListGetString(doc,child->children, 1);
     QString experimenters = QString((char*)sExperimenters);
     xmlFree(sExperimenters);
     generalInformation.setExperimenters(experimenters); 
    }
    if(QString((char*)child->name) == DESCRIPTION){
     xmlChar* sDescription = xmlNodeListGetString(doc,child->children, 1);
     //QString description = QString((char*)sDescription);
     QString description = QString::fromUtf8((char*)sDescription);
     xmlFree(sDescription);
     generalInformation.setDescription(description); 
    }  
    if(QString((char*)child->name) == NOTES){
     xmlChar* sNotes = xmlNodeListGetString(doc,child->children, 1);
     QString notes = QString((char*)sNotes);
     xmlFree(sNotes);
     generalInformation.setNotes(notes); 
    }    
   }
  }
 }  
 
 xmlFree(searchPath); 
 xmlXPathFreeObject(result);
}


 
void XmlReader::getVideoInfo(QMap<QString,double>& videoInformation)const{
 //videoInformation will contain the sampling rate of the video recording system, the width and the heigth of the image.
 xmlXPathObjectPtr result;
 xmlChar* searchPath = xmlCharStrdup("/" + PARAMETERS + "/" + VIDEO );
 
 //Evaluate xpath expression
 result = xmlXPathEvalExpression(searchPath,xpathContex);
 if(result != NULL){
  xmlNodeSetPtr nodeset = result->nodesetval;
  if(!xmlXPathNodeSetIsEmpty(nodeset)){
   xmlNodePtr child;
   for(child = nodeset->nodeTab[0]->children;child != NULL;child = child->next){
    //skip the carriage return (text node named text and containing /n)
    if(child->type == XML_TEXT_NODE) continue;
     
    if(QString((char*)child->name) == SAMPLING_RATE){
     xmlChar* sSamplingRate = xmlNodeListGetString(doc,child->children, 1);
     double samplingRate = QString((char*)sSamplingRate).toDouble();
     xmlFree(sSamplingRate);
     videoInformation.insert(SAMPLING_RATE,samplingRate); 
    }
    if(QString((char*)child->name) == WIDTH){
     xmlChar* sWidth = xmlNodeListGetString(doc,child->children, 1);
     int width = QString((char*)sWidth).toInt();
     xmlFree(sWidth);
     videoInformation.insert(WIDTH,width); 
    }
    if(QString((char*)child->name) == HEIGHT){
     xmlChar* sHeight = xmlNodeListGetString(doc,child->children, 1);
     int height = QString((char*)sHeight).toInt();
     xmlFree(sHeight);
     videoInformation.insert(HEIGHT,height); 
    }  
   }
  }
 }  
 
 xmlFree(searchPath); 
 xmlXPathFreeObject(result); 
}
 
 double XmlReader::getLfpInformation()const{
  double lfpSamplingRate = 0;
  xmlXPathObjectPtr result;
  xmlChar* searchPath = xmlCharStrdup("//" + FIELD_POTENTIALS + "/" + LFP_SAMPLING_RATE);
  
  //Evaluate xpath expression
  result = xmlXPathEvalExpression(searchPath,xpathContex);
  if(result != NULL){
   xmlNodeSetPtr nodeset = result->nodesetval;
   if(!xmlXPathNodeSetIsEmpty(nodeset)){
    //Should be only one lfpSamplingRate element, so take the first one.
    xmlChar* sLfpSamplingRate = xmlNodeListGetString(doc,nodeset->nodeTab[0]->children, 1);
    lfpSamplingRate = QString((char*)sLfpSamplingRate).toDouble();
    xmlFree(sLfpSamplingRate);
   }
  }
  
  xmlFree(searchPath); 
  xmlXPathFreeObject(result);
  return lfpSamplingRate;
 }
 
 float XmlReader::getScreenGain() const{
  float gain = 0;
  xmlXPathObjectPtr result;
  xmlChar* searchPath = xmlCharStrdup("//" + MISCELLANEOUS + "/" + SCREENGAIN);
 
  //Evaluate xpath expression
  result = xmlXPathEvalExpression(searchPath,xpathContex);
  if(result != NULL){
   xmlNodeSetPtr nodeset = result->nodesetval;
   if(!xmlXPathNodeSetIsEmpty(nodeset)){
    //Should be only one gain element, so take the first one.
    xmlChar* sGain = xmlNodeListGetString(doc,nodeset->nodeTab[0]->children, 1);
    gain = QString((char*)sGain).toFloat();
    xmlFree(sGain);
   }
  }
 
  xmlFree(searchPath); 
  xmlXPathFreeObject(result);
  return gain;  
 }
 
QString XmlReader::getTraceBackgroundImage() const{
  QString traceBackgroundImage = 0;
  xmlXPathObjectPtr result;
  xmlChar* searchPath = xmlCharStrdup("//" + MISCELLANEOUS + "/" + TRACE_BACKGROUND_IMAGE);
 
  //Evaluate xpath expression
  result = xmlXPathEvalExpression(searchPath,xpathContex);
  if(result != NULL){
   xmlNodeSetPtr nodeset = result->nodesetval;
   if(!xmlXPathNodeSetIsEmpty(nodeset)){
    //Should be only one traceBackgroundImage element, so take the first one.
    xmlChar* straceBackgroundImage = xmlNodeListGetString(doc,nodeset->nodeTab[0]->children, 1);
    traceBackgroundImage = QString((char*)straceBackgroundImage);
    xmlFree(straceBackgroundImage);
   }
  }
 
  xmlFree(searchPath); 
  xmlXPathFreeObject(result);
  return traceBackgroundImage;  
 } 
 
 int XmlReader::getNbSamples() const{
  int nbSamples = 0;
  xmlXPathObjectPtr result;
  xmlChar* searchPath = xmlCharStrdup("//" + NEUROSCOPE + "/" + SPIKES + "/" + NB_SAMPLES);
  
  //Evaluate xpath expression
  result = xmlXPathEvalExpression(searchPath,xpathContex);
  if(result != NULL){
   xmlNodeSetPtr nodeset = result->nodesetval;
   if(!xmlXPathNodeSetIsEmpty(nodeset)){
    //Should be only one nbSamples element, so take the first one.
    xmlChar* sNbSamples = xmlNodeListGetString(doc,nodeset->nodeTab[0]->children, 1);
    nbSamples = QString((char*)sNbSamples).toInt();
    xmlFree(sNbSamples);
   }
  }
 
  xmlFree(searchPath); 
  xmlXPathFreeObject(result);
  return nbSamples;
 }

 int XmlReader::getPeakSampleIndex()const{
  int index = 0;
  xmlXPathObjectPtr result;
  xmlChar* searchPath = xmlCharStrdup("//" + NEUROSCOPE + "/" + SPIKES + "/" + PEAK_SAMPLE_INDEX);
 
  //Evaluate xpath expression
  result = xmlXPathEvalExpression(searchPath,xpathContex);
  if(result != NULL){
   xmlNodeSetPtr nodeset = result->nodesetval;
   if(!xmlXPathNodeSetIsEmpty(nodeset)){
    //Should be only one index element, so take the first one.
    xmlChar* sindex = xmlNodeListGetString(doc,nodeset->nodeTab[0]->children, 1);
    index = QString((char*)sindex).toInt();
    xmlFree(sindex);
   }
  }
 
  xmlFree(searchPath); 
  xmlXPathFreeObject(result);
  return index;  
 }
 
 void XmlReader::getNeuroscopeVideoInfo(NeuroscopeVideoInfo& videoInfo) const{
  xmlXPathObjectPtr result;
  xmlChar* searchPath = xmlCharStrdup("//" + NEUROSCOPE + "/" + VIDEO);

 //Evaluate xpath expression
 result = xmlXPathEvalExpression(searchPath,xpathContex);
 if(result != NULL){
  xmlNodeSetPtr nodeset = result->nodesetval;
  if(!xmlXPathNodeSetIsEmpty(nodeset)){
   xmlNodePtr child;
   for(child = nodeset->nodeTab[0]->children;child != NULL;child = child->next){
    //skip the carriage return (text node named text and containing /n)
    if(child->type == XML_TEXT_NODE) continue;
     
    if(QString((char*)child->name) == ROTATE){
     xmlChar* sAngle = xmlNodeListGetString(doc,child->children, 1);
     int angle = QString((char*)sAngle).toInt();
     xmlFree(sAngle);
     videoInfo.setRotation(angle);
    }
    if(QString((char*)child->name) == FLIP){
     xmlChar* sOrientation = xmlNodeListGetString(doc,child->children, 1);
     int orientation = QString((char*)sOrientation).toInt();
     xmlFree(sOrientation);
     videoInfo.setFlip(orientation);
    }
    if(QString((char*)child->name) == VIDEO_IMAGE){
     xmlChar* sBackgroundPath = xmlNodeListGetString(doc,child->children, 1);
     QString backgroundPath = QString((char*)sBackgroundPath);
     xmlFree(sBackgroundPath);
     videoInfo.setBackgroundImage(backgroundPath);
    }  
    if(QString((char*)child->name) == POSITIONS_BACKGROUND){
     xmlChar* sTrajectory = xmlNodeListGetString(doc,child->children, 1);
     int drawTrajectory = QString((char*)sTrajectory).toInt();
     xmlFree(sTrajectory);
     videoInfo.setTrajectory(drawTrajectory);
    }   
   }
  }
 }  
 
 xmlFree(searchPath); 
 xmlXPathFreeObject(result); 
 }
 
 void XmlReader::getFilesInformation(QValueList<FileInformation>& files)const{
  xmlXPathObjectPtr result;
  xmlChar* searchPath = xmlCharStrdup("//" + FILES + "/" + ndmanager::FILE);

  //Evaluate xpath expression
  result = xmlXPathEvalExpression(searchPath,xpathContex);
  if(result != NULL){
   xmlNodeSetPtr nodeset = result->nodesetval;
   if(!xmlXPathNodeSetIsEmpty(nodeset)){
    //loop on all the child.
    int nbFiles = nodeset->nodeNr;   
    for(int i = 0; i < nbFiles; ++i){
     xmlNodePtr child;
     FileInformation fileInformation;
     for(child = nodeset->nodeTab[i]->children;child != NULL;child = child->next){
      //skip the carriage return (text node named text and containing /n)
      if(child->type == XML_TEXT_NODE) continue;
 
      if(QString((char*)child->name) == EXTENSION){
       xmlChar* sExtendion = xmlNodeListGetString(doc,child->children, 1);
       QString extension = QString((char*)sExtendion);
       xmlFree(sExtendion);
       fileInformation.setExtension(extension);
      }
      if(QString((char*)child->name) == SAMPLING_RATE){
       xmlChar* sSamplingRate = xmlNodeListGetString(doc,child->children, 1);
       double samplingRate = QString((char*)sSamplingRate).toDouble();             
       xmlFree(sSamplingRate);
       fileInformation.setSamplingRate(samplingRate);
      }
      if(QString((char*)child->name) == CHANNEL_MAPPING){
       QMap<int, QValueList<int> > mapping;
       //loop on the ORIGINAL_CHANNELS which contain a list of channels
       xmlNodePtr originalChannels;
       int channelId = 0;
       for(originalChannels = child->children;originalChannels != NULL;originalChannels = originalChannels->next){
        //skip the carriage return (text node named text and containing /n)
        if(originalChannels->type == XML_TEXT_NODE) continue;
       
        if(QString((char*)originalChannels->name) == ORIGINAL_CHANNELS){
         QValueList<int> channelList;
         xmlNodePtr channels;
         for(channels = originalChannels->children;channels != NULL;channels = channels->next){
          //skip the carriage return (text node named text and containing /n)
          if(channels->type == XML_TEXT_NODE) continue;
         
          if(QString((char*)channels->name) == CHANNEL){  
           xmlChar* sId = xmlNodeListGetString(doc,channels->children, 1);
           int channelId = QString((char*)sId).toInt();
           xmlFree(sId);
           channelList.append(channelId);
          }   
         }
         if(channelList.size() != 0) mapping.insert(channelId,channelList);
         channelId++;
        }
       }
       fileInformation.setChannelMapping(mapping);
      }      
     }
     files.append(fileInformation);
    }
   }
  }
 
  xmlFree(searchPath); 
  xmlXPathFreeObject(result); 
 }
  
 void XmlReader::getProgramsInformation(QValueList<ProgramInformation>& programs) const{
  xmlXPathObjectPtr result;
  xmlChar* searchPath = xmlCharStrdup("//" + PROGRAMS + "/" + PROGRAM);

  //Evaluate xpath expression
  result = xmlXPathEvalExpression(searchPath,xpathContex);
  if(result != NULL){
   xmlNodeSetPtr nodeset = result->nodesetval;
   if(!xmlXPathNodeSetIsEmpty(nodeset)){
    //loop on all the child.
    int nbPrograms = nodeset->nodeNr;   
    for(int i = 0; i < nbPrograms; ++i){
     xmlNodePtr child;
     ProgramInformation programInformation;
     QMap<int, QValueList<QString> > parameters;
     int parameterId = 0;
     for(child = nodeset->nodeTab[i]->children;child != NULL;child = child->next){
      //skip the carriage return (text node named text and containing /n)
      if(child->type == XML_TEXT_NODE) continue;
       
      if(QString((char*)child->name) == NAME){
        xmlChar* sName = xmlNodeListGetString(doc,child->children, 1);
        QString name = QString((char*)sName);
        xmlFree(sName);
        programInformation.setProgramName(name);
      }

      if(QString((char*)child->name) == HELP){
        xmlChar* sHelp = xmlNodeListGetString(doc,child->children, 1);
        QString help = QString((char*)sHelp);
        xmlFree(sHelp);
        programInformation.setHelp(help);
      }
      
      if(QString((char*)child->name) == PARAMETERS){
       //loop on the children of the parameters tag: parameter
       xmlNodePtr parametersNode;
       for(parametersNode = child->children;parametersNode != NULL;parametersNode = parametersNode->next){
        //skip the carriage return (text node named text and containing /n)
        if(child->type == XML_TEXT_NODE) continue;
      
        if(QString((char*)parametersNode->name) == PARAMETER){
         QValueList<QString> parameterInfo;
         //loop on the children of the parameter tag: name, status and value
         xmlNodePtr parameter;
         for(parameter = parametersNode->children;parameter != NULL;parameter = parameter->next){
          //skip the carriage return (text node named text and containing /n)
          if(parameter->type == XML_TEXT_NODE) continue;
         
          if(QString((char*)parameter->name) == NAME){
           xmlChar* sName = xmlNodeListGetString(doc,parameter->children, 1);
           QString name = QString((char*)sName);
           xmlFree(sName);
           parameterInfo.prepend(name);
          }
          
          if(QString((char*)parameter->name) == STATUS){
           xmlChar* sStatus = xmlNodeListGetString(doc,parameter->children, 1);
           QString status = QString((char*)sStatus);
           xmlFree(sStatus);
           parameterInfo.append(status);
          }
  
          if(QString((char*)parameter->name) == VALUE){
           xmlChar* sValue = xmlNodeListGetString(doc,parameter->children, 1);
           QString value = QString((char*)sValue);
           xmlFree(sValue);
           if(parameterInfo.size() == 1) parameterInfo.append(value);
           else{
            QValueList<QString>::iterator it = parameterInfo.begin();
            parameterInfo.insert(++it,value);
           }
          }
         }                
         if(parameterInfo.size() != 0) parameters.insert(parameterId,parameterInfo);
         parameterId++;
        }
       }      
      }
     }
     if(parameters.size() != 0) programInformation.setParameterInformation(parameters);
     if(programInformation.getProgramName() != "") programs.append(programInformation);
    }
   }
  }
 
  xmlFree(searchPath); 
  xmlXPathFreeObject(result);   
 }
 
 
void XmlReader::getProgramInformation(ProgramInformation& programInformation) const{
  xmlXPathObjectPtr result;
  xmlChar* searchPath = xmlCharStrdup("//" + PROGRAM);

  //Evaluate xpath expression
  result = xmlXPathEvalExpression(searchPath,xpathContex);
  if(result != NULL){
   xmlNodeSetPtr nodeset = result->nodesetval;
   if(!xmlXPathNodeSetIsEmpty(nodeset)){   
    QMap<int, QValueList<QString> > parameters;
    int parameterId = 0;
    xmlNodePtr child;
    //There should be only one child, so take the first one.
    for(child = nodeset->nodeTab[0]->children;child != NULL;child = child->next){
     //skip the carriage return (text node named text and containing /n)
     if(child->type == XML_TEXT_NODE) continue;
      
     if(QString((char*)child->name) == NAME){
       xmlChar* sName = xmlNodeListGetString(doc,child->children, 1);
       QString name = QString((char*)sName);
       xmlFree(sName);
       programInformation.setProgramName(name);
     }

     if(QString((char*)child->name) == HELP){
       xmlChar* sHelp = xmlNodeListGetString(doc,child->children, 1);
       QString help = QString((char*)sHelp);
       xmlFree(sHelp);
       programInformation.setHelp(help);
     }
               
     if(QString((char*)child->name) == PARAMETERS){     
      //loop on the children of the parameters tag: parameter
      xmlNodePtr parametersNode;
      for(parametersNode = child->children;parametersNode != NULL;parametersNode = parametersNode->next){
       //skip the carriage return (text node named text and containing /n)
       if(child->type == XML_TEXT_NODE) continue;
     
       if(QString((char*)parametersNode->name) == PARAMETER){
        QValueList<QString> parameterInfo;
        //loop on the children of the parameter tag: name, status and value
        xmlNodePtr parameter;
        for(parameter = parametersNode->children;parameter != NULL;parameter = parameter->next){
         //skip the carriage return (text node named text and containing /n)
         if(parameter->type == XML_TEXT_NODE) continue;
        
         if(QString((char*)parameter->name) == NAME){
          xmlChar* sName = xmlNodeListGetString(doc,parameter->children, 1);
          QString name = QString((char*)sName);
           xmlFree(sName);
          parameterInfo.prepend(name);
         }
         
         if(QString((char*)parameter->name) == STATUS){
          xmlChar* sStatus = xmlNodeListGetString(doc,parameter->children, 1);
          QString status = QString((char*)sStatus);
          xmlFree(sStatus);
          parameterInfo.append(status);
         }
 
         if(QString((char*)parameter->name) == VALUE){
          xmlChar* sValue = xmlNodeListGetString(doc,parameter->children, 1);
          QString value = QString((char*)sValue);
          xmlFree(sValue);
          if(parameterInfo.size() == 1) parameterInfo.append(value);
          else{
           QValueList<QString>::iterator it = parameterInfo.begin();
           parameterInfo.insert(++it,value);
          }
         }       
        }                
        if(parameterInfo.size() != 0) parameters.insert(parameterId,parameterInfo);
        parameterId++;
       }
      }      
     }
    }
    if(parameters.size() != 0) programInformation.setParameterInformation(parameters);
   }
  }
 
  xmlFree(searchPath); 
  xmlXPathFreeObject(result);   
 }
