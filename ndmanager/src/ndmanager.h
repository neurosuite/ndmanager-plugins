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

#ifndef _NDMANAGER_H_
#define _NDMANAGER_H_

//include files for QT
#include <qdom.h>

#include <QList>


#include <QDockWidget>

#include <QProcess>
#include <QMainWindow>
#include <qrecentfileaction.h>
#include <QToolBar>

// application specific includes
#include "parameterview.h"
#include "ndmanagerdoc.h"
#include "generalinformation.h"
#include "fileinformation.h"
#include "programinformation.h"
#include "neuroscopevideoinfo.h"
#include "managerview.h"

class QAction;


/**
 * This class serves as the main window for ndManager.  It handles the
 * menus, toolbars, and status bars. There is only one document open by application.
 *
 * @short Main window class
 * @author Lynn Hazan <lynn.hazan@myrealbox.com>
 * @version 0.1
 */
class ndManager : public QMainWindow
{
    Q_OBJECT
public:
    /**
   * Constructor.
   */
    ndManager();

    /**
   * Destructor
   */
    ~ndManager();

    /**Opens a file, only one document at the time is allowed.
  * Asking for a new one will open a new instance of the application with it.
  */
    void openDocumentFile(const QString& url=QString());

    /**Creates a ParameterView.
  * @param anatomicalGroups map given the composition of the anatomical groups. The trash group is not included, channels which are not part of
  * any group are in the trash group.
  * @param attributes map given the correspondance between the attribute names and a map given for each channel the value of the attribute.
  * @param spikeGroups map given the composition of the spike groups. The trash and undefined groups are not included, channels which are not
  * part of any group nor the trash group (this group is common to the anatomical and spike groups) are in the undefined group.
  * @param spikeGroupsInformation map containing the additional information for each spike group except the trash and undefined groups.
  * @param generalInformation object storing the information contained in the General Information part of the parameter file.
  * @param acquisitionSystemInfo map given the information related to the acquisition system.
  * @param videoInformation map given the information related to the video recording system.
  * @param files list of object storing the information describing an additional file (filter and/or subset of the raw data file).
  * @param channelColors object storing the channel color information.
  * @param channelOffsets map storing the channel default offset information.
  * @param neuroscopeVideoInfo object storing the video information used by NeuroScope.
  * @param programs list of object storing the information describing a program to be used to process the raw data.
  * @param lfpRate rate of the local field potential file.
  * @param screenGain screen gain in milivolts by centimeters used to display the field potentiels in NeuroScope.
  * @param nbSamples the number of samples in a spike; value used by NeuroScope.
  * @param peakSampleIndex the sample index corresponding to the peak of the spike; value used dby NeuroScope
  * @param traceBackgroundImage background image for the trace view in NeuroScope
  */
    void createParameterView(QMap<int, QList<int> >& anatomicalGroups, QMap<QString, QMap<int,QString> >& attributes,
                             QMap<int, QList<int> >& spikeGroups, QMap<int, QMap<QString,QString> >& spikeGroupsInformation, QMap<int, QStringList >& units,
                             GeneralInformation& generalInformation, QMap<QString,double>& acquisitionSystemInfo, QMap<QString,double>& videoInformation,
                             QList<FileInformation>& files, QList<ChannelColors>& channelColors, QMap<int,int>& channelOffsets,
                             NeuroscopeVideoInfo& neuroscopeVideoInfo, QList<ProgramInformation>& programs,
                             double lfpRate, float screenGain, int nbSamples, int peakSampleIndex, const QString &traceBackgroundImage);

    /**returns a pointer on the ParameterView containing the parameter information.
 * @return pointer on the ParameterView.
 */
    inline ParameterView* getParameterView(){return parameterView;}

public slots:

    void slotStateChanged(const QString& state);


    /**Triggers the update of the dropdown list containing the spike groups in the managerView.
  * @param nbGroups the new number of spike groups.
  */
    void nbSpikeGroupsModified(int nbGroups);

    /**Triggers the update of the dropdown list containing the file extensions in the managerView.
  * @param extensions list containing the extensions of all the specific files.
  */
    void fileModification(const QStringList& extensions);

    /**Checks that everything is ok before launching a program. This includes that the
  * modifications to the parameter file have been saved.
  * This function informs the managerView of the status and gives it the current parameter file url.*/
    void checkBeforeLaunchingPrograms();

    /**Checks that everything is ok before launching a script. This includes that the
  * modifications to the parameter file have been saved.
  * This function informs the managerView of the status and gives it the current parameter file url.*/
    void checkBeforeLaunchingScripts();

#if 0
    /**Triggers the update of the dropdown list containing the script names in the managerView.
  * @param scriptNames list containing the names.
  */
    void scriptModification(const QStringList& scriptNames);
#endif
private slots:

    void slotAbout();
    /**Opens a parameter file and loads it.*/
    void slotFileOpen();

    /**Creates a new parameter file using defaults.*/
    void slotNewFile();

    /**Opens a file from the recent files menu.
  * @param url url of the file to open.
  */
    void slotFileOpenRecent(const QString& url);
    
    /**Imports a file to be used to create a new parameter file.*/
    void slotImport();

    /**Save the current parameter file as the default one in the user .kde directory.*/
    void slotSaveDefault();

    /**Reload the current file without saving the current modified information.*/
    void slotReload();

    /**If need it, warns the user that the spike groups have been modified, then closes the actual file and displayss.*/
    void slotFileClose();

    /** queryClose is called by KDocMainWindow call just before being closed.
   */
    virtual bool queryClose();

    /** Toggles the main tool bar.*/
    void slotViewMainToolBar();

    /**Toggles the statusbar.
   */
    void slotViewStatusBar();

    /**Changes the statusbar contents for the standard label permanently, used to indicate current actions.
   * @param text the text that is displayed in the statusbar.
   */
    void slotStatusMsg(const QString &text);
    
    /***Saves the current parameter file.*/
    void slotSave();

    /***Saves the current parameter file with a new name.*/
    void slotSaveAs();


    /**Shift between the user and expert mode.*/
    void slotExpertMode();

    /**Process query.*/
    void slotQuery();

    void slotHanbook();

    void slotSaveRecentFiles();

    void slotQuit();

protected:
    void closeEvent(QCloseEvent *event);

private:
    /**Sets up all the actions used.*/
    void setupActions();

    /**Resets the state of the application to a none document open state.*/
    void resetState();

    /** sets up the statusbar for the main window by initialzing a statuslabel.
   */
    void initStatusBar();

    void readSettings();

private:

    /** Doc represents your actual document and is created only once. It keeps
  * information such as filename and does the serialization of your files.
  */
    ndManagerDoc* doc;

    /** mainDock is the main DockWidget to which all other dockWidget will be dock. Inititalized in
  * initDisplay()
  */
    QDockWidget* mainDock;

    /**The path of the currently open document.*/
    QString filePath;

    //Action and toolbar pointers
    QAction* viewMainToolBar;
    QAction* viewToolBar;
    QAction* viewStatusBar;
    QAction* mOpenAction;
    QAction* mCloseAction;
    QAction* mSaveAction;
    QAction* mSaveAsAction;
    QAction* mQuitAction;
    QAction* mUseTemplateAction;
    QAction* mReloadAction;
    QAction* mSaveAsDefaultAction;
    QAction* mNewAction;
    QAction* mQueryAction;
    //QAction* mProcessingManager;

    QAction* mExpertMode;
    //QAction* mPreferenceAction;
    QRecentFileAction *mFileOpenRecent;

    QToolBar *mMainToolBar;

    /**Stores if the current file has been imported and therefore needs to be saved under a new name.*/
    bool importedFile;

    /**Stores if the current file is new and therefore needs to be saved under a new name.*/
    bool newFile;

    /**Pointer on the ParameterView containing all the parameter information.*/
    ParameterView*  parameterView;

    /**Pointer on the ManagerView aloowing to manage the data processing.*/
    ManagerView* managerView;

    /**If the current file has been imported, this variable stores the original URL.*/
    QString importedFileUrl;

    QString html;
};

#endif // _NDMANAGER_H_
