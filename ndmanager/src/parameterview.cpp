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

// include files for QT
#include <qlayout.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qcheckbox.h>
#include <qpixmap.h>
#include <qobject.h>
#include <qstringlist.h>
#include <QTextEdit>
#include <QDebug>
#include <QStackedWidget>

#include <QTextStream>
#include <QList>
#include <QFrame>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QSplitter>

//include files for the application
#include "parameterview.h"
#include "ndmanagerdoc.h"
#include "ndmanager.h"
#include "tags.h"
#include "xmlreader.h"
#include "ndmanagerutils.h"

#include "parametertree.h"

using namespace ndmanager;

const QString ParameterView::DEFAULT_COLOR = "#0080ff";

ParameterView::ParameterView(ndManager*,ndManagerDoc& doc,QWidget* parent, const char*, bool expertMode)
    : QWidget(parent)
    ,doc(doc),counter(0),programsModified(false),programId(0),expertMode(expertMode){

    QHBoxLayout *hbox = new QHBoxLayout;
    setLayout(hbox);

    QSplitter *splitter = new QSplitter;
    splitter->setChildrenCollapsible(false);
    hbox->addWidget(splitter);
    mParameterTree = new ParameterTree;
    mParameterTree->setHeaderLabels(QStringList()<<QString());
    splitter->addWidget(mParameterTree);

    mStackWidget = new QStackedWidget;
    splitter->addWidget(mStackWidget);
    connect(mParameterTree,SIGNAL(showWidgetPage(QWidget*)),mStackWidget,SLOT(setCurrentWidget(QWidget*)));

    setWindowTitle(tr("Parameter View"));

    //adding page "General information"
    generalInfo = new GeneralInfoPage;
    mStackWidget->addWidget(generalInfo);
    mParameterTree->addPage(":/icons/folder-blue",tr("General information"),generalInfo);

    //adding page "Acquisition System"
    acquisitionSystem = new AcquisitionSystemPage;
    mStackWidget->addWidget(acquisitionSystem);
    mParameterTree->addPage(":/icons/acquisition",tr("Acquisition System"),acquisitionSystem);


    //adding page "Video"
    video = new VideoPage;
    mStackWidget->addWidget(video);
    mParameterTree->addPage(":/icons/video",tr("Video"),video);

    //adding page "Local Field Potentials "
    lfp = new LfpPage;
    mStackWidget->addWidget(lfp);
    mParameterTree->addPage(":/icons/lfp",tr("Local Field Potentials"),lfp);


    //adding page "Files"
    //This page is added only if the expert mode is set. The page is always created to keep track of the file information
    if(expertMode){
        files = new FilesPage;
        mStackWidget->addWidget(files);
        mParameterTree->addPage(":/icons/files",tr("Files"),files);
    } else {
        files = new FilesPage();
    }

    //adding page "Anatomical Groups"
    //This page is added only if the expert mode is set. The page is always created to keep track of the file information
    if(expertMode){
        anatomy = new AnatomyPage;
        mStackWidget->addWidget(anatomy);
        mParameterTree->addPage(":/icons/anatomy",tr("Anatomical Groups"),anatomy);
    }
    else{
        anatomy = new AnatomyPage();
    }

    //adding page "Spike Groups"
    //This page is added only if the expert mode is set. The page is always created to keep track of the file information
    if(expertMode){
        spike = new SpikePage;
        mStackWidget->addWidget(spike);
        mParameterTree->addPage(":/icons/spikes", tr("Spike Groups"), spike);
    } else {
        spike = new SpikePage();
    }

    //adding page "Unit List"
    unitList = new UnitListPage;
    mStackWidget->addWidget(unitList);
    mParameterTree->addPage(":/icons/units", tr("Units"), unitList);

    //adding page "Neuroscope"

    QTabWidget* tabWidget = new QTabWidget;
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
    if(expertMode)
        tabWidget->addTab(channelDefaultOffsets,tr("Channel Offsets"));


    mStackWidget->addWidget(tabWidget);
    mParameterTree->addPage(":/icons/neuroscope", tr("Neuroscope"), tabWidget);


    //adding page "Programs"

    programs = new ProgramsPage(expertMode);

    mStackWidget->addWidget(programs);
    mScriptsItem = mParameterTree->addPage(":/icons/programs", tr("Plugins"), programs);

    //set connections
    connect(acquisitionSystem,SIGNAL(nbChannelsModified(int)),this,SLOT(nbChannelsModified(int)));
    connect(programs,SIGNAL(addNewProgram()),this,SLOT(addNewProgram()));
    connect(programs,SIGNAL(programToLoad(QString)),this,SLOT(loadProgram(QString)));
    connect(spike,SIGNAL(nbGroupsModified(int)),this,SLOT(nbSpikeGroupsModified(int)));
    connect(files,SIGNAL(fileModification(QStringList)),this,SLOT(fileModification(QStringList)));




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

ParameterView::~ParameterView()
{
    if (!expertMode) {
       delete files;
       delete anatomy;
       delete spike;
       delete channelColors;
       delete channelDefaultOffsets;
    }
}

void ParameterView::addNewProgram(){
    counter++;
    programId++;
    QString programName = QString::fromLatin1("New Plugin-%1").arg(programId);
    ProgramPage* program = addProgram(programName);
    program->initialisationOver();
    emit scriptListHasBeenModified(QStringList()<<programDict.keys());
}

ProgramPage* ParameterView::addProgram(const QString& programName){
    ProgramPage* program = addProgram(programName,true);
    return program;
}

ProgramPage* ParameterView::addProgram(const QString& programName,bool show){
    //adding page "Video"
    QWidget *w = new QWidget(this);
    ProgramPage* program = new ProgramPage(expertMode,w,programName);
    mStackWidget->addWidget(program);

    QTreeWidgetItem * item = mParameterTree->addSubPage(mScriptsItem,programName, program);

    ProgramPageId pageId;
    pageId.item = item;
    pageId.page = program;

    programDict.insert(programName,pageId);

    //set the parameterPage program name
    ParameterPage* parameterPage = program->getParameterPage();
    parameterPage->setProgramName(programName);

    //set connections
    connect(program,SIGNAL(programNameChanged(ProgramPage*,QString,QString,QString)),this,SLOT(changeProgramName(ProgramPage*,QString,QString,QString)));
    connect(program,SIGNAL(programToRemove(ProgramPage*)),this,SLOT(removeProgram(ProgramPage*)));
    connect(program,SIGNAL(scriptHidden()),this,SLOT(scriptHidden()));

    //Show the new page
    if(show)
        mStackWidget->setCurrentWidget(program);

    return program;
}

void ParameterView::changeProgramName(ProgramPage* programPage, const QString& newName, const QString &message, const QString &title){

    const QString oldName = programPage->objectName();
    if(newName == oldName)
        return;

    if(programDict.contains(newName)){
        //set back the old name
        ParameterPage* parameterPage = programPage->getParameterPage();
        parameterPage->setProgramName(oldName);
        //this method will be call anyway but by calling it now a first time, we ensure that the text display in the script tab in consistent with the program (the old program which is kept)
        programPage->nameChanged(oldName);

        const QString currentMessage =  tr("There is already a plugin with the name %1.").arg(newName);
        QMessageBox::critical (this, tr("plugin name conflict"),currentMessage);

        return;
    }
    ProgramPageId id = programDict[oldName];
    id.item->setText(0,newName);
    id.page->setObjectName(newName);

    programDict.remove(oldName);
    programDict.insert(newName,id);


    mStackWidget->setCurrentWidget(id.page);
    //If the message if not empty show a message box with it
    if(!message.isEmpty())
        QMessageBox::critical (this,title,message );

    emit scriptListHasBeenModified(QStringList()<<programDict.keys());
}


void ParameterView::removeProgram(ProgramPage* programPage){
    programsModified = true;

    const QString name = programPage->objectName();
    ProgramPageId id = programDict[name];
    mStackWidget->removeWidget(id.page);
    delete id.page;

    delete id.item;
    //mParameterTree->(id.item, 0);
    programDict.remove(name);

    /*if(name.contains("New Program-") || name.contains("Untitled-"))*/ counter--;
    emit scriptListHasBeenModified(QStringList()<<programDict.keys());
    //mParameterTree->setCurrentItem(mScriptsItem);
}

void ParameterView::initialize(QMap<int, QList<int> >& anatomicalGroups,QMap<QString, QMap<int,QString> >& attributes,
                               QMap<int, QList<int> >& spikeGroups,QMap<int, QMap<QString,QString> >& spikeGroupsInformation,QMap<int, QStringList >& units,
                               GeneralInformation& generalInformation,QMap<QString,double>& acquisitionSystemInfo,QMap<QString,double>& videoInformation,
                               QList<FileInformation>& fileList,QList<ChannelColors>& channelColors,QMap<int,int>& channelDefaultOffsets,
                               NeuroscopeVideoInfo& neuroscopeVideoInfo,QList<ProgramInformation>& programList,
                               double lfpRate,float screenGain,int nbSamples,int peakSampleIndex,const QString& traceBackgroundImage){

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
    if(videoInformation[SAMPLING_RATE] != 0)
        video->setSamplingRate(videoInformation[SAMPLING_RATE]);
    if(videoInformation[WIDTH] != 0)
        video->setWidth(static_cast<int>(videoInformation[WIDTH]));
    if(videoInformation[HEIGHT] != 0)
        video->setHeight(static_cast<int>(videoInformation[HEIGHT]));
    video->initialisationOver();

    //Initialize the lfp page
    lfp->setSamplingRate(lfpRate);
    lfp->initialisationOver();

    //Initialize the files page
    QList<FileInformation>::iterator fileIterator;
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
    anatomy->setModified(false);

    //Initialize the spike groups page
    spike->setGroups(spikeGroups,spikeGroupsInformation);
    spike->setModified(false);

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
    this->channelColors->setModified(false);

    //Initialize the channel offset page
    this->channelDefaultOffsets->setNbChannels(static_cast<int>(acquisitionSystemInfo[NB_CHANNELS]));
    this->channelDefaultOffsets->setOffsets(channelDefaultOffsets);
    this->channelDefaultOffsets->setModified(false);


    //Initialize the programs page
    QList<ProgramInformation>::iterator programIterator;
    for(programIterator = programList.begin(); programIterator != programList.end(); ++programIterator){
        ProgramInformation programInformation = static_cast<ProgramInformation>(*programIterator);
        ProgramPage* programPage = addProgram(programInformation.getProgramName(),false);
        //set the parameters
        ParameterPage* parameterPage = programPage->getParameterPage();
        QString name = programInformation.getProgramName();
        parameterPage->setProgramName(name);
        //set the help
        programPage->setHelp(programInformation.getHelp());
        QMap<int, QStringList > info = programInformation.getParameterInformation();
        parameterPage->setParameterInformation(info);
        if(expertMode){
            //set the script if any
            QTextEdit* scriptView = programPage->getScriptView();
            //find the file corresponding to the program name
            QString path = NdManagerUtils::findExecutable(name);
            if(!path.isNull()){
                QFile file(path);
                if(!file.open(QIODevice::ReadOnly)){
                    const QString message = tr("The file %1 is not readable.").arg(name);
                    QMessageBox::critical (this,tr("IO Error!"),message);
                } else {
                    QTextStream stream(&file);
                    QString firstLine = stream.readLine();
                    const int i = firstLine.indexOf(QRegExp("^#!"));

                    if(i != -1) {
                        scriptView->setText(stream.readAll());
                        file.close();
                    } else {
                        const QString message = tr("The file %1  does not appear to be a plugin file (a script file should begin with #!).").arg(name);
                        QMessageBox::critical (this,tr("IO Error!"),message);
                        scriptView->clear();
                    }
                }
            }
            programPage->initialisationOver();
        }
    }
}

void ParameterView::loadProgram(const QString &programUrl) {
    //QString programDecriptionName = QFileInfo(programUrl).fileName();
    counter++;
    programId++;

    // Get the information concering the program from the file.
    XmlReader reader = XmlReader();
    reader.parseFile(programUrl);
    qDebug()<<" ssssssssssssssssssssssssssssssssssssssssssss"<<programUrl;
    ProgramInformation programInformation;
    reader.getProgramInformation(programInformation);

    QString name = programInformation.getProgramName();
    qDebug()<<" name"<<name;
    //If the description file was incorrect, no name was supplied
    if(name.isEmpty())
        name = QString::fromLatin1("Untitled-%1").arg(programId);

    if(programDict.contains(name)){
        const QString message =  tr("The selected plugin %1 is already loaded. Do you want to reload it?").arg(name);
        const int answer = QMessageBox::question(this,tr("plugin already loaded"),message, QMessageBox::Yes|QMessageBox::No );
        if(answer == QMessageBox::No){
            counter--;
            return;
        } else {
            ProgramPage* programPage = programDict[name].page;
            removeProgram(programPage);
        }
    }

    programsModified = true;

    ProgramPage* program = addProgram(name);

    //Set the parameters
    ParameterPage* parameterPage = program->getParameterPage();
    parameterPage->setProgramName(name);
    QMap<int, QStringList > info = programInformation.getParameterInformation();
    parameterPage->setParameterInformation(info);

    //set the help
    program->setHelp(programInformation.getHelp());
    if(expertMode){
        //set the script if any
        QTextEdit* scriptView = program->getScriptView();
        //find the file corresponding to the program name
        const QString path = NdManagerUtils::findExecutable(name);
        if(!path.isNull()){
            QFile file(path);
            if(!file.open(QIODevice::ReadOnly)){
                const QString message = tr("The file %1 is not readable.").arg(name);
                QMessageBox::critical (this,message, tr("IO Error!"));
            }
            else{
                QTextStream stream(&file);
                QString firstLine = stream.readLine();
                const int i = firstLine.indexOf(QRegExp("^#!"));

                if(i != -1){
                    scriptView->setText(stream.readAll());
                    file.close();
                } else {
                    const QString message =  tr("The file %1  does not appear to be a plugin file (a script file should begin with #!).").arg(name);
                    QMessageBox::critical (this,tr("IO Error!"),message);
                    scriptView->clear();
                }
            }
        }
        program->initialisationOver();
    }
    emit scriptListHasBeenModified(QStringList()<<programDict.keys());
}

void ParameterView::nbChannelsModified(int nbChannels){
    //All the parameters which use the number of channels are reset

    //the files page
    //remove any channel mapping
    QMap<int, QList<int> > mapping;
    QList<FilePage*> fileList;
    files->getFilePages(fileList);
    for(int i= 0 ; i < fileList.count();++i) {
        fileList.at(i)->setChannelMapping(mapping);
    }

    //the anatomical groups page
    anatomy->setNbChannels(nbChannels);
    //all the channels are put in the group 1
    QMap<int, QList<int> > anatomicalGroups;
    QList<int> anatomicalGroupOne;
    for(int i = 0; i<nbChannels;++i) anatomicalGroupOne.append(i);
    anatomicalGroups.insert(1,anatomicalGroupOne);
    anatomy->setGroups(anatomicalGroups);
    //For the moment the attribute names and values are hard coded (there is only the skip attribut with a default at 0)
    QMap<QString, QMap<int,QString> > attributes;
    QMap<int,QString> skip;
    for(int i = 0; i<nbChannels;++i)
        skip.insert(i,"0");
    attributes.insert("Skip",skip);
    anatomy->setAttributes(attributes);

    //the spike groups page
    //all the channels are put in the undefined group, therefore there is not group in the map (the trash and undefined groups are not shown)
    QMap<int, QList<int> > spikeGroups;
    //no other information is provided
    QMap<int, QMap<QString,QString> > spikeGroupsInformation;
    spike->setGroups(spikeGroups,spikeGroupsInformation);

    //the channel color page
    this->channelColors->setNbChannels(nbChannels);
    //all the channels have the same default color
    QList<ChannelColors> channelColorsList;
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

QStringList ParameterView::modifiedScripts() const{
    QStringList programModified;

    QMapIterator<QString, ProgramPageId> i(programDict);
    while (i.hasNext())  {
        i.next();
        ProgramPage* program = i.value().page;
        if(program->isScriptModified()) {
            programModified.append(i.key());
        }
    }
    return programModified;
}

QStringList ParameterView::modifiedProgramDescription() const{
    QStringList programModified;

    QMapIterator<QString, ProgramPageId> i(programDict);
    while (i.hasNext())  {
        i.next();
        ProgramPage* program = i.value().page;
        if(program->isDescriptionModifiedAndNotSaved()) {
            programModified.append(i.key());
        }
    }
    return programModified;
}


bool ParameterView::isModified(){
    bool parameterModified = false;
    bool descriptionModified = false;

    QMapIterator<QString, ProgramPageId> i(programDict);
    while (i.hasNext())  {
        i.next();
        ProgramPage* program = i.value().page;
        parameterModified = program->areParametersModified();
        descriptionModified = program->isDescriptionModified();
        if(parameterModified || descriptionModified)
            break;
    }

    const bool p = (programsModified ||
                    (counter != 0)||
                    generalInfo->isModified() ||
                    acquisitionSystem->isModified() ||
                    video->isModified() ||
                    lfp->isModified()  ||
                    anatomy->isModified() ||
                    spike->isModified() ||
                    unitList->isModified() ||
                    miscellaneous->isModified() ||
                    neuroscopeVideo->isModified() ||
                    clusters->isModified() ||
                    this->channelColors->isModified() ||
                    this->channelDefaultOffsets->isModified() ||
                    files->isModified() ||
                    parameterModified ||
                    descriptionModified);

    return p;
}

void ParameterView::getInformation(QMap<int, QList<int> >& anatomicalGroups,QMap<QString, QMap<int,QString> >& attributes,
                                   QMap<int, QList<int> >& spikeGroups,QMap<int, QMap<QString,QString> >& spikeGroupsInformation,QMap<int, QStringList >& units,
                                   GeneralInformation& generalInformation,QMap<QString,double>& acquisitionSystemInfo,QMap<QString,double>& videoInformation,
                                   QList<FileInformation>& files,QList<ChannelColors>& channelColors,QMap<int,int>& channelDefaultOffsets,
                                   NeuroscopeVideoInfo& neuroscopeVideoInfo,QList<ProgramInformation>& programs,
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

    QList<FilePage*> fileList;
    this->files->getFilePages(fileList);
    FilePage* filePage;
    for(int i = 0; i <fileList.count(); ++i) {
        filePage = fileList.at(i);
        FileInformation fileInformation;
        fileInformation.setSamplingRate(filePage->getSamplingRate());
        fileInformation.setExtension(filePage->getExtension());
        QMap<int, QList<int> > mapping = filePage->getChannelMapping();
        fileInformation.setChannelMapping(mapping);
        files.append(fileInformation);
    }

    QMapIterator<QString, ProgramPageId> i(programDict);
    while (i.hasNext())  {
        i.next();
        const QString name = i.key();
        ProgramPage* program = i.value().page;
        ProgramInformation programInformation;
        programInformation.setProgramName(name);
        programInformation.setHelp(program->getHelp());
        ParameterPage* parameterPage = program->getParameterPage();
        QMap<int, QStringList > parameterInformation = parameterPage->getParameterInformation();
        programInformation.setParameterInformation(parameterInformation);
        programs.append(programInformation);
    }
}

void ParameterView::hasBeenSave(){
    programsModified = false;

    emit resetModificationStatus();

    //This object has a track of all the programPage
    QMapIterator<QString, ProgramPageId> i(programDict);
    while (i.hasNext())  {
        i.next();
        ProgramPage* program = i.value().page;
        program->resetModificationStatus();
    }
    counter = 0; /// added by MZ
}

bool ParameterView::saveScript(const QString& programName){
    ProgramPage* program = programDict[programName].page;
    return program->saveProgramScript();
}

void ParameterView::saveProgramDescription(const QString& programName){
    ProgramPage* program = programDict[programName].page;
    program->saveProgramParameters();
}


void ParameterView::scriptHidden(){
    emit partHidden();
}

QStringList ParameterView::getFileScriptNames()const
{
    return QStringList()<<programDict.keys();
}

void ParameterView::setCurrentPage(int index)
{
    mStackWidget->setCurrentIndex(index);
}

int ParameterView::currentPage() const
{
    return mStackWidget->currentIndex();
}
