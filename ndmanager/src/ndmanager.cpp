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
#include <qcursor.h>
#include <qevent.h>
//Added by qt3to4:
#include <QTextStream>
#include <QList>
#include <Q3PtrList>
#include <QPixmap>
#include <QFileDialog>
#include <QMessageBox>
#include <QStatusBar>
#include <QProcess>




// application specific includes
#include "ndmanager.h"
#include "configuration.h"  // class Configuration
#include "queryinputdialog.h"
#include "queryoutputdialog.h"

//General C++ include files
#include <iostream>
using namespace std;

ndManager::ndManager()
    :QMainWindow(0, "NDManager"),
      prefDialog(0L),
      mainDock(0),
      filePath(""),
      importedFile(false),
      newFile(false),
      managerView(0L)
{

    //Apply the user settings.
    initializePreferences();

    //Create a ndManagereDoc which will hold the document manipulated by the application.
    doc = new ndManagerDoc(this);

    //Setup the actions
    setupActions();

    initStatusBar();


    // initialize the recent file list
    //KDAB_PENDING fileOpenRecent->loadEntries(config);

    //Disable some actions at startup (see the ndManager.rc file)
    slotStateChanged("initState");

    slotViewStatusBar();
}

ndManager::~ndManager(){
    //Clear the memory by deleting all the pointers
    delete doc;
}

void ndManager::initializePreferences(){

}

void ndManager::setupActions()
{

    //File Menu
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));

    mNewAction = fileMenu->addAction(tr("&Open..."));
    mNewAction->setShortcut(QKeySequence::New);
    connect(mNewAction, SIGNAL(triggered()), this, SLOT(slotNewOpen()));


    mOpenAction = fileMenu->addAction(tr("&Open..."));
    mOpenAction->setShortcut(QKeySequence::Open);
    connect(mOpenAction, SIGNAL(triggered()), this, SLOT(slotFileOpen()));


    //KDAB_PENDING fileOpenRecent = KStdAction::openRecent(this, SLOT(slotFileOpenRecent(const QString&)), actionCollection());

    mCloseAction = fileMenu->addAction(tr("Close"));
    mCloseAction->setShortcut(QKeySequence::Close);
    connect(mCloseAction, SIGNAL(triggered()), this, SLOT(slotFileClose()));

    mSaveAction = fileMenu->addAction(tr("Save..."));
    mSaveAction->setShortcut(QKeySequence::Save);
    connect(mSaveAction, SIGNAL(triggered()), this, SLOT(slotSave()));

    mSaveAsAction = fileMenu->addAction(tr("&Save As..."));
    mSaveAsAction->setShortcut(QKeySequence::SaveAs);
    connect(mSaveAsAction, SIGNAL(triggered()), this, SLOT(slotSaveAs()));


    mQuitAction = fileMenu->addAction(tr("Quit"));
    mQuitAction->setShortcut(QKeySequence::Print);
    connect(mQuitAction, SIGNAL(triggered()), this, SLOT(close()));

    //KDAB_PENDING viewMainToolBar = KStdAction::showToolbar(this, SLOT(slotViewMainToolBar()), actionCollection());

    viewStatusBar = settingsMenu->addAction(tr("Show StatusBar"));
    viewStatusBar->setCheckable(true);
    connect(viewStatusBar,SIGNAL(triggered()), this,SLOT(slotViewStatusBar()));


    // KStdAction::preferences(this,SLOT(executePreferencesDlg()), actionCollection());

    //Custom actions and menus
    //File menu

    mUseTemplateAction = fileMenu->addAction(tr("Use &Template..."));
    connect(mUseTemplateAction, SIGNAL(triggered()), this, SLOT(slotImport()));

    mReloadAction = fileMenu->addAction(tr("&Reload"));
    mReloadAction->setShortcut(Qt::Key_F5);
    connect(mReloadAction, SIGNAL(triggered()), this, SLOT(slotReload()));

    mSaveAsDefaultAction = fileMenu->addAction(tr("Save as &Default"));
    connect(mSaveAsDefaultAction, SIGNAL(triggered()), this, SLOT(slotSaveDefault()));


    QMenu *actionMenu = menuBar()->addMenu(tr("&Actions"));
    mQueryAction = actionMenu->addAction(tr("&Query"));
    connect(mQueryAction, SIGNAL(triggered()), this, SLOT(slotQuery()));


    //Processing menu
    QMenu *processingMenu = menuBar()->addMenu(tr("&Processing"));
    mProcessingManager = processingMenu->addAction(tr("Show Processing Manager"));

    //Settings
    QMenu *settingsMenu = menuBar()->addMenu("&Settings");
    mExpertMode = settingsMenu->addAction(tr("&Expert Mode"));
#if KDAB_PENDING
    config->setGroup("General");
    expertMode->setChecked(config->readBoolEntry("expertMode"));
#endif

    QMenu *helpMenu = menuBar()->addMenu(tr("Help"));
    QAction *about = helpMenu->addAction(tr("About"));
    connect(about,SIGNAL(triggered()), this,SLOT(slotAbout()));


    createGUI(QString(),false);

}

void ndManager::initStatusBar()
{
    statusBar()->showMessage(tr("Ready."));
}


void ndManager::slotStatusMsg(const QString &text)
{
    ///////////////////////////////////////////////////////////////////
    // change status message permanently
    statusBar()->clear();
    statusBar()->showMessage(text);
}

void ndManager::slotViewMainToolBar()
{
    slotStatusMsg(tr("Toggle the main toolbar..."));

    // turn Toolbar on or off
    if(!viewMainToolBar->isChecked()) toolBar("mainToolBar")->hide();
    else toolBar("mainToolBar")->show();

    slotStatusMsg(tr("Ready."));
}


void ndManager::slotViewStatusBar()
{
    slotStatusMsg(tr("Toggle the statusbar..."));
    ///////////////////////////////////////////////////////////////////
    //turn Statusbar on or off
    if(!viewStatusBar->isChecked()) statusBar()->hide();
    else statusBar()->show();

    slotStatusMsg(tr("Ready."));
}


void ndManager::slotFileOpen()
{
    slotStatusMsg(tr("Opening file..."));

    QString url=QFileDialog::getOpenFileName(this, tr("Open File..."),QString(),
                                             tr("*.xml|Parameter File (*.xml)\n*|All files") );
    if(!url.isEmpty()) openDocumentFile(url);

    slotStatusMsg(tr("Ready."));
}

void ndManager::slotFileOpenRecent(const QString& url){
    slotStatusMsg(tr("Opening file..."));

    openDocumentFile(url);

    slotStatusMsg(tr("Ready."));
}

void ndManager::slotNewFile(){
    slotStatusMsg(tr("Initializing new file..."));
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    //If no document is open already, create a new parameter file.
    if(!mainDock){
        newFile = true;
        int returnStatus = doc->newDocument();
        if(returnStatus == ndManagerDoc::PARSE_ERROR){
            QApplication::restoreOverrideCursor();
            QMessageBox::critical (this, tr("Error!"),tr("The new parameter file could not be initialize due to parsing error."));
            resetState();
            return;
        }

        QString url = QDir::currentPath()+QDir::separator() + "Untitled";
        doc->rename(url);
        filePath = url;
        setCaption(url);
    }
    //Open a new instance of the application.
    else{
        QProcess::startDetached("ndmanager", QStringList()<<command);
    }
    QApplication::restoreOverrideCursor();
    slotStatusMsg(tr("Ready."));
}

void ndManager::openDocumentFile(const QString& url)
{
    slotStatusMsg(tr("Opening file..."));

    filePath = url;
#if KDAB_PENDING
    if(url.protocol() == "file"){
        QFileInfo file(filePath);
        if((fileOpenRecent->items().contains(url.prettyURL())) && !file.exists()){
            QString title = "File not found: ";
            title.append(filePath);
            int answer = QMessageBox::question(this,tr("The selected file no longer exists. Do you want to remove it from the list of recent opened files ?"), tr(title));
            if(answer == QMessageBox::Yes){
                QString* urlB = new QString();
                urlB->setPath(url.url());
                //KDAB_PENDING fileOpenRecent->removeURL(url);
            }
            else  {
                //KDAB_PENDING fileOpenRecent->addURL(url); //hack, unselect the item
            }
            filePath = "";

            return;
        }
    }
    //Do not handle remote files
    else{
        QMessageBox::critical(this,tr("Remote file handling"),tr("Sorry, NDManager does not handle remote files."));
        //KDAB_PENDING fileOpenRecent->addURL(url); //hack, unselect the item
        return;
    }
#endif
    //Check if the file exists
    if(!QFile::exists(url)){
        QMessageBox::critical (this, tr("Error!"),tr("The selected file does not exist."));
        //KDAB_PENDING fileOpenRecent-> removeURL(url);
        return;
    }

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    //If no document is open already, open the document asked.
    if(!mainDock){
        //KDAB_PENDING fileOpenRecent->addURL(url);

        //Open the file (that will also initialize the document)
        int returnStatus = doc->openDocument(url);
        if(returnStatus == ndManagerDoc::PARSE_ERROR){
            QApplication::restoreOverrideCursor();
            QMessageBox::critical (this, tr("Error!"),tr("The selected parameter file could not be initialize due to parsing error."));
            //close the document
            doc->closeDocument();
            resetState();
            return;
        }

        //Save the recent file list
        fileOpenRecent->saveEntries(config);

        setCaption(url);
        QApplication::restoreOverrideCursor();
    }
    // check, if this document is already open. If yes, do not do anything
    else{
        QString path = doc->url();

        if(path == url){
            //KDAB_PENDING fileOpenRecent->addURL(url); //hack, unselect the item
            QApplication::restoreOverrideCursor();
            return;
        }
        //If the document asked is not the already open. Open a new instance of the application with it.
        else{
            //KDAB_PENDING fileOpenRecent->addURL(url);
            //Save the recent file list
            fileOpenRecent->saveEntries(config);
            filePath = path;

            QProcess::startDetached("ndmanager", QStringList()<<url);
            QApplication::restoreOverrideCursor();
        }
    }

    slotStatusMsg(tr("Ready."));
}

void ndManager::createParameterView(QMap<int, QList<int> >& anatomicalGroups,QMap<QString, QMap<int,QString> >& attributes,
                                    QMap<int, QList<int> >& spikeGroups,QMap<int, QMap<QString,QString> >& spikeGroupsInformation,QMap<int, QList<QString> >& units,
                                    GeneralInformation& generalInformation,QMap<QString,double>& acquisitionSystemInfo,QMap<QString,double>& videoInformation,
                                    QList<FileInformation>& files,QList<ChannelColors>& channelColors,QMap<int,int>& channelOffsets,
                                    NeuroscopeVideoInfo& neuroscopeVideoInfo,QList<ProgramInformation>& programs,
                                    double lfpRate,float screenGain,int nbSamples,int peakSampleIndex,QString traceBackgroundImage){

    //Create the mainDock (parameter view)
    mainDock = createDockWidget( "1", QPixmap(), 0L, tr("Parameters"), tr("Parameters"));
    mainDock->setDockWindowTransient(this,true);

    parameterView = new ParameterView(this,*doc,mainDock,tr("ParameterView"),expertMode->isChecked());

    connect(parameterView,SIGNAL(partShown(Kate::View*)),this,SLOT(updateGUI(Kate::View*)));
    connect(parameterView,SIGNAL(partHidden()),this,SLOT(updateGUI()));
    connect(parameterView,SIGNAL(nbSpikeGroupsHasBeenModified(int)),this,SLOT(nbSpikeGroupsModified(int)));
    connect(parameterView,SIGNAL(fileHasBeenModified(QList<QString>)),this,SLOT(fileModification(QList<QString>)));
    connect(parameterView,SIGNAL(scriptListHasBeenModified(const QList<QString>&)),this,SLOT(scriptModification(const QList<QString>&)));



    parameterView->initialize(anatomicalGroups,attributes,spikeGroups,spikeGroupsInformation,units,generalInformation,
                              acquisitionSystemInfo,videoInformation,files,channelColors,channelOffsets,neuroscopeVideoInfo,programs,lfpRate,screenGain,nbSamples,peakSampleIndex,traceBackgroundImage);

    mainDock->setWidget(parameterView);
    //allow dock on the Bottom side only
    mainDock->setDockSite(QDockWidget::DockBottom);
    setCentralWidget(mainDock); // central widget in a KDE mainwindow <=> setMainWidget
    setMainDockWidget(mainDock);
    //disable docking abilities of mainDock itself
    mainDock->setEnableDocking(Qt::NoDockWidgetArea);

    //show all the encapsulated widgets of all controlled dockwidgets
    dockManager->activate();

    //Enable some actions now that a document is open (see the klustersui.rc file)
    slotStateChanged("documentState");

}

void ndManager::createManagerView(){
#if KDAB_PENDING	
    if(managerView == 0L){
        //Create and add the ManagerView
        QDockWidget* manager = createDockWidget("Manager", QPixmap());
        managerView = new ManagerView();

        int returnStatus =  managerView->addKonsole(doc->url(),parameterView->getNbGroups(),parameterView->getFileExtensions(),
                                                    parameterView->getFileScriptNames());
        if(returnStatus == ManagerView::NO_KPART){
            QMessageBox::critical (this, tr("IO Error!"),tr("The Konsole part does not exist, no terminal can be created."));
            return;
        }
        if(returnStatus == ManagerView::PART_LOADING_ERROR){
            QMessageBox::critical (this, tr("IO Error!"),tr("The Konsole part could not be loaded."));
            return;
        }


        //Enable the application to be informed that the dockWidget is being closed.
        //To do so, connect the dockwidget close button to the konsoleDockBeingClosed slot.
        connect(manager, SIGNAL(headerCloseButtonClicked()), this, SLOT(konsoleDockBeingClosed()));
        //Enable the application to be informed that Konsole is being destroyed.
        connect(managerView, SIGNAL(beingDestroyed()), this, SLOT(konsoleDockBeingClosed()));
        //the managerView needs to check information about the parameter file before being able to launch a program or a script.
        connect(managerView, SIGNAL(checkBeforeLaunchingPrograms()), this, SLOT(checkBeforeLaunchingPrograms()));
        connect(managerView, SIGNAL(checkBeforeLaunchingScripts()), this, SLOT(checkBeforeLaunchingScripts()));

        manager->setWidget(managerView);//assign the widget
        manager->manualDock(mainDock,QDockWidget::DockBottom,25);
        manager->setEnableDocking(QDockWidget::DockCorner);
        manager->setDockSite(QDockWidget::DockCorner);
    }
    else{
        managerView->addKonsole(doc->url(),parameterView->getNbGroups(),parameterView->getFileExtensions(),parameterView->getFileScriptNames());
        QDockWidget* manager = dockManager->findWidgetParentDock(managerView);
        manager->manualDock(mainDock,QDockWidget::DockBottom,25);
        manager->setEnableDocking(QDockWidget::DockCorner);
        manager->setDockSite(QDockWidget::DockCorner);
    }

    //show all the encapsulated widgets of all controlled dockwidgets
    dockManager->activate();
    slotStateChanged("hideManger");
#endif
}

void ndManager::konsoleDockBeingClosed(){
    QDockWidget* dock = dockManager->findWidgetParentDock(managerView);
    dock->undock();
    dock->hide();
    slotStateChanged("showManager");
}


void ndManager::slotImport(){
    slotStatusMsg(tr("importing file as model..."));
    importedFile = true;

    QString url = QFileDialog::getOpenFileName(this, tr("Import file as model..."),QString(),
                                               tr("*.xml|Parameter File (*.xml)\n*|All files") );
    if(!url.isEmpty()) openDocumentFile(url);

    importedFileUrl = url + QDir::separator() + "Untitled";

    doc->rename(url);
    filePath = url;
    setCaption(url);

    slotStatusMsg(tr("Ready."));
}

void ndManager::slotFileClose(){
    if(doc != 0){
        bool hasBeenCancel = false;

        //check first if some scripts have been modified
        QList<QString> scriptModified = parameterView->modifiedScripts();
        if(scriptModified.size() != 0){
            QList<QString>::iterator iterator;
            for(iterator = scriptModified.begin(); iterator != scriptModified.end(); ++iterator){
                QString name = *iterator;
                switch(QMessageBox::question(0,tr("Script modification"),tr("The script %1 has been modified, do you want to save the it?").arg(name),QMessageBox::Save|QMessageBox::Discard|QMessageBox::Cancel)){
                case QMessageBox::Save://<=> Save
                    parameterView->saveScript(name);
                    break;
                case QMessageBox::Discard://<=> Discard
                    break;
                case QMessageBox::Cancel:
                    hasBeenCancel = true;
                    break;
                }
            }
        }

        //check if some descriptions have been modified
        QList<QString> programModified = parameterView->modifiedProgramDescription();
        if(programModified.size() != 0){
            QList<QString>::iterator iterator;
            for(iterator = programModified.begin(); iterator != programModified.end(); ++iterator){
                QString name = *iterator;
                switch(QMessageBox::question(0,tr("Program description modification"),tr("The description of the program %1 has been modified, do you want to save the it?").arg(name),QMessageBox::Save|QMessageBox::Discard|QMessageBox::Cancel)){
                case QMessageBox::Save://<=> Save
                    parameterView->saveProgramDescription(name);
                    break;
                case QMessageBox::Discard://<=> Discard
                    break;
                case QMessageBox::Cancel:
                    hasBeenCancel = true;
                    break;
                }
            }
        }

        if(!hasBeenCancel){
            //if at least one parameter has been modified, prompt the user to save the information
            if(parameterView->isModified()){
                switch(QMessageBox::question(0,tr("Parameters modification"),tr("Some parameters have changed, do you want to save the parameter file?"),QMessageBox::Save|QMessageBox::Discard|QMessageBox::Cancel)){
                case QMessageBox::Save://<=> Save
                    slotSave();
                    break;
                case QMessageBox::Discard://<=> Discard
                    break;
                case QMessageBox::Cancel:
                    return;
                }
            }

            //close the document
            doc->closeDocument();
            //update gui in case of Kate kparte added
            updateGUI();
            //Delete the main view
            delete mainDock;
            mainDock = 0L;
            if(managerView != 0L){
                QDockWidget* dock = dockManager->findWidgetParentDock(managerView);
                dock->undock();
                dock->hide();
                delete dock;
            }
            resetState();
        }
    }

}

bool ndManager::queryClose()
{
    //Save the recent file list
    fileOpenRecent->saveEntries(config);

    //Save the current mode
    config->setGroup("General");
    config->writeEntry("expertMode",expertMode->isChecked());

    if(doc == 0 || mainDock == 0L) return true;
    else{
        //check first if some scripts have been modified
        QList<QString> scriptModified = parameterView->modifiedScripts();
        if(scriptModified.size() != 0){
            QList<QString>::iterator iterator;
            for(iterator = scriptModified.begin(); iterator != scriptModified.end(); ++iterator){
                QString name = *iterator;
                switch(QMessageBox::question(0,tr("Script modification"),tr("The script %1 has been modified, do you want to save the it?").arg(name),QMessageBox::Save|QMessageBox::Discard|QMessageBox::Cancel)){
                case QMessageBox::Save://<=> Save
                    parameterView->saveScript(name);
                    break;
                case QMessageBox::Discard://<=> Discard
                    break;
                case QMessageBox::Cancel:
                    return false;
                    break;
                }
            }
        }

        //check if some descriptions have been modified
        QList<QString> programModified = parameterView->modifiedProgramDescription();
        if(programModified.size() != 0){
            QList<QString>::iterator iterator;
            for(iterator = programModified.begin(); iterator != programModified.end(); ++iterator){
                QString name = *iterator;
                switch(QMessageBox::question(0,tr("Program description modification"),tr("The description of the program %1 has been modified, do you want to save the it?").arg(name),QMessageBox::Save|QMessageBox::Discard|QMessageBox::Cancel)){
                case QMessageBox::Save://<=> Save
                    parameterView->saveProgramDescription(name);
                    break;
                case QMessageBox::Discard://<=> Discard
                    break;
                case QMessageBox::Cancel:
                    return false;
                    break;
                }
            }
        }

        //if at least one parameter has been modified, prompt the user to save the information
        if(parameterView->isModified()){
            switch(QMessageBox::question(0,tr("Parameters modification"),tr("Some parameters have changed, do you want to save the parameter file?"),QMessageBox::Save|QMessageBox::Discard|QMessageBox::Cancel)){
            case QMessageBox::Save://<=> Save
                slotSave();
                break;
            case QMessageBox::Discard://<=> Discard
                break;
            case QMessageBox::Cancel:
                return false;
            }
        }

        //close the document
        doc->closeDocument();
        return true;
    }

}

void ndManager::slotSave(){
    slotStatusMsg(tr("Saving..."));
    //if  the current file is new or was initially imported, it has to be saved under a new name
    if(importedFile || newFile){
        QString initialPath;
        if(newFile) initialPath = QDir::currentPath();
        else{
            QString currentUrl =  doc->url();
            currentUrl.setFileName("");
            initialPath = currentUrl;
        }

        QString url=QFileDialog::getSaveFileName( this, tr("Save as..."),initialPath,tr("*.xml|Xml Files\n*|All Files"));
        if(!url.isEmpty()){
            QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
            int saveStatus = doc->saveAs(url);
            if(saveStatus == ndManagerDoc::SAVE_ERROR){
                QMessageBox::critical(0, tr("IO Error!"),tr("The current file could not be saved possibly because of insufficient file access permissions."
                                                            " You may consider saving your session file to another location."));
            }
            if(importedFile || newFile){
                filePath = url;
                setCaption(url);
            }
            importedFile = false;
            newFile = false;
            QApplication::restoreOverrideCursor();
        }
    }
    else{
        //Save the parameter file
        int saveStatus = doc->save();
        if(saveStatus == ndManagerDoc::SAVE_ERROR){
            QMessageBox::critical(0, tr("IO Error!"),tr("The current file could not be saved possibly because of insufficient file access permissions."
                                                        " You may consider saving your session file to another location using the Save As entry in the File menu."));
        }
    }
    slotStatusMsg(tr("Ready."));
}

void ndManager::slotSaveAs(){
    slotStatusMsg(tr("Saving as..."));

    //Save the parameter file
    QString url=QFileDialog::getSaveFileName(this, tr("Save as..."),doc->url(),tr("*|All files"));
    if(!url.isEmpty()){
        int saveStatus = doc->saveAs(url);
        if(saveStatus == ndManagerDoc::SAVE_ERROR){
            QMessageBox::critical(0,tr("The current file could not be saved possibly because of insufficient file access permissions."), tr("I/O Error !"));
        }
        filePath = url;
        setCaption(url);

        importedFile = false;
        newFile = false;
    }
    slotStatusMsg(tr("Ready."));
}
void ndManager::resetState(){
    //Disable some actions when no document is open (see the klustersui.rc file)
    slotStateChanged("initState");
    setCaption("");
    importedFile = false;
    newFile = false;
    managerView = 0L;
}

void ndManager::executePreferencesDlg(){
    if(prefDialog == 0L){
        prefDialog = new ndManagerPreferences(this);
        // connect to the "settingsChanged" signal
        connect(prefDialog,SIGNAL(settingsChanged()),this,SLOT(applyPreferences()));
    }

    // update the dialog widgets.
    prefDialog->updateDialog();

    if(prefDialog->exec() == QDialog::Accepted){  // execute the dialog
        //if the user did not click the applyButton, save the new settings.
        if(prefDialog->isApplyEnable()){
            prefDialog->updateConfiguration();        // store settings in config object
            applyPreferences();                      // let settings take effect
        }
    }
}

void ndManager::applyPreferences() {
    configuration().write();
}

void ndManager::slotSaveDefault(){
    slotStatusMsg(tr("Saving as default..."));
    int saveStatus = doc->saveDefault();
    if(saveStatus == ndManagerDoc::SAVE_ERROR){
        QMessageBox::critical(0, tr("IO Error!"),tr("The current file could not be saved as the default parameter file."));
    }
    slotStatusMsg(tr("Ready."));
}

void ndManager::slotReload(){
    slotStatusMsg(tr("reloading..."));

    //Get the current active page index
    int activePageIndex = parameterView->activePageIndex();

    //Save the recent file list
    fileOpenRecent->saveEntries(config);
    config->writePathEntry("openFile",filePath);

    //close the current document
    doc->closeDocument();
    //update gui in case of Kate kparte added
    updateGUI();
    //Delete the main view
    delete mainDock;
    mainDock = 0L;
    if(managerView != 0L){
        QDockWidget* dock = dockManager->findWidgetParentDock(managerView);
        dock->undock();
        dock->hide();
        delete dock;
    }
    resetState();

    //Reopen the document

    QString url = QString(filePath);

    QFileInfo file(filePath);

    if(!file.exists()){
        QMessageBox::critical (this, tr("Sorry!"),tr("The selected parameter file could not be reloaded as it appears to have been removed from disk."));
        slotStatusMsg(tr("Ready."));
        return;
    }

    //Open the file (that will also initialize the document)
    int returnStatus = doc->openDocument(url);
    if(returnStatus == ndManagerDoc::PARSE_ERROR){
        QApplication::restoreOverrideCursor();
        QMessageBox::critical (this, tr("IO Error!"),tr("The selected parameter file could not be initialize due to parsing error."));
        //close the document
        doc->closeDocument();
        resetState();
        slotStatusMsg(tr("Ready."));
        return;
    }

    setCaption(filePath);
    QApplication::restoreOverrideCursor();

    //Raise the previously active page
    parameterView->showPage(activePageIndex);

    slotStatusMsg(tr("Ready."));
}

void ndManager::slotQuery(){
    slotStatusMsg(tr("Processing query..."));
    queryResult = "";
    QueryInputDialog *queryInputDialog = new QueryInputDialog();
    if(queryInputDialog->exec() == QDialog::Accepted)
    {
        // Run query
        QProcess process;
#if KDAB_PENDING
        //Not portable
        process.setUseShell(true);
        process << "find " + queryInputDialog->getPath() + " -name '*xml' -exec xpathReader --html {} \"" + queryInputDialog->getQuery() + "\" \\; | sed 'N;s/<tr>/<tr class=\"tr1\">/;s/<tr>/<tr class=\"tr2\">/'";
        connect(&process,SIGNAL(receivedStdout(QProcess*,char*,int)),this,SLOT(slotQueryResult(QProcess*,char*,int)));
        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
        process.start(QProcess::Block,QProcess::Stdout);
#endif
        QApplication::restoreOverrideCursor();

        // Read HTML footer from file
        QFile htmlFile(":/queryreport.html");
        if(htmlFile.open(QIODevice::ReadOnly))
        {
            QTextStream stream(&htmlFile);
            html = stream.read();
            htmlFile.close();
        }
        // Display query result
        // 1) Insert HTML table into HTML template
        html.replace("QUERY_RESULTS",queryResult);
        html.replace("QUERY",queryInputDialog->getQuery());
        // 2) Convert HTML table to plain text (so the user can save the results as text)
        queryResult.replace(QRegExp("<tr[^>]*><td><a[^>]*>"),"");
        queryResult.replace("</a>","");
        queryResult.replace("</td><td>","\t");
        queryResult.replace("</td></tr>","");
        QueryOutputDialog *queryOutputDialog = new QueryOutputDialog(html,queryResult);
        queryOutputDialog->exec();
        delete queryOutputDialog;
    }
    delete queryInputDialog;
    slotStatusMsg(tr("Ready."));
}

void ndManager::slotQueryResult(QString message){
    queryResult += message;
}

void ndManager::updateGUI(){
    Q3PtrList<KXMLGUIClient> clients = guiFactory()->clients();

    //A part has already be added, remove
    if(clients.count() > 1){
        guiFactory()->removeClient(clients.at(1));
    }
}

void ndManager::slotExpertMode(){
    bool isImportedFile = importedFile;
    bool isNewFile = newFile;

    //close the file and reopen it in the new mode
    slotFileClose();

    if(isNewFile && filePath.contains("Untitled")) slotNewFile();
    else if(isImportedFile && filePath.contains("Untitled")){
        QString url = importedFileUrl;
        openDocumentFile(url);
        url.setFileName("Untitled");
        doc->rename(url);
        filePath = url;
        setCaption(url);
    }
    else{
        QString url;
        url.setPath(filePath);
        openDocumentFile(url);
    }
}

void ndManager::nbSpikeGroupsModified(int nbGroups){
    if(managerView != 0L){
        managerView->updateSpikeGroupList(nbGroups);
    }
}

void ndManager::fileModification(QList<QString> extensions){
    if(managerView != 0L){
        managerView->updateFileList(extensions);
    }
}

void ndManager::scriptModification(const QList<QString>& scriptNames){
    if(managerView != 0L){
        managerView->updateScriptList(scriptNames);
    }
}

void ndManager::checkBeforeLaunchingPrograms(){
    if(managerView != 0L){
        if(importedFile || newFile) managerView->updateDocumentInformation(doc->url(),false);
        else{
            if(parameterView->isModified()) managerView->updateDocumentInformation(doc->url(),false);
            else{
                QList<QString> programModified = parameterView->modifiedProgramDescription();
                if(programModified.size()!= 0) managerView->updateDocumentInformation(doc->url(),false);
                else managerView->updateDocumentInformation(doc->url(),true);
            }
        }
    }
}

void ndManager::checkBeforeLaunchingScripts(){
    if(managerView != 0L){
        if(importedFile || newFile) managerView->updateDocumentInformation(doc->url(),false);
        else{
            if(parameterView->isModified()) managerView->updateDocumentInformation(doc->url(),false);
            else{
                QList<QString> programModified = parameterView->modifiedProgramDescription();
                if(programModified.size()!= 0) managerView->updateDocumentInformation(doc->url(),false);
                else{
                    QList<QString> scriptModified = parameterView->modifiedScripts();
                    if(scriptModified.size()!= 0) managerView->updateDocumentInformation(doc->url(),false);
                    else managerView->updateDocumentInformation(doc->url(),true);
                }
            }
        }
    }
}


void ndManager::slotStateChanged(const QString& state)
{
    if(state == QLatin1String("initState")) {
    } else if(state == QLatin1String("documentState")) {
    } else if(state == QLatin1String("showManager")) {
    } else if(state == QLatin1String("hideManger")) {

    } else {
        qDebug()<<" state unknown"<<state;
    }

}

void ndManager::slotAbout()
{
    QMessageBox::about(this,tr("NdManager"),tr("Neurophysiological Data Processing Manager"));
}


#include "ndmanager.moc"
