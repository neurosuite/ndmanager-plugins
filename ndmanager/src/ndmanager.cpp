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
#include <qcursor.h>
#include <qevent.h>

#include <QTextStream>
#include <QList>
#include <QPixmap>
#include <QFileDialog>
#include <QMessageBox>
#include <QStatusBar>
#include <QProcess>
#include <QMenu>
#include <QMenuBar>
#include <QDebug>
#include <QSettings>
#include <QPointer>

// application specific includes
#include "ndmanager.h"
#include "queryinputdialog.h"
#include "queryoutputdialog.h"
#include "qhelpviewer.h"
#include "config-ndmanager.h"


ndManager::ndManager()
    :QMainWindow(0),
      mainDock(0),
      importedFile(false),
      newFile(false),
      managerView(0L),
      parameterView(0)
{

    setObjectName("NDManager");
    mMainToolBar = new QToolBar();
    mMainToolBar->setObjectName("maintoolbar");

    addToolBar(mMainToolBar);

    //Create a ndManagereDoc which will hold the document manipulated by the application.
    doc = new ndManagerDoc(this);

    //Setup the actions
    setupActions();

    initStatusBar();
    //Disable some actions at startup (see the ndManager.rc file)
    slotStateChanged("initState");

    slotViewStatusBar();
    readSettings();
}

ndManager::~ndManager(){
    //Clear the memory by deleting all the pointers
    delete doc;
    delete mainDock;
}

void ndManager::closeEvent(QCloseEvent *event)
{
    if (!queryClose()) {
        event->ignore();
        return;
    }
    QSettings settings;
    settings.beginGroup("geometry");
    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());
    settings.endGroup();
    settings.sync();
    QMainWindow::closeEvent(event);
}


void ndManager::setupActions()
{
    //File Menu
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));

    mNewAction = fileMenu->addAction(tr("&New..."));
    mNewAction->setIcon(QPixmap(":/shared-icons/document-new"));
    mNewAction->setShortcut(QKeySequence::New);
    connect(mNewAction, SIGNAL(triggered()), this, SLOT(slotNewFile()));


    mOpenAction = fileMenu->addAction(tr("&Open..."));
    mOpenAction->setIcon(QPixmap(":/shared-icons/document-open"));
    mOpenAction->setShortcut(QKeySequence::Open);
    connect(mOpenAction, SIGNAL(triggered()), this, SLOT(slotFileOpen()));

    mFileOpenRecent = new QRecentFileAction(this);
    QSettings settings;
    mFileOpenRecent->setRecentFiles(settings.value(QLatin1String("Recent Files"),QStringList()).toStringList());
    fileMenu->addAction(mFileOpenRecent);
    connect(mFileOpenRecent, SIGNAL(recentFileSelected(QString)), this, SLOT(slotFileOpenRecent(QString)));
    connect(mFileOpenRecent, SIGNAL(recentFileListChanged()), this, SLOT(slotSaveRecentFiles()));

    mUseTemplateAction = fileMenu->addAction(tr("Use &Template..."));
    connect(mUseTemplateAction, SIGNAL(triggered()), this, SLOT(slotImport()));

    fileMenu->addSeparator();
    mSaveAction = fileMenu->addAction(tr("Save..."));
    mSaveAction->setIcon(QPixmap(":/shared-icons/document-save"));
    mSaveAction->setShortcut(QKeySequence::Save);
    connect(mSaveAction, SIGNAL(triggered()), this, SLOT(slotSave()));

    mSaveAsAction = fileMenu->addAction(tr("&Save As..."));
    mSaveAsAction->setIcon(QPixmap(":/shared-icons/document-save-as"));
    mSaveAsAction->setShortcut(QKeySequence::SaveAs);
    connect(mSaveAsAction, SIGNAL(triggered()), this, SLOT(slotSaveAs()));


    mSaveAsDefaultAction = fileMenu->addAction(tr("Save as &Default"));
    connect(mSaveAsDefaultAction, SIGNAL(triggered()), this, SLOT(slotSaveDefault()));

    mReloadAction = fileMenu->addAction(tr("&Reload"));
    mReloadAction->setShortcut(Qt::Key_F5);
    connect(mReloadAction, SIGNAL(triggered()), this, SLOT(slotReload()));


    fileMenu->addSeparator();


    mCloseAction = fileMenu->addAction(tr("Close"));
    mCloseAction->setIcon(QPixmap(":/shared-icons/document-close"));
    mCloseAction->setShortcut(QKeySequence::Close);
    connect(mCloseAction, SIGNAL(triggered()), this, SLOT(slotFileClose()));


    fileMenu->addSeparator();

    mQuitAction = fileMenu->addAction(tr("Quit"));
    mQuitAction->setIcon(QPixmap(":/shared-icons/window-close"));
    mQuitAction->setShortcut(QKeySequence::Quit);
    connect(mQuitAction, SIGNAL(triggered()), this, SLOT(close()));


    QMenu *actionMenu = menuBar()->addMenu(tr("&Actions"));
    mQueryAction = actionMenu->addAction(tr("&Query"));
#ifndef Q_OS_UNIX
    mQueryAction->setEnabled(false);
#endif
    connect(mQueryAction, SIGNAL(triggered()), this, SLOT(slotQuery()));

    //mProcessingManager = actionMenu->addAction(tr("Show Processing Manager"));

    QMenu *settingsMenu = menuBar()->addMenu(tr("&Settings"));

    //Settings
    mExpertMode = settingsMenu->addAction(tr("&Expert Mode"));
    mExpertMode->setCheckable(true);
    connect(mExpertMode, SIGNAL(triggered(bool)), this, SLOT(slotExpertMode()));
    settingsMenu->addSeparator();

    settings.beginGroup("General");
    if ( settings.contains("expertMode")) mExpertMode->setChecked(settings.value("expertMode").toBool());
	 else mExpertMode->setChecked(true);
    settings.endGroup();

    viewMainToolBar = settingsMenu->addAction(tr("Show Main Toolbar"));

    viewMainToolBar->setCheckable(true);
    viewMainToolBar->setChecked(true);
    connect(viewMainToolBar,SIGNAL(triggered()), this,SLOT(slotViewMainToolBar()));


    viewStatusBar = settingsMenu->addAction(tr("Show StatusBar"));
    viewStatusBar->setCheckable(true);
    connect(viewStatusBar,SIGNAL(triggered()), this,SLOT(slotViewStatusBar()));
    viewStatusBar->setChecked(true);

    QMenu *helpMenu = menuBar()->addMenu(tr("Help"));
    QAction *handbook = helpMenu->addAction(tr("Handbook"));
    handbook->setShortcut(Qt::Key_F1);
    connect(handbook,SIGNAL(triggered()), this,SLOT(slotHanbook()));

    QAction *about = helpMenu->addAction(tr("About"));
    connect(about,SIGNAL(triggered()), this,SLOT(slotAbout()));

    mMainToolBar->addAction(mNewAction);
    mMainToolBar->addAction(mOpenAction);
    mMainToolBar->addAction(mSaveAction);

    resize(800,600);
}

void ndManager::initStatusBar()
{
    statusBar()->showMessage(tr("Ready."));
}


void ndManager::slotStatusMsg(const QString &text)
{
    ///////////////////////////////////////////////////////////////////
    // change status message permanently
    statusBar()->clearMessage();
    statusBar()->showMessage(text);
}

void ndManager::slotViewMainToolBar()
{
    slotStatusMsg(tr("Toggle the main toolbar..."));
    mMainToolBar->setVisible(viewMainToolBar->isChecked());
    slotStatusMsg(tr("Ready."));
}


void ndManager::slotViewStatusBar()
{
    slotStatusMsg(tr("Toggle the statusbar..."));
    ///////////////////////////////////////////////////////////////////
    //turn Statusbar on or off
    statusBar()->setVisible(viewStatusBar->isChecked());

    slotStatusMsg(tr("Ready."));
}


void ndManager::slotFileOpen()
{
    slotStatusMsg(tr("Opening file..."));

    QSettings settings;
    const QString url=QFileDialog::getOpenFileName(this, tr("Open File..."),settings.value("CurrentDirectory").toString(),
                                                   tr("Parameter File (*.xml);;All files (*.*)") );
    if(!url.isEmpty()) {
        QDir CurrentDir;
        settings.setValue("CurrentDirectory", CurrentDir.absoluteFilePath(url));
        openDocumentFile(url);
    }

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

        const QString url = QDir::currentPath()+QDir::separator() + "Untitled";
        doc->rename(url);
        filePath = url;
        setWindowTitle(url);
    }
    //Open a new instance of the application.
    else{
        QProcess::startDetached("ndmanager");
    }
    QApplication::restoreOverrideCursor();
    slotStatusMsg(tr("Ready."));
}

void ndManager::openDocumentFile(const QString& url)
{
    slotStatusMsg(tr("Opening file..."));

    filePath = url;
    QFileInfo file(filePath);
    if(!file.exists()){
        QString title = tr("File not found: %1").arg(filePath);
        const int answer = QMessageBox::question(this,title, tr("The selected file no longer exists. Do you want to remove it from the list of recent opened files ?"), QMessageBox::Yes|QMessageBox::No);
        if(answer == QMessageBox::Yes){
            mFileOpenRecent->removeRecentFile(url);
        } else  {
            mFileOpenRecent->addRecentFile(url); //hack, unselect the item
        }
        filePath.clear();

        return;
    }
    //Check if the file exists
    if(!QFile::exists(url)){
        QMessageBox::critical (this, tr("Error!"),tr("The selected file does not exist."));
        mFileOpenRecent-> removeRecentFile(url);
        return;
    }

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    //If no document is open already, open the document asked.
    if(!mainDock){
        mFileOpenRecent->addRecentFile(url);

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


        setWindowTitle(url);
        QApplication::restoreOverrideCursor();
    }
    // check, if this document is already open. If yes, do not do anything
    else{
        QString path = doc->url();

        if(path == url){
            mFileOpenRecent->addRecentFile(url); //hack, unselect the item
            QApplication::restoreOverrideCursor();
            return;
        }
        //If the document asked is not the already open. Open a new instance of the application with it.
        else{
            mFileOpenRecent->addRecentFile(url);
            filePath = path;

            QProcess::startDetached("ndmanager", QStringList()<<url);
            QApplication::restoreOverrideCursor();
        }
    }

    slotStatusMsg(tr("Ready."));
}

void ndManager::createParameterView(QMap<int, QList<int> >& anatomicalGroups,QMap<QString, QMap<int,QString> >& attributes,
                                    QMap<int, QList<int> >& spikeGroups,QMap<int, QMap<QString,QString> >& spikeGroupsInformation,QMap<int, QStringList >& units,
                                    GeneralInformation& generalInformation,QMap<QString,double>& acquisitionSystemInfo,QMap<QString,double>& videoInformation,
                                    QList<FileInformation>& files,QList<ChannelColors>& channelColors,QMap<int,int>& channelOffsets,
                                    NeuroscopeVideoInfo& neuroscopeVideoInfo,QList<ProgramInformation>& programs,
                                    double lfpRate,float screenGain,int nbSamples,int peakSampleIndex,const QString& traceBackgroundImage){

    //Create the mainDock (parameter view)
    delete mainDock;
    mainDock = new QDockWidget(tr("Parameters"));

    parameterView = new ParameterView(this,*doc,mainDock,"ParameterView",mExpertMode->isChecked());

    connect(parameterView,SIGNAL(nbSpikeGroupsHasBeenModified(int)),this,SLOT(nbSpikeGroupsModified(int)));
    connect(parameterView,SIGNAL(fileHasBeenModified(QStringList)),this,SLOT(fileModification(QStringList)));
    //connect(parameterView,SIGNAL(scriptListHasBeenModified(QStringList)),this,SLOT(scriptModification(QStringList)));



    parameterView->initialize(anatomicalGroups,attributes,spikeGroups,spikeGroupsInformation,units,generalInformation,
                              acquisitionSystemInfo,videoInformation,files,channelColors,channelOffsets,neuroscopeVideoInfo,programs,lfpRate,screenGain,nbSamples,peakSampleIndex,traceBackgroundImage);

    mainDock->setWidget(parameterView);
    //allow dock on the Bottom side only
    setCentralWidget(parameterView); // central widget in a KDE mainwindow <=> setMainWidget
    //Enable some actions now that a document is open (see the klustersui.rc file)
    slotStateChanged("documentState");

}

void ndManager::slotImport(){
    slotStatusMsg(tr("importing file as model..."));
    importedFile = true;

    QSettings settings;
    const QString url = QFileDialog::getOpenFileName(this, tr("Import file as model..."),settings.value("CurrentDirectory").toString(),
                                                     tr("Parameter File (*.xml);;All files (*.*)") );
    if(!url.isEmpty()) {
        QDir CurrentDir;
        settings.setValue("CurrentDirectory", CurrentDir.absoluteFilePath(url));
        openDocumentFile(url);
    }

    importedFileUrl = url + QDir::separator() + "Untitled";

    doc->rename(url);
    filePath = url;
    setWindowTitle(url);

    slotStatusMsg(tr("Ready."));
}

void ndManager::slotFileClose(){
    if(doc != 0 ){
        bool hasBeenCancel = false;

        //check first if some scripts have been modified
        QStringList scriptModified = parameterView->modifiedScripts();
        if(!scriptModified.isEmpty()) {
            Q_FOREACH(const QString& name, scriptModified ) {
                switch(QMessageBox::question(0,tr("Plugin modification"),tr("The plugin %1 has been modified, do you want to save the it?").arg(name),QMessageBox::Save|QMessageBox::Discard|QMessageBox::Cancel)){
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
        QStringList programModified = parameterView->modifiedProgramDescription();
        if(!programModified.isEmpty()){
            Q_FOREACH(const QString& name, programModified ) {
                switch(QMessageBox::question(this,tr("Program description modification"),tr("The description of the program %1 has been modified, do you want to save the it?").arg(name),QMessageBox::Save|QMessageBox::Discard|QMessageBox::Cancel)){
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
                switch(QMessageBox::question(this,tr("Parameters modification"),tr("Some parameters have changed, do you want to save the parameter file?"),QMessageBox::Save|QMessageBox::Discard|QMessageBox::Cancel)){
                case QMessageBox::Save://<=> Save
                    slotSave();
                    break;
                case QMessageBox::Discard://<=> Discard
                    break;
                case QMessageBox::Cancel:
                    return;
                }
            }
            delete parameterView;
            parameterView = 0;
            //close the document
            doc->closeDocument();
            //Delete the main view
            delete mainDock;
            mainDock = 0L;
            resetState();
        }
    }
}

bool ndManager::queryClose()
{
    if(doc == 0 || mainDock == 0L)
        return true;
    else{
        //check first if some scripts have been modified
        const QStringList scriptModified = parameterView->modifiedScripts();
        if( !scriptModified.isEmpty() ){
            Q_FOREACH(const QString& name, scriptModified) {
                switch(QMessageBox::question(this,tr("Plugin modification"),tr("The plugin %1 has been modified, do you want to save the it?").arg(name),QMessageBox::Save|QMessageBox::Discard|QMessageBox::Cancel)){
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
        const QStringList programModified = parameterView->modifiedProgramDescription();
        if(!programModified.isEmpty()){
            Q_FOREACH(const QString& name, programModified) {
                switch(QMessageBox::question(this,tr("Program description modification"),tr("The description of the program %1 has been modified, do you want to save the it?").arg(name),QMessageBox::Save|QMessageBox::Discard|QMessageBox::Cancel)){
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
            switch(QMessageBox::question(this,tr("Parameters modification"),tr("Some parameters have changed, do you want to save the parameter file?"),QMessageBox::Save|QMessageBox::Discard|QMessageBox::Cancel)){
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
        if(newFile)
            initialPath = QDir::currentPath();
        else{
            QString currentUrl =  doc->url();
            initialPath = QFileInfo(currentUrl).absolutePath();
        }

        const QString url=QFileDialog::getSaveFileName( this, tr("Save as..."),initialPath,tr("Xml Files (*.xml)|All Files (*.*)"));
        if(!url.isEmpty()){
            QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
            int saveStatus = doc->saveAs(url);
            if(saveStatus == ndManagerDoc::SAVE_ERROR){
                QMessageBox::critical(this, tr("IO Error!"),tr("The current file could not be saved possibly because of insufficient file access permissions."
                                                               " You may consider saving your session file to another location."));
            }
            if(importedFile || newFile){
                filePath = url;
                setWindowTitle(url);
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
            QMessageBox::critical(this, tr("IO Error!"),tr("The current file could not be saved possibly because of insufficient file access permissions."
                                                           " You may consider saving your session file to another location using the Save As entry in the File menu."));
        }
    }
    slotStatusMsg(tr("Ready."));
}

void ndManager::slotSaveAs(){
    slotStatusMsg(tr("Saving as..."));

    //Save the parameter file
    QString url=QFileDialog::getSaveFileName(this, tr("Save as..."),doc->url(),tr("All files (*.*)"));
    if(!url.isEmpty()){
        int saveStatus = doc->saveAs(url);
        if(saveStatus == ndManagerDoc::SAVE_ERROR){
            QMessageBox::critical(0,tr("The current file could not be saved possibly because of insufficient file access permissions."), tr("I/O Error !"));
        }
        filePath = url;
        setWindowTitle(url);

        importedFile = false;
        newFile = false;
    }
    slotStatusMsg(tr("Ready."));
}
void ndManager::resetState(){
    //Disable some actions when no document is open (see the klustersui.rc file)
    slotStateChanged("initState");
    setWindowTitle("");
    importedFile = false;
    newFile = false;
    managerView = 0L;
}

void ndManager::slotSaveDefault(){
    slotStatusMsg(tr("Saving as default..."));
    int saveStatus = doc->saveDefault();
    if(saveStatus == ndManagerDoc::SAVE_ERROR){
        QMessageBox::critical(this, tr("IO Error!"),tr("The current file could not be saved as the default parameter file."));
    }
    slotStatusMsg(tr("Ready."));
}

void ndManager::slotReload(){
    slotStatusMsg(tr("reloading..."));

    //Get the current active page index
    int currentPage = parameterView->currentPage();

    //close the current document
    doc->closeDocument();
    //Delete the main view
    delete mainDock;
    mainDock = 0L;
    resetState();

    //Reopen the document

    QFileInfo file(filePath);

    if(!file.exists()){
        QMessageBox::critical (this, tr("Sorry!"),tr("The selected parameter file could not be reloaded as it appears to have been removed from disk."));
        slotStatusMsg(tr("Ready."));
        return;
    }

    //Open the file (that will also initialize the document)
    const int returnStatus = doc->openDocument(filePath);
    if(returnStatus == ndManagerDoc::PARSE_ERROR){
        QApplication::restoreOverrideCursor();
        QMessageBox::critical (this, tr("IO Error!"),tr("The selected parameter file could not be initialize due to parsing error."));
        //close the document
        doc->closeDocument();
        resetState();
        slotStatusMsg(tr("Ready."));
        return;
    }

    setWindowTitle(filePath);
    QApplication::restoreOverrideCursor();

    //Raise the previously active page
    parameterView->setCurrentPage(currentPage);

    slotStatusMsg(tr("Ready."));
}

void ndManager::slotQuery(){
#ifdef Q_OS_UNIX
    slotStatusMsg(tr("Processing query..."));
    QPointer<QueryInputDialog> queryInputDialog = new QueryInputDialog();
    if(queryInputDialog->exec() == QDialog::Accepted)
    {
        // Run query
        QProcess process;

        //Not portable
        //process.setUseShell(true);
        const QString program("find");
        QStringList arguments;
        arguments<<queryInputDialog->getPath();
        arguments<<QString::fromLatin1(" -name '*xml' -exec xpathReader --html {} \"%1\" \\; | sed 'N;s/<tr>/<tr class=\"tr1\">/;s/<tr>/<tr class=\"tr2\">/'").arg(queryInputDialog->getQuery());
        //process << "find " + queryInputDialog->getPath() + " -name '*xml' -exec xpathReader --html {} \"" + queryInputDialog->getQuery() + "\" \\; | sed 'N;s/<tr>/<tr class=\"tr1\">/;s/<tr>/<tr class=\"tr2\">/'";
        process.start(program, arguments);
        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
        if (!process.waitForFinished()) {
            //TODO
            delete queryInputDialog;
            return;
        }
        QString queryResult = process.readAll();
        QApplication::restoreOverrideCursor();

        // Read HTML footer from file
        QFile htmlFile(":/queryreport.html");
        if(htmlFile.open(QIODevice::ReadOnly))
        {
            QTextStream stream(&htmlFile);
            html = stream.readAll();
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
        QueryOutputDialog queryOutputDialog(html,queryResult);
        queryOutputDialog.exec();
    }
    delete queryInputDialog;
    slotStatusMsg(tr("Ready."));
#endif
}

void ndManager::slotExpertMode(){
    QSettings settings;
    settings.beginGroup("General");
    settings.setValue("expertMode",mExpertMode->isChecked());
    settings.endGroup();

    bool isImportedFile = importedFile;
    bool isNewFile = newFile;

    //close the file and reopen it in the new mode
    slotFileClose();

    if(isNewFile && filePath.contains("Untitled")) {
        slotNewFile();
    } else if(isImportedFile && filePath.contains("Untitled")){
        QString url = importedFileUrl;
        openDocumentFile(url);
        url+= QDir::separator() + tr("Untitled");
        doc->rename(url);
        filePath = url;
        setWindowTitle(url);
    } else {
        openDocumentFile(filePath);
    }
}

void ndManager::nbSpikeGroupsModified(int nbGroups){
    if(managerView){
        //managerView->updateSpikeGroupList(nbGroups);
    }
}

void ndManager::fileModification(const QStringList& extensions){
    if(managerView){
        //managerView->updateFileList(extensions);
    }
}
#if 0
void ndManager::scriptModification(const QStringList& scriptNames){
    if(managerView){
        managerView->updateScriptList(scriptNames);
    }
}
#endif
void ndManager::checkBeforeLaunchingPrograms(){
    if(managerView){
        if(importedFile || newFile) {
            managerView->updateDocumentInformation(doc->url(),false);
        } else {
            if(parameterView->isModified()) {
                managerView->updateDocumentInformation(doc->url(),false);
            } else {
                const QStringList programModified = parameterView->modifiedProgramDescription();
                if(!programModified.isEmpty())
                    managerView->updateDocumentInformation(doc->url(),false);
                else
                    managerView->updateDocumentInformation(doc->url(),true);
            }
        }
    }
}

void ndManager::checkBeforeLaunchingScripts(){
    if(managerView){
        if(importedFile || newFile) {
            managerView->updateDocumentInformation(doc->url(),false);
        } else {
            if(parameterView->isModified()) {
                managerView->updateDocumentInformation(doc->url(),false);
            } else {
                const QStringList programModified = parameterView->modifiedProgramDescription();
                if(!programModified.isEmpty()) {
                    managerView->updateDocumentInformation(doc->url(),false);
                } else {
                    const QStringList scriptModified = parameterView->modifiedScripts();
                    if(!scriptModified.isEmpty())
                        managerView->updateDocumentInformation(doc->url(),false);
                    else
                        managerView->updateDocumentInformation(doc->url(),true);
                }
            }
        }
    }
}


void ndManager::slotStateChanged(const QString& state)
{
    if(state == QLatin1String("initState")) {
        mReloadAction->setEnabled(false);
        mSaveAsAction->setEnabled(false);
        mSaveAction->setEnabled(false);
        mCloseAction->setEnabled(false);
        mSaveAsDefaultAction->setEnabled(false);
        //mProcessingManager->setEnabled(false);
        mExpertMode->setEnabled(false);
        mOpenAction->setEnabled(true);
        mNewAction->setEnabled(true);
        mFileOpenRecent->setEnabled(true);
        mUseTemplateAction->setEnabled(true);
        mQueryAction->setEnabled(true);
    } else if(state == QLatin1String("documentState")) {
        mReloadAction->setEnabled(true);
        mSaveAsAction->setEnabled(true);
        mSaveAction->setEnabled(true);
        mCloseAction->setEnabled(true);
        mSaveAsDefaultAction->setEnabled(true);
        //mProcessingManager->setEnabled(true);
        mExpertMode->setEnabled(true);
    } else if(state == QLatin1String("showManager")) {
        //mProcessingManager->setEnabled(false);
    } else if(state == QLatin1String("hideManger")) {
        //mProcessingManager->setEnabled(false);
    } else {
        qDebug()<<" state unknown"<<state;
    }

}

void ndManager::slotAbout()
{
    QMessageBox::about(this,tr("About - NdManager"),tr("Neurophysiological Data Processing Manager\n(C) 2004-2007 Lynn Hazan"));
}


void ndManager::slotHanbook()
{
    QHelpViewer *helpDialog = new QHelpViewer(this);
    helpDialog->setHtml(NDMANAGER_DOC_PATH + QLatin1String("index.html"));
    helpDialog->setAttribute( Qt::WA_DeleteOnClose );
    helpDialog->show();
}

void ndManager::slotSaveRecentFiles()
{
    QSettings settings;
    settings.setValue(QLatin1String("Recent Files"),mFileOpenRecent->recentFiles());
}

void ndManager::slotQuit()
{
    if (!queryClose())
        return;
    close();
}

void ndManager::readSettings()
{
    QSettings settings;
    settings.beginGroup("geometry");
    restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("windowState").toByteArray());
    settings.endGroup();
}

