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

// include files for QT
#include <qlayout.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qcheckbox.h>
#include <q3header.h>
#include <qpixmap.h>
#include <qobject.h>
#include <qstringlist.h>
#include <q3table.h>
//Added by qt3to4:
#include <Q3TextStream>
#include <Q3ValueList>
#include <Q3PtrList>
#include <Q3Frame>
#include <Q3VBoxLayout>

// include files for KDE

#include <kiconloader.h>    // for KIconLoader
#include <kglobal.h>        // for KGlobal
#include <kstandarddirs.h>
#include <kate/view.h>
#include <kate/document.h>

//include files for the application
#include "parameterview.h"
#include "ndmanagerdoc.h"
#include "ndmanager.h"
#include "tags.h"
#include "xmlreader.h"

//General C++ include files
#include <iostream>
using namespace std;

using namespace ndmanager;

const QString ParameterView::DEFAULT_COLOR = "#0080ff";

ParameterView::ParameterView(ndManager*,ndManagerDoc& doc,QWidget* parent, const char*, bool expertMode): KJanusWidget(parent, tr("Parameter View"),TreeList),doc(doc),counter(0),programsModified(false),programId(0),expertMode(expertMode){

 //Showw the icones next to the name in the list
 setShowIconsInTreeList(true);

 //adding page "General information"
 Q3Frame* frame = addPage(tr("General"), tr("General information"),
     KGlobal::iconLoader()->loadIcon("kfm",KIcon::Panel,0,false) );
 Q3VBoxLayout* frameLayout = new Q3VBoxLayout(frame,0,0);
 generalInfo = new GeneralInfoPage(frame);
 frameLayout->addWidget(generalInfo);

 //adding page "Acquisition System"
 frame = addPage(tr("Acquisition System"), tr("Acquisition System"),
     KGlobal::iconLoader()->loadIcon("acquisition",KIcon::User,0,false) );
 frameLayout = new Q3VBoxLayout(frame,0,0);
 acquisitionSystem = new AcquisitionSystemPage(frame);
 frameLayout->addWidget(acquisitionSystem);

 //adding page "Video"
 frame = addPage(tr("Video"), tr("Video"),
     KGlobal::iconLoader()->loadIcon("video",KIcon::User,0,false) );
 frameLayout = new Q3VBoxLayout(frame,0,0);
 video = new VideoPage(frame);
 frameLayout->addWidget(video);

//adding page "Local Field Potentials "
 frame = addPage(tr("Local Field Potentials"), tr("Local Field Potentials"),
     KGlobal::iconLoader()->loadIcon("lfp",KIcon::User,0,false) );
 frameLayout = new Q3VBoxLayout(frame,0,0);
 lfp = new LfpPage(frame);
 frameLayout->addWidget(lfp);


  //adding page "Files"
  //This page is added only if the expert mode is set. The page is always created to keep track of the file information
 if(expertMode){
  frame = addPage(tr("Files"), tr("Other Files"),
     KGlobal::iconLoader()->loadIcon("files",KIcon::User,0,false));
  frameLayout = new Q3VBoxLayout(frame,0,0);
  files = new FilesPage(frame);
  frameLayout->addWidget(files);
 }
 else{
   files = new FilesPage();
 }

  //adding page "Anatomical Groups"
 //This page is added only if the expert mode is set. The page is always created to keep track of the file information
 if(expertMode){
  frame = addPage(tr("Anatomical Groups"), tr("Anatomical Groups"),
      KGlobal::iconLoader()->loadIcon("anatomy",KIcon::User,0,false) );
  frameLayout = new Q3VBoxLayout(frame,0,0);
  anatomy = new AnatomyPage(frame);
  frameLayout->addWidget(anatomy);
 }
 else{
   anatomy = new AnatomyPage();
 }

  //adding page "Spike Groups"
  //This page is added only if the expert mode is set. The page is always created to keep track of the file information
  if(expertMode){
   frame = addPage(tr("Spike Groups"), tr("Spike Groups"),
       KGlobal::iconLoader()->loadIcon("spikes",KIcon::User,0,false) );
   frameLayout = new Q3VBoxLayout(frame,0,0);
   spike = new SpikePage(frame);
   frameLayout->addWidget(spike);
  }
 else{
   spike = new SpikePage();
 }

  //adding page "Unit List"
  frame = addPage(tr("Units"), tr("Units"),
      KGlobal::iconLoader()->loadIcon("units",KIcon::User,0,false) );
  frameLayout = new Q3VBoxLayout(frame,0,0);
  unitList = new UnitListPage(frame);
  frameLayout->addWidget(unitList);

 //adding page "Neuroscope"
 frame = addPage(tr("Neuroscope"), tr("Neuroscope"),
     KGlobal::iconLoader()->loadIcon("neuroscope",KIcon::User,0,false) );
 frameLayout = new Q3VBoxLayout(frame,0,0);

 QTabWidget* tabWidget = new QTabWidget(frame);
 frameLayout->addWidget(tabWidget);
 //adding "Miscellaneous" tab
 miscellaneous = new MiscellaneousPage();
 tabWidget->addTab(miscellaneous,tr("Miscellaneous"));
 //adding "Video" tab
 neuroscopeVideo = new NeuroscopeVideoPage();
 tabWidget->addTab(neuroscopeVideo,tr("Video"));
 //adding "Clusters" tab
 clusters = new ClustersPage();
 tabWidget->addTab(clusters,tr("Clusters"));

  //adding "Channel color" tab
  channelColors = new ChannelColorsPage();
  //This tab is added only if the expert mode is set. The page is always created to keep track of the file information
  if(expertMode) tabWidget->addTab(channelColors,tr("Channel Colors"));
  //adding "Channel offeset" tab
  channelDefaultOffsets = new ChannelOffsetsPage();
  //This tab is added only if the expert mode is set. The page is always created to keep track of the file information
  if(expertMode)tabWidget->addTab(channelDefaultOffsets,tr("Channel Offsets"));

 //adding page "Programs"
 programsFrame = addPage(tr("Scripts"), tr("Scripts"),
     KGlobal::iconLoader()->loadIcon("programs",KIcon::User,0,false) );
 frameLayout = new Q3VBoxLayout(programsFrame,0,0);
 programs = new ProgramsPage(expertMode,programsFrame);
 frameLayout->addWidget(programs);

 //set connections
 connect(acquisitionSystem,SIGNAL(nbChannelsModified(int)),this,SLOT(nbChannelsModified(int)));
 connect(programs,SIGNAL(addNewProgram()),this,SLOT(addNewProgram()));
 connect(programs,SIGNAL(programToLoad(QString)),this,SLOT(loadProgram(QString)));
 connect(this,SIGNAL(aboutToShowPage(QWidget *)),this,SLOT(pageWillBeShown(QWidget *)));
 connect(spike,SIGNAL(nbGroupsModified(int)),this,SLOT(nbSpikeGroupsModified(int)));
 connect(files,SIGNAL(fileModification(Q3ValueList<QString>)),this,SLOT(fileModification(Q3ValueList<QString>)));




 connect(this,SIGNAL(resetModificationStatus()),generalInfo,SLOT(resetModificationStatus()));
 connect(this,SIGNAL(resetModificationStatus()),acquisitionSystem,SLOT(resetModificationStatus()));
 connect(this,SIGNAL(resetModificationStatus()),video,SLOT(resetModificationStatus()));
 connect(this,SIGNAL(resetModificationStatus()),lfp,SLOT(resetModificationStatus()));
 connect(this,SIGNAL(resetModificationStatus()),miscellaneous,SLOT(resetModificationStatus()));
 connect(this,SIGNAL(resetModificationStatus()),neuroscopeVideo,SLOT(resetModificationStatus()));
 connect(this,SIGNAL(resetModificationStatus()),clusters,SLOT(resetModificationStatus()));
  connect(this,SIGNAL(resetModificationStatus()),unitList,SLOT(resetModificationStatus()));


 if(expertMode){
  connect(this,SIGNAL(resetModificationStatus()),files,SLOT(resetModificationStatus()));
  connect(this,SIGNAL(resetModificationStatus()),anatomy,SLOT(resetModificationStatus()));
  connect(this,SIGNAL(resetModificationStatus()),spike,SLOT(resetModificationStatus()));
  connect(this,SIGNAL(resetModificationStatus()),channelDefaultOffsets,SLOT(resetModificationStatus()));
 }
}

ParameterView::~ParameterView(){}

void ParameterView::addNewProgram(){
 counter++;
 programId++;
 QString programName = QString("New Script-%1").arg(programId);
 ProgramPage* program = addProgram(programName);
 program->initialisationOver();
 emit scriptListHasBeenModified(programNames);
}

ProgramPage* ParameterView::addProgram(QString programName){
 ProgramPage* program = addProgram(programName,true);
 return program;
}

ProgramPage* ParameterView::addProgram(QString programName,bool show){
 QStringList programPath;
 programPath.append("Scripts");
 programPath.append(programName);

 Q3Frame* frame = addPage(programPath);
 Q3VBoxLayout* frameLayout = new Q3VBoxLayout(frame,0,0);
 ProgramPage* program = new ProgramPage(expertMode,frame,programName);
 frameLayout->addWidget(program);

 programDict.insert(programName,program);
 programNames.append(programName);

//set the parameterPage program name
 ParameterPage* parameterPage = program->getParameterPage();
 parameterPage->setProgramName(programName);

  //set connections
 connect(program,SIGNAL(programNameChanged(ProgramPage*,const QString&,QString,QString)),this,SLOT(changeProgramName(ProgramPage*,const QString&,QString,QString)));
 connect(program,SIGNAL(programToRemove(ProgramPage*)),this,SLOT(removeProgram(ProgramPage*)));
 connect(program,SIGNAL(scriptShown(Kate::View*)),this,SLOT(scriptShown(Kate::View*)));
 connect(program,SIGNAL(scriptHidden()),this,SLOT(scriptHidden()));

 //Show the new page
 if(show) showPage(pageIndex(frame));

 return program;
}

void ParameterView::changeProgramName(ProgramPage* programPage,const QString& newName,QString message,QString title){

 QString oldName = programPage->name();
 if(newName == oldName) return;

 if(programNames.contains(newName)){
  //set back the old name
  ParameterPage* parameterPage = programPage->getParameterPage();
  parameterPage->setProgramName(oldName);
  //this method will be call anyway but by calling it now a first time, we ensure that the text display in the script tab in consistent with the program (the old program which is kept)
  programPage->nameChanged(oldName);

  QString currentMessage =  QString("There is already a script with the name %1.").arg(newName);
  KMessageBox::error (this,tr(currentMessage), tr("script name conflict"));

  return;
 }

 Q3Frame* programFrame;

 //To change the name in the treeview, this one has to be rebuilt
 Q3ValueList<QString>::iterator iterator;
 for(iterator = programNames.begin(); iterator != programNames.end(); ++iterator){
  QString name = *iterator;

  ProgramPage* program = programDict[name];
  Q3Frame* parentFrame = static_cast<Q3Frame*>(program->parent());
  removePage(parentFrame);

  bool isTobeModified = false;
  if(name == oldName){
   programNames.insert(iterator,newName);
   program->setName(newName);
   name = newName;
   isTobeModified = true;
  }

  QStringList programPath;
  programPath.append("Scripts");
  programPath.append(name);

  Q3Frame* frame = addPage(programPath);
  Q3VBoxLayout* frameLayout = new Q3VBoxLayout(frame,0,0);
  program->reparent(frame,QPoint(0,0));
  frameLayout->addWidget(program);

  delete parentFrame;
  if(isTobeModified) programFrame = frame;
 }

 //remove the old name from the programNames and programDict
 programNames.remove(oldName);
 programDict.remove(oldName);
 programDict.insert(newName,programPage);

 //Show the page
 showPage(pageIndex(programFrame));

 //If the message if not empty show a message box with it
 if(message !="") KMessageBox::error (this,tr(message), tr(title));

 emit scriptListHasBeenModified(programNames);
}


void ParameterView::removeProgram(ProgramPage* programPage){
 programsModified = true;
 Q3Frame* parentFrame = static_cast<Q3Frame*>(programPage->parent());
 removePage(parentFrame);
 QString name = programPage->name();
 programNames.remove(name);
 programDict.remove(name);
 delete parentFrame;

 /*if(name.contains("New Program-") || name.contains("Untitled-"))*/ counter--;
 emit scriptListHasBeenModified(programNames);

  //Show the program page
 showPage(pageIndex(programsFrame));
}

void ParameterView::initialize(QMap<int, Q3ValueList<int> >& anatomicalGroups,QMap<QString, QMap<int,QString> >& attributes,
                          QMap<int, Q3ValueList<int> >& spikeGroups,QMap<int, QMap<QString,QString> >& spikeGroupsInformation,QMap<int, Q3ValueList<QString> >& units,
                          GeneralInformation& generalInformation,QMap<QString,double>& acquisitionSystemInfo,QMap<QString,double>& videoInformation,
                          Q3ValueList<FileInformation>& fileList,Q3ValueList<ChannelColors>& channelColors,QMap<int,int>& channelDefaultOffsets,
                          NeuroscopeVideoInfo& neuroscopeVideoInfo,Q3ValueList<ProgramInformation>& programList,
                           double lfpRate,float screenGain,int nbSamples,int peakSampleIndex,QString traceBackgroundImage){

 //Initialize the general page
 generalInfo->setDate(generalInformation.getDate());
 generalInfo->setDescription(generalInformation.getDescription());
 generalInfo->setExperimenters(generalInformation.getExperimenters());
 generalInfo->setNotes(generalInformation.getNotes());
 generalInfo->initialisationOver();

 //Initialize the acquisition page
 acquisitionSystem->setAmplification(static_cast<int>(acquisitionSystemInfo[AMPLIFICATION]));
 acquisitionSystem->setOffset(static_cast<int>(acquisitionSystemInfo[OFFSET]));
 acquisitionSystem->setResolution(static_cast<int>(acquisitionSystemInfo[BITS]));
 acquisitionSystem->setSamplingRate(acquisitionSystemInfo[SAMPLING_RATE]);
 acquisitionSystem->setVoltageRange(static_cast<int>(acquisitionSystemInfo[VOLTAGE_RANGE]));
 acquisitionSystem->setNbChannels(static_cast<int>(acquisitionSystemInfo[NB_CHANNELS]));
 acquisitionSystem->initialisationOver();

 //Initialize the video page
 if(videoInformation[SAMPLING_RATE] != 0) video->setSamplingRate(videoInformation[SAMPLING_RATE]);
 if(videoInformation[WIDTH] != 0) video->setWidth(static_cast<int>(videoInformation[WIDTH]));
 if(videoInformation[HEIGHT] != 0) video->setHeight(static_cast<int>(videoInformation[HEIGHT]));
 video->initialisationOver();

 //Initialize the lfp page
 lfp->setSamplingRate(lfpRate);
 lfp->initialisationOver();

 //Initialize the files page
 Q3ValueList<FileInformation>::iterator fileIterator;
 for(fileIterator = fileList.begin(); fileIterator != fileList.end(); ++fileIterator){
  FileInformation fileInformation = static_cast<FileInformation>(*fileIterator);
  QString extension = fileInformation.getExtension();
  FilePage* filePage = files->addFile(extension);
  filePage->setExtension(extension);
  filePage->setSamplingRate(fileInformation.getSamplingRate());
  filePage->setChannelMapping(fileInformation.getChannelMapping());
  filePage->initialisationOver();
 }


 //Initialize the anatomical groups page
 anatomy->setNbChannels(static_cast<int>(acquisitionSystemInfo[NB_CHANNELS]));
 anatomy->setGroups(anatomicalGroups);
 anatomy->setAttributes(attributes);

 //Initialize the spike groups page
 spike->setGroups(spikeGroups,spikeGroupsInformation);

 //Initialize the unit list page
 unitList->setNbUnits(units.count());
 unitList->setUnits(units);

 //Initialize the NeuroScope miscellaneous page
 miscellaneous->setScreenGain(screenGain);
 miscellaneous->setTraceBackgroundImage(traceBackgroundImage);
 miscellaneous->initialisationOver();

 //Initialize the video NeuroScope page
 neuroscopeVideo->setBackgroundImage(neuroscopeVideoInfo.getBackgroundImage());
 neuroscopeVideo->setFlip(neuroscopeVideoInfo.getFlip());
 neuroscopeVideo->setRotation(neuroscopeVideoInfo.getRotation());
 neuroscopeVideo->setPositionsBackground(neuroscopeVideoInfo.getTrajectory());
 neuroscopeVideo->initialisationOver();

 //Initialize the spike NeuroScope page
 clusters->setNbSamples(nbSamples);
 clusters->setPeakIndex(peakSampleIndex);
 clusters->initialisationOver();

 //Initialize the channel color page
 this->channelColors->setNbChannels(static_cast<int>(acquisitionSystemInfo[NB_CHANNELS]));
 this->channelColors->setColors(channelColors);

 //Initialize the channel offset page
 this->channelDefaultOffsets->setNbChannels(static_cast<int>(acquisitionSystemInfo[NB_CHANNELS]));
 this->channelDefaultOffsets->setOffsets(channelDefaultOffsets);


 //Initialize the programs page
 Q3ValueList<ProgramInformation>::iterator programIterator;
 for(programIterator = programList.begin(); programIterator != programList.end(); ++programIterator){
  ProgramInformation programInformation = static_cast<ProgramInformation>(*programIterator);
  ProgramPage* programPage = addProgram(programInformation.getProgramName(),false);

  //set the parameters
  ParameterPage* parameterPage = programPage->getParameterPage();
  QString name = programInformation.getProgramName();
  parameterPage->setProgramName(name);
  //set the help
  programPage->setHelp(programInformation.getHelp());
  QMap<int, Q3ValueList<QString> > info = programInformation.getParameterInformation();
  parameterPage->setParameterInformation(info);

  if(expertMode){
   //set the script if any
   KTextEditor::Document* scriptDoc = programPage->getScript();
   Kate::View* scriptView = programPage->getScriptView();
   //find the file corresponding to the program name
   QString path = KStandardDirs::findExe(name,getenv("PATH"),true);
   if(!path.isNull()){
    QFile file(path);
    if(!file.open(QIODevice::ReadOnly)){
     QString message = tr("The file %1 is not readable.").arg(name);
     KMessageBox::error (this,message,tr("IO Error!"));
    }
    else{
     Q3TextStream stream(&file);
     QString firstLine = stream.readLine();
     int i = firstLine.find(QRegExp("^#!"));

     if(i != -1){
      scriptDoc->openURL(path);
      file.close();
      programPage->initialisationOver();
     }
     else{
      QString message =  tr("The file %1  does not appear to be a script file (a script file should begin with #!).").arg(name);
      KMessageBox::error (this,message, tr("IO Error!"));
      scriptView->getDoc()->text();
     }
    }
   }
  }
 }
}

void ParameterView::loadProgram(QString programUrl){
 QString programDecriptionName = programUrl.fileName();
 counter++;
 programId++;

 // Get the information concering the program from the file.
 XmlReader reader = XmlReader();
 reader.parseFile(programUrl);
 ProgramInformation programInformation;
 reader.getProgramInformation(programInformation);

 QString name = programInformation.getProgramName();
 //If the description file was incorrect, no name was supplied
 if(name=="") name = QString("Untitled-%1").arg(programId);

 if(programNames.contains(name)){
  QString message =  tr("The selected script %1 is already loaded. Do you want to reload it?").arg(name);
  int answer = KMessageBox::questionYesNo(this,message, tr("Script already loaded"));
  if(answer == KMessageBox::No){
   counter--;
   return;
  }
  else{
   ProgramPage* programPage = programDict[name];
   removeProgram(programPage);
  }
 }

 programsModified = true;

 ProgramPage* program = addProgram(name);

 //Set the parameters
 ParameterPage* parameterPage = program->getParameterPage();
 parameterPage->setProgramName(name);
 QMap<int, Q3ValueList<QString> > info = programInformation.getParameterInformation();
 parameterPage->setParameterInformation(info);

 //set the help
 program->setHelp(programInformation.getHelp());

 if(expertMode){
  //set the script if any
   KTextEditor::Document* scriptDoc = program->getScript();
   Kate::View* scriptView = program->getScriptView();
  //find the file corresponding to the program name
  QString path = KStandardDirs::findExe(name,getenv("PATH"),true);
  if(!path.isNull()){
   QFileInfo fileInfo(path);
   QFile file(path);
   if(!file.open(QIODevice::ReadOnly)){
    QString message = QString("The file %1 is not readable.").arg(name);
    KMessageBox::error (this,tr(message), tr("IO Error!"));
   }
   else{
    Q3TextStream stream(&file);
    QString firstLine = stream.readLine();
    int i = firstLine.find(QRegExp("^#!"));

    if(i != -1){
     scriptDoc->openURL(path);
     file.close();
    }
    else{
     QString message =  tr("The file %1  does not appear to be a script file (a script file should begin with #!).").arg(name);
     KMessageBox::error (this,message, tr("IO Error!"));
     scriptView->getDoc()->text();
    }
   }
  }
  program->initialisationOver();
 }
 emit scriptListHasBeenModified(programNames);
}

void ParameterView::nbChannelsModified(int nbChannels){
 //All the parameters which use the number of channels are reset

 //the files page
 //remove any channel mapping
 QMap<int, Q3ValueList<int> > mapping;
 Q3PtrList<FilePage> fileList;
 files->getFilePages(fileList);
 FilePage* filePage;
 for(filePage = fileList.first(); filePage!=0; filePage = fileList.next()){
  filePage->setChannelMapping(mapping);
 }

 //the anatomical groups page
 anatomy->setNbChannels(nbChannels);
 //all the channels are put in the group 1
 QMap<int, Q3ValueList<int> > anatomicalGroups;
 Q3ValueList<int> anatomicalGroupOne;
 for(int i = 0; i<nbChannels;++i) anatomicalGroupOne.append(i);
 anatomicalGroups.insert(1,anatomicalGroupOne);
 anatomy->setGroups(anatomicalGroups);
 //For the moment the attribute names and values are hard coded (there is only the skip attribut with a default at 0)
 QMap<QString, QMap<int,QString> > attributes;
 QMap<int,QString> skip;
 for(int i = 0; i<nbChannels;++i) skip.insert(i,"0");
 attributes.insert("Skip",skip);
 anatomy->setAttributes(attributes);

 //the spike groups page
 //all the channels are put in the undefined group, therefore there is not group in the map (the trash and undefined groups are not shown)
 QMap<int, Q3ValueList<int> > spikeGroups;
 //no other information is provided
 QMap<int, QMap<QString,QString> > spikeGroupsInformation;
 spike->setGroups(spikeGroups,spikeGroupsInformation);

 //the channel color page
 this->channelColors->setNbChannels(nbChannels);
 //all the channels have the same default color
 Q3ValueList<ChannelColors> channelColorsList;
 for(int i = 0; i<nbChannels;++i){
  ChannelColors channelColors;
  channelColors.setId(i);
  channelColors.setColor(DEFAULT_COLOR);
  channelColors.setGroupColor(DEFAULT_COLOR);
  channelColors.setSpikeGroupColor(DEFAULT_COLOR);
  channelColorsList.append(channelColors);
 }
 this->channelColors->setColors(channelColorsList);

}

Q3ValueList<QString> ParameterView::modifiedScripts(){
 Q3ValueList<QString> programModified;

 Q3ValueList<QString>::iterator iterator;
 for(iterator = programNames.begin(); iterator != programNames.end(); ++iterator){
  QString name = *iterator;
  ProgramPage* program = programDict[name];
  if(program->isScriptModified()) {
   programModified.append(name);
  }
 }
 return programModified;
}

Q3ValueList<QString> ParameterView::modifiedProgramDescription(){
 Q3ValueList<QString> programModified;

 Q3ValueList<QString>::iterator iterator;
 for(iterator = programNames.begin(); iterator != programNames.end(); ++iterator){
  QString name = *iterator;
  ProgramPage* program = programDict[name];
  if(program->isDescriptionModifiedAndNotSaved()) {
   programModified.append(name);
  }
 }
 return programModified;
}


bool ParameterView::isModified(){
 bool parameterModified = false;
 bool descriptionModified = false;

 Q3ValueList<QString>::iterator iterator;
 for(iterator = programNames.begin(); iterator != programNames.end(); ++iterator){
  QString name = *iterator;
  ProgramPage* program = programDict[name];
  parameterModified = program->areParametersModified();
  descriptionModified = program->isDescriptionModified();
  if(parameterModified || descriptionModified) break;
 }

 //if one of the page has been modified, return true, false otherwise
 return (programsModified || (counter != 0)||generalInfo->isModified() || acquisitionSystem->isModified() || video->isModified() || lfp->isModified()  ||  anatomy->isModified() || spike->isModified() || unitList->isModified() || miscellaneous->isModified() || neuroscopeVideo->isModified() || clusters->isModified() ||  this->channelColors->isModified() || this->channelDefaultOffsets->isModified() || files->isModified() || parameterModified || descriptionModified);

 //programs
}

void ParameterView::getInformation(QMap<int, Q3ValueList<int> >& anatomicalGroups,QMap<QString, QMap<int,QString> >& attributes,
                          QMap<int, Q3ValueList<int> >& spikeGroups,QMap<int, QMap<QString,QString> >& spikeGroupsInformation,QMap<int, Q3ValueList<QString> >& units,
                          GeneralInformation& generalInformation,QMap<QString,double>& acquisitionSystemInfo,QMap<QString,double>& videoInformation,
                          Q3ValueList<FileInformation>& files,Q3ValueList<ChannelColors>& channelColors,QMap<int,int>& channelDefaultOffsets,
                          NeuroscopeVideoInfo& neuroscopeVideoInfo,Q3ValueList<ProgramInformation>& programs,
                           double& lfpRate,float& screenGain,int& nbSamples,int& peakSampleIndex,QString& traceBackgroundImage){

 //First check if the number of channels has changed before returning the information.
 acquisitionSystem->checkNbChannels();

 //Gather the information from the different pages
 anatomy->getGroups(anatomicalGroups);
 anatomy->getAttributes(attributes);

 spike->getGroups(spikeGroups);
 spike->getGroupInformation(spikeGroupsInformation);

 unitList->getUnits(units);

 generalInformation.setDate(generalInfo->getDate());
 generalInformation.setDescription(generalInfo->getDescription());
 generalInformation.setExperimenters(generalInfo->getExperimenters());
 generalInformation.setNotes(generalInfo->getNotes());

 acquisitionSystemInfo.insert(AMPLIFICATION,static_cast<float>(acquisitionSystem->getAmplification()));
 acquisitionSystemInfo.insert(OFFSET,static_cast<float>(acquisitionSystem->getOffset()));
 acquisitionSystemInfo.insert(BITS,static_cast<float>(acquisitionSystem->getResolution()));
 acquisitionSystemInfo.insert(SAMPLING_RATE,acquisitionSystem->getSamplingRate());
 acquisitionSystemInfo.insert(VOLTAGE_RANGE,static_cast<float>(acquisitionSystem->getVoltageRange()));
 acquisitionSystemInfo.insert(NB_CHANNELS,static_cast<float>(acquisitionSystem->getNbChannels()));

 //If the width is 0 (<=> no video info has been provided), do not store the video information
 if(video->getWidth() != 0){
  videoInformation.insert(SAMPLING_RATE,video->getSamplingRate());
  videoInformation.insert(WIDTH,video->getWidth());
  videoInformation.insert(HEIGHT,video->getHeight());
 }

 lfpRate = lfp->getSamplingRate();
 screenGain = miscellaneous->getScreenGain();
 traceBackgroundImage = miscellaneous->getTraceBackgroundImage();
 nbSamples = clusters->getNbSamples();
 peakSampleIndex = clusters->getPeakIndex();
 neuroscopeVideoInfo.setBackgroundImage(neuroscopeVideo->getBackgroundImage());
 neuroscopeVideoInfo.setFlip(neuroscopeVideo->getFlip());
 neuroscopeVideoInfo.setRotation(neuroscopeVideo->getRotation());
 neuroscopeVideoInfo.setTrajectory(neuroscopeVideo->getPositionsBackground());

 this->channelColors->getColors(channelColors);
 this->channelDefaultOffsets->getOffsets(channelDefaultOffsets);

 Q3PtrList<FilePage> fileList;
 this->files->getFilePages(fileList);
 FilePage* filePage;
 for(filePage = fileList.first(); filePage!=0; filePage = fileList.next()){
  FileInformation fileInformation;
  fileInformation.setSamplingRate(filePage->getSamplingRate());
  fileInformation.setExtension(filePage->getExtension());
  QMap<int, Q3ValueList<int> > mapping = filePage->getChannelMapping();
  fileInformation.setChannelMapping(mapping);
  files.append(fileInformation);
 }


 Q3ValueList<QString>::iterator iterator;
 for(iterator = programNames.begin(); iterator != programNames.end(); ++iterator){
  QString name = *iterator;
  ProgramPage* program = programDict[name];
  ProgramInformation programInformation;
  programInformation.setProgramName(name);
  programInformation.setHelp(program->getHelp());
  ParameterPage* parameterPage = program->getParameterPage();
  QMap<int, Q3ValueList<QString> > parameterInformation = parameterPage->getParameterInformation();
  programInformation.setParameterInformation(parameterInformation);
  programs.append(programInformation);
 }
}

void ParameterView::hasBeenSave(){
 programsModified = false;

 emit resetModificationStatus();

 //This object has a track of all the programPage
 Q3ValueList<QString>::iterator iterator;
 for(iterator = programNames.begin(); iterator != programNames.end(); ++iterator){
  QString name = *iterator;
  ProgramPage* program = programDict[name];
  program->resetModificationStatus();
 }
 counter = 0; /// added by MZ
}

bool ParameterView::saveScript(QString programName){
 ProgramPage* program = programDict[programName];
 return program->saveProgramScript();
}

void ParameterView::saveProgramDescription(QString programName){
 ProgramPage* program = programDict[programName];
 program->saveProgramParameters();
}

void ParameterView::pageWillBeShown(QWidget *){
 //There are 8 main pages, everything else is an additional program
 if(activePageIndex() < 9) emit partHidden();
}

void ParameterView::scriptShown(Kate::View* scriptView){
 emit partShown(scriptView);
}

void ParameterView::scriptHidden(){
 emit partHidden();
}


#include "parameterview.moc"
