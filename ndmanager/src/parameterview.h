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

#ifndef _NDPARAMETERVIEW_H_
#define _NDPARAMETERVIEW_H_

// include files for QT
#include <qwidget.h>
#include <qvalidator.h>
#include <QMap>
#include <qwidget.h>
#include <QList>

#include <QFrame>


//include files for the application
#include "acquisitionsystempage.h"
#include "anatomypage.h"
#include "unitlistpage.h"
#include "generalinfopage.h"
#include "lfppage.h"
#include "spikepage.h"
#include "videopage.h"
#include "filespage.h"
#include "channelcolorspage.h"
#include "clusterspage.h"
#include "miscellaneouspage.h"
#include "neuroscopevideopage.h"
#include "programspage.h"
#include "programpage.h"
#include "generalinformation.h"
#include "fileinformation.h"
#include "programinformation.h"
#include "neuroscopevideoinfo.h"
#include "channeloffsetspage.h"

class ndManager;
class ndManagerDoc;
class ParameterTree;
class QStackedWidget;
class QTreeWidgetItem;
/**
 * This is the main view class for ndManager.  Most of the non-menu,
 * non-toolbar, and non-statusbar (e.g., non frame) GUI code should go
 * here.
 *
 * @short Main view
 * @author Lynn Hazan <lynn.hazan@myrealbox.com>
 * @version 0.1
 */
class ParameterView : public QWidget
{
    Q_OBJECT
public:
    /**
  * Constructor
  * @param mainWindow a pointer on the main window of the application.
  * @param doc the document instance the view represents.
  * @param parent the parent QWidget.
  * @param name name of the widget (can be used for introspection).
  * @param expertMode true if the file is opened in expert mode, false otherwise.
  */
    explicit ParameterView(ndManager* mainWindow,ndManagerDoc& doc,QWidget *parent, const char* name, bool expertMode);

    /**
  * Destructor
  */
    ~ParameterView();

    /**Initialize the parameter pages.
  * @param anatomicalGroups map given the composition of the anatomical groups. The trash group is not included, channels which are not part of
  * any group are in the trash group.
  * @param attributes map given the correspondance between the attribute names and a map given for each channel the value of the attribute.
  * @param spikeGroups map given the composition of the spike groups. The trash and undefined groups are not included, channels which are not
  * part of any group nor the trash group (this group is common to the anatomical and spike groups) are in the undefined group.
  * @param spikeGroupsInformation map containing the additional information for each spike group except the trash and undefined groups.
  * @param units map given the composition of the unit list.
  * @param generalInformation object storing the information contained in the General Information part of the parameter file.
  * @param acquisitionSystemInfo map given the information related to the acquisition system.
  * @param videoInformation map given the information related to the video recording system.
  * @param files list of object storing the information describing an additional file (filter and/or subset of the raw data file).
  * @param channelColors object storing the channel color information.
  * @param channelDefaultOffsets map storing the channel default offset information.
  * @param neuroscopeVideoInfo object storing the video information used by NeuroScope.
  * @param programs list of object storing the information describing a program to be used to process the raw data.
  * @param lfpRate rate of the local field potential file.
  * @param screenGain screen gain in milivolts by centimeters used to display the field potentiels in NeuroScope.
  * @param nbSamples the number of samples in a spike; value used by NeuroScope.
  * @param peakSampleIndex the sample index corresponding to the peak of the spike; value use dby NeuroScope.
  * @param traceBackgroundImage background image for the trace view in NeuroScope.
  */
    void initialize(QMap<int, QList<int> >& anatomicalGroups, QMap<QString, QMap<int,QString> >& attributes,
                    QMap<int, QList<int> >& spikeGroups, QMap<int, QMap<QString,QString> >& spikeGroupsInformation, QMap<int, QStringList >& units,
                    GeneralInformation& generalInformation, QMap<QString,double>& acquisitionSystemInfo, QMap<QString,double>& videoInformation,
                    QList<FileInformation>& files, QList<ChannelColors>& channelColors, QMap<int,int>& channelDefaultOffsets,
                    NeuroscopeVideoInfo& neuroscopeVideoInfo, QList<ProgramInformation>& programs,
                    double lfpRate, float screenGain, int nbSamples, int peakSampleIndex, const QString &traceBackgroundImage);

    /**Gets the information from the parameter pages.
  * @param anatomicalGroups map given the composition of the anatomical groups. The trash group is not included, channels which are not part of
  * any group are in the trash group.
  * @param attributes map given the correspondance between the attribute names and a map given for each channel the value of the attribute.
  * @param spikeGroups map given the composition of the spike groups. The trash and undefined groups are not included, channels which are not
  * part of any group nor the trash group (this group is common to the anatomical and spike groups) are in the undefined group.
  * @param spikeGroupsInformation map containing the additional information for each spike group except the trash and undefined groups.
  * @param units map containing the composition of the unit list.
  * @param generalInformation object storing the information contained in the General Information part of the parameter file.
  * @param acquisitionSystemInfo map given the information related to the acquisition system.
  * @param videoInformation map given the information related to the video recording system.
  * @param files list of object storing the information describing an additional file (filter and/or subset of the raw data file).
  * @param channelColors object storing the channel color information.
  * @param channelDefaultOffsets map storing the channel default offset information.
  * @param neuroscopeVideoInfo object storing the video information used by NeuroScope.
  * @param programs list of object storing the information describing a program to be used to process the raw data.
  * @param lfpRate rate of the local field potential file.
  * @param screenGain screen gain in milivolts by centimeters used to display the field potentiels in NeuroScope.
  * @param nbSamples the number of samples in a spike; value used by NeuroScope.
  * @param peakSampleIndex the sample index corresponding to the peak of the spike; value use dby NeuroScope
  * @param traceBackgroundImage background image for the trace view in NeuroScope
  */
    void getInformation(QMap<int, QList<int> >& anatomicalGroups,QMap<QString, QMap<int,QString> >& attributes,
                        QMap<int, QList<int> >& spikeGroups,QMap<int, QMap<QString,QString> >& spikeGroupsInformation,QMap<int, QStringList >& units,
                        GeneralInformation& generalInformation,QMap<QString,double>& acquisitionSystemInfo,QMap<QString,double>& videoInformation,
                        QList<FileInformation>& files,QList<ChannelColors>& channelColors,QMap<int,int>& channelDefaultOffsets,
                        NeuroscopeVideoInfo& neuroscopeVideoInfo,QList<ProgramInformation>& programs,
                        double& lfpRate,float& screenGain,int& nbSamples,int& peakSampleIndex,QString& traceBackgroundImage);

    /**True if at least one parameter has been modified, false otherwise.*/
    bool isModified();

    /**Returns the list of the names of the programs for which the script is currently modified.
 * @return list of the names of the programs having their script modified.
 */
    QStringList modifiedScripts() const;

    /**Returns the list of the names of the programs for which the program description is currently modified.
 * The program description includes the program name, the number of parameters, their name and status,
 * and the help.
 * @return list of the names of the programs having their description modified.
 */
    QStringList modifiedProgramDescription() const;


    /**Warns all the pages that the current state has been saved.*/
    void hasBeenSave();

    /**Saves the script @p programName.
 * @param programName name of the script to save.
 * @return the saving status.
 */
    bool saveScript(const QString &programName);

    /**Saves the description of the program @p programName.
 * @param programName name of the program description to save.
 */
    void saveProgramDescription(const QString &programName);

    /**Returns the current number of group of spikes.
 */
    inline int getNbGroups()const{return spike->getNbGroups();}

    /**Returns a list containing all the extensions of the specific files.
 * @return extension list.
 */
    inline QStringList getFileExtensions() const {return files->getFileExtensions();}

    /**Returns a list containing all the script names.
 * @return script name list.
 */
    QStringList getFileScriptNames() const;

signals:
    /**
   * This signal is used to change the content of the statusbar;
   */
    void signalChangeStatusbar(const QString& text);

    /**This signal is used to resets the internal modification status of all the pages to false.*/
    void resetModificationStatus();

    /**This signal is used to update the menus and toolbars.*/
    void partHidden();

    /**This signal is used to update the dropdown list containing the spike groups in the managerView.*/
    void nbSpikeGroupsHasBeenModified(int nbGroups);

    /**This signal is used to update of the dropdown list containing the file extensions in the managerView.*/
    void fileHasBeenModified(const QStringList& extensions);

    /**This signal is used to update of the dropdown list containing the script names in the managerView.*/
    void scriptListHasBeenModified(const QStringList& scriptNames);

public slots:

    /**Adds a new program to the subtree under the node Program.
  */
    void addNewProgram();

    /**Loads a program to the subtree under the node Program.
  * @param programUrl url of the file describing the program to load.
  */
    void loadProgram(const QString& programUrl);

    /**Removes a program from the subtree under the node Program.
  * @param programPage the ProgramPage to remove.
  */
    void removeProgram(ProgramPage* programPage);

    /**Changes the name of the program.
  * @param programPage the ProgramPage to rename.
  * @param newName the new name of the program.
  * @param message error message to use in case of an error while loading a new script.
  * @param title title of the possible error message box.
  */
    void changeProgramName(ProgramPage* programPage,const QString& newName,const QString& message,const QString& title);

    /**Triggers the changes in the different pages due to a modification of the number of channels.
  * @param nbChannels the new number of channels.
  */
    void nbChannelsModified(int nbChannels);

    /**Triggers the update of the dropdown list containing the spike groups in the managerView.
  * @param nbGroups the new number of spike groups.
  */
    void nbSpikeGroupsModified(int nbGroups){emit nbSpikeGroupsHasBeenModified(nbGroups);}

    /**Triggers the update of the dropdown list containing the file extensions in the managerView.
  * @param extensions list containing the extensions of all the specific files.
  */
    void fileModification(const QStringList& extensions){emit fileHasBeenModified(extensions);}

    void setCurrentPage(int index);
    int currentPage() const;

private slots:

    void scriptHidden();

private:

    /**Adds a program to the subtree under the node Program.
   * @param programName name of the program to add.
   * @return a pointer on the added ProgramPage.
   */
    ProgramPage* addProgram(const QString &programName);

    /**Adds a program to the subtree under the node Program.
   * @param programName name of the program to add.
   * @param show true if the added program page has to been shown, false otherwise.
   * @return a pointer on the added ProgramPage.
   */
    ProgramPage* addProgram(const QString &programName, bool show);

    /** The document connected to the view, specified in the constructor */
    ndManagerDoc& doc;

    /**Pointer on the general information page.*/
    GeneralInfoPage* generalInfo;

    /**Pointer on the acquisition system information page.*/
    AcquisitionSystemPage* acquisitionSystem;
    
    /**Pointer on the video information page.*/
    VideoPage* video;

    /**Pointer on the local field potentials information page.*/
    LfpPage* lfp;

    /**Pointer on the page containing information for the other type of files.*/
    FilesPage* files;

    /**Pointer on the page containing information for the anatomical groups.*/
    AnatomyPage* anatomy;

    /**Pointer on the page containing information for the spike groups.*/
    SpikePage* spike;

    /**Pointer on the page containing information for the units.*/
    UnitListPage* unitList;

    /**Pointer on the page containing the miscellaneous information for NeuroScope.*/
    MiscellaneousPage* miscellaneous;

    /**Pointer on the page containing the video information for NeuroScope.*/
    NeuroscopeVideoPage* neuroscopeVideo;

    /**Pointer on the page containing the spike information for NeuroScope.*/
    ClustersPage* clusters;

    /**Pointer on the page containing the channel color information for NeuroScope.*/
    ChannelColorsPage* channelColors;

    /**Pointer on the page containing the channel default offset information for NeuroScope.*/
    ChannelOffsetsPage* channelDefaultOffsets;

    /**Pointer on the page containing information for the programs.*/
    ProgramsPage* programs;

    struct ProgramPageId {
        ProgramPageId() {
            page = 0;
            item = 0;
        }
        ProgramPage *page;
        QTreeWidgetItem *item;
    };

    /**Dictionary containing all the programs.*/
    QMap<QString,ProgramPageId> programDict;

    /**Counter given the number existing programs. */
    int counter;

    static const QString DEFAULT_COLOR;

    bool programsModified;

    /**Counter given an id for the new programs. */
    int programId;

    /**True if the file is opened in expert mode, false otherwise*/
    bool expertMode;
    QTreeWidgetItem *mScriptsItem;

    ParameterTree *mParameterTree;
    QStackedWidget *mStackWidget;

};

#endif // _NDPARAMETERVIEW_H_
