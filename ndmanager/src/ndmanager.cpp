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
#include <Q3TextStream>
#include <Q3ValueList>
#include <Q3PtrList>
#include <QPixmap>
#include <QFileDialog>

// include files for KDE


#include <kdeversion.h>
#include <QStatusBar>
#include <QProcess>




#include <kstandarddirs.h>

// application specific includes
#include "ndmanager.h"
#include "configuration.h"  // class Configuration
#include "queryinputdialog.h"
#include "queryoutputdialog.h"

//General C++ include files
#include <iostream>
using namespace std;

ndManager::ndManager():KDockMainWindow(0, "NDManager"),prefDialog(0L),mainDock(0),filePath(""),
                       importedFile(false),newFile(false),managerView(0L){
 //Gets the configuration object of the application throught the static reference to the application kapp
  config = kapp->config();

   //Apply the user settings.
  initializePreferences();

  //Create a ndManagereDoc which will hold the document manipulated by the application.
  doc = new ndManagerDoc(this);

  //Setup the actions
  setupActions();

  initStatusBar();

  // Apply the saved mainwindow settings, if any, and ask the mainwindow
  // to automatically save settings if changed: window size, toolbar
 // position, icon size, etc.
  setAutoSaveSettings();

  // initialize the recent file list
  fileOpenRecent->loadEntries(config);

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

 KStdAction::open(this, SLOT(slotFileOpen()), actionCollection());
 KStdAction::openNew(this, SLOT(slotNewFile()), actionCollection());
 fileOpenRecent = KStdAction::openRecent(this, SLOT(slotFileOpenRecent(const QString&)), actionCollection());
 new KAction(tr("&Close"), "fileclose",0,this, SLOT(slotFileClose()),actionCollection(), "file_close");
 KStdAction::save(this, SLOT(slotSave()), actionCollection());
 KStdAction::saveAs(this, SLOT(slotSaveAs()), actionCollection());

 KStdAction::quit(this, SLOT(close()), actionCollection());
 viewMainToolBar = KStdAction::showToolbar(this, SLOT(slotViewMainToolBar()), actionCollection());
 viewStatusBar = KStdAction::showStatusbar(this, SLOT(slotViewStatusBar()), actionCollection());
// KStdAction::preferences(this,SLOT(executePreferencesDlg()), actionCollection());

  //Custom actions and menus
 //File menu
 new KAction(tr("Use &Template..."),0,this,SLOT(slotImport()),actionCollection(),"import");
 new KAction(tr("&Reload"),"F5",this,SLOT(slotReload()),actionCollection(),"reload");
 new KAction(tr("Save as &Default"),0,this,SLOT(slotSaveDefault()),actionCollection(),"save_as_default");

 //the Query menu
 new KAction(tr("&Query"),0,this,SLOT(slotQuery()),actionCollection(),"query");

 //Processing menu
 new KAction(tr("Show Processing Manager"),0,this,SLOT(createManagerView()),actionCollection(),"processingManager");

 //Settings
 expertMode = new KToggleAction(tr("&Expert Mode"),0,this,SLOT(slotExpertMode()),actionCollection(),"expert");
 config->setGroup("General");
 expertMode->setChecked(config->readBoolEntry("expertMode"));


 createGUI(QString(),false);

}

void ndManager::initStatusBar()
{
  statusBar()->insertItem(tr("Ready."),1);
}

void ndManager::saveProperties()
{
#if KDAB_PENDING
  //Save the recent file list
  fileOpenRecent->saveEntries(config);
  config->writePathEntry("openFile",filePath);

  //Save the curent mode
  this->config->setGroup("General");
  this->config->writeEntry("expertMode",expertMode->isChecked());
#endif
}

void ndManager::readProperties()
{
#if KDAB_PENDING
 // initialize the recent file list
  fileOpenRecent->loadEntries(config);
  filePath = config->readPathEntry("openFile");

  //Read out the mode
  this->config->setGroup("General");
  expertMode->setChecked(this->config->readBoolEntry("expertMode"));

  QString url;
  url.setPath(filePath);
  openDocumentFile(url);
#endif
}

void ndManager::slotStatusMsg(const QString &text)
{
  ///////////////////////////////////////////////////////////////////
  // change status message permanently
  statusBar()->clear();
  statusBar()->changeItem(text,1);
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
   KMessageBox::error (this,tr("The new parameter file could not be initialize due to parsing error."), tr("Error!"));
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

  if(url.protocol() == "file"){
   QFileInfo file(filePath);
   if((fileOpenRecent->items().contains(url.prettyURL())) && !file.exists()){
    QString title = "File not found: ";
    title.append(filePath);
    int answer = KMessageBox::questionYesNo(this,tr("The selected file no longer exists. Do you want to remove it from the list of recent opened files ?"), tr(title));
    if(answer == KMessageBox::Yes){
     QString* urlB = new QString();
     urlB->setPath(url.url());
     fileOpenRecent->removeURL(url);
    }
    else  fileOpenRecent->addURL(url); //hack, unselect the item
    filePath = "";

    return;
   }
  }
  //Do not handle remote files
  else{
   KMessageBox::sorry(this,tr("Sorry, NDManager does not handle remote files."),tr("Remote file handling"));
   fileOpenRecent->addURL(url); //hack, unselect the item
   return;
  }

  //Check if the file exists
  if(!KStandardDirs::exists(url)){
   KMessageBox::error (this,tr("The selected file does not exist."), tr("Error!"));
   fileOpenRecent->removeURL(url);
   return;
  }

  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

  //If no document is open already, open the document asked.
  if(!mainDock){
   fileOpenRecent->addURL(url);

    //Open the file (that will also initialize the document)
   int returnStatus = doc->openDocument(url);
   if(returnStatus == ndManagerDoc::PARSE_ERROR){
     QApplication::restoreOverrideCursor();
     KMessageBox::error (this,tr("The selected parameter file could not be initialize due to parsing error."), tr("Error!"));
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
    fileOpenRecent->addURL(url); //hack, unselect the item
    QApplication::restoreOverrideCursor();
    return;
   }
   //If the document asked is not the already open. Open a new instance of the application with it.
   else{
    fileOpenRecent->addURL(url);
    //Save the recent file list
    fileOpenRecent->saveEntries(config);
    filePath = path;

    QProcess::startDetached("ndmanager", QStringList()<<url);
    QApplication::restoreOverrideCursor();
   }
  }

  slotStatusMsg(tr("Ready."));
}

void ndManager::createParameterView(QMap<int, Q3ValueList<int> >& anatomicalGroups,QMap<QString, QMap<int,QString> >& attributes,
                          QMap<int, Q3ValueList<int> >& spikeGroups,QMap<int, QMap<QString,QString> >& spikeGroupsInformation,QMap<int, Q3ValueList<QString> >& units,
                          GeneralInformation& generalInformation,QMap<QString,double>& acquisitionSystemInfo,QMap<QString,double>& videoInformation,
                          Q3ValueList<FileInformation>& files,Q3ValueList<ChannelColors>& channelColors,QMap<int,int>& channelOffsets,
                          NeuroscopeVideoInfo& neuroscopeVideoInfo,Q3ValueList<ProgramInformation>& programs,
                           double lfpRate,float screenGain,int nbSamples,int peakSampleIndex,QString traceBackgroundImage){

 //Create the mainDock (parameter view)
  mainDock = createDockWidget( "1", QPixmap(), 0L, tr("Parameters"), tr("Parameters"));
  mainDock->setDockWindowTransient(this,true);

  parameterView = new ParameterView(this,*doc,mainDock,tr("ParameterView"),expertMode->isChecked());

  connect(parameterView,SIGNAL(partShown(Kate::View*)),this,SLOT(updateGUI(Kate::View*)));
  connect(parameterView,SIGNAL(partHidden()),this,SLOT(updateGUI()));
  connect(parameterView,SIGNAL(nbSpikeGroupsHasBeenModified(int)),this,SLOT(nbSpikeGroupsModified(int)));
  connect(parameterView,SIGNAL(fileHasBeenModified(Q3ValueList<QString>)),this,SLOT(fileModification(Q3ValueList<QString>)));
  connect(parameterView,SIGNAL(scriptListHasBeenModified(const Q3ValueList<QString>&)),this,SLOT(scriptModification(const Q3ValueList<QString>&)));



  parameterView->initialize(anatomicalGroups,attributes,spikeGroups,spikeGroupsInformation,units,generalInformation,
                           acquisitionSystemInfo,videoInformation,files,channelColors,channelOffsets,neuroscopeVideoInfo,programs,lfpRate,screenGain,nbSamples,peakSampleIndex,traceBackgroundImage);

  mainDock->setWidget(parameterView);
  //allow dock on the Bottom side only
  mainDock->setDockSite(KDockWidget::DockBottom);
  setView(mainDock); // central widget in a KDE mainwindow <=> setMainWidget
  setMainDockWidget(mainDock);
  //disable docking abilities of mainDock itself
  mainDock->setEnableDocking(KDockWidget::DockNone);

  //show all the encapsulated widgets of all controlled dockwidgets
  dockManager->activate();

  //Enable some actions now that a document is open (see the klustersui.rc file)
  slotStateChanged("documentState");

}

void ndManager::createManagerView(){
 if(managerView == 0L){
   //Create and add the ManagerView
  KDockWidget* manager = createDockWidget("Manager", QPixmap());
  managerView = new ManagerView();

  int returnStatus =  managerView->addKonsole(doc->url(),parameterView->getNbGroups(),parameterView->getFileExtensions(),
   parameterView->getFileScriptNames());
  if(returnStatus == ManagerView::NO_KPART){
   KMessageBox::error (this,tr("The Konsole part does not exist, no terminal can be created."), tr("IO Error!"));
   return;
  }
  if(returnStatus == ManagerView::PART_LOADING_ERROR){
   KMessageBox::error (this,tr("The Konsole part could not be loaded."), tr("IO Error!"));
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
  manager->manualDock(mainDock,KDockWidget::DockBottom,25);
  manager->setEnableDocking(KDockWidget::DockCorner);
  manager->setDockSite(KDockWidget::DockCorner);
 }
 else{
  managerView->addKonsole(doc->url(),parameterView->getNbGroups(),parameterView->getFileExtensions(),parameterView->getFileScriptNames());
  KDockWidget* manager = dockManager->findWidgetParentDock(managerView);
  manager->manualDock(mainDock,KDockWidget::DockBottom,25);
  manager->setEnableDocking(KDockWidget::DockCorner);
  manager->setDockSite(KDockWidget::DockCorner);
 }

 //show all the encapsulated widgets of all controlled dockwidgets
 dockManager->activate();
 slotStateChanged("hideManger");
}

void ndManager::konsoleDockBeingClosed(){
 KDockWidget* dock = dockManager->findWidgetParentDock(managerView);
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
  Q3ValueList<QString> scriptModified = parameterView->modifiedScripts();
  if(scriptModified.size() != 0){
   Q3ValueList<QString>::iterator iterator;
   for(iterator = scriptModified.begin(); iterator != scriptModified.end(); ++iterator){
    QString name = *iterator;
    switch(KMessageBox::warningYesNoCancel(0,tr("The script " + name + " has been modified, do you want to save the it?"),tr("Script modification"),KGuiItem("Save"),KGuiItem("Discard"))){
     case KMessageBox::Yes://<=> Save
      parameterView->saveScript(name);
      break;
     case KMessageBox::No://<=> Discard
      break;
     case KMessageBox::Cancel:
      hasBeenCancel = true;
      break;
    }
   }
  }

  //check if some descriptions have been modified
  Q3ValueList<QString> programModified = parameterView->modifiedProgramDescription();
  if(programModified.size() != 0){
   Q3ValueList<QString>::iterator iterator;
   for(iterator = programModified.begin(); iterator != programModified.end(); ++iterator){
    QString name = *iterator;
    switch(KMessageBox::warningYesNoCancel(0,tr("The description of the program " + name + " has been modified, do you want to save the it?"),tr("Program description modification"),KGuiItem("Save"),KGuiItem("Discard"))){
     case KMessageBox::Yes://<=> Save
      parameterView->saveProgramDescription(name);
      break;
     case KMessageBox::No://<=> Discard
      break;
     case KMessageBox::Cancel:
      hasBeenCancel = true;
      break;
    }
   }
  }

  if(!hasBeenCancel){
   //if at least one parameter has been modified, prompt the user to save the information
   if(parameterView->isModified()){
     switch(KMessageBox::warningYesNoCancel(0,tr("Some parameters have changed, do you want to save the parameter file?"),tr("Parameters modification"),KGuiItem("Save"),KGuiItem("Discard"))){
      case KMessageBox::Yes://<=> Save
       slotSave();
       break;
      case KMessageBox::No://<=> Discard
       break;
      case KMessageBox::Cancel:
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
    KDockWidget* dock = dockManager->findWidgetParentDock(managerView);
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
   Q3ValueList<QString> scriptModified = parameterView->modifiedScripts();
   if(scriptModified.size() != 0){
    Q3ValueList<QString>::iterator iterator;
    for(iterator = scriptModified.begin(); iterator != scriptModified.end(); ++iterator){
     QString name = *iterator;
     switch(KMessageBox::warningYesNoCancel(0,tr("The script " + name + " has been modified, do you want to save the it?"),tr("Script modification"),KGuiItem("Save"),KGuiItem("Discard"))){
      case KMessageBox::Yes://<=> Save
       parameterView->saveScript(name);
       break;
      case KMessageBox::No://<=> Discard
       break;
      case KMessageBox::Cancel:
       return false;
       break;
     }
    }
   }

  //check if some descriptions have been modified
  Q3ValueList<QString> programModified = parameterView->modifiedProgramDescription();
  if(programModified.size() != 0){
   Q3ValueList<QString>::iterator iterator;
   for(iterator = programModified.begin(); iterator != programModified.end(); ++iterator){
    QString name = *iterator;
    switch(KMessageBox::warningYesNoCancel(0,tr("The description of the program " + name + " has been modified, do you want to save the it?"),tr("Program description modification"),KGuiItem("Save"),KGuiItem("Discard"))){
     case KMessageBox::Yes://<=> Save
      parameterView->saveProgramDescription(name);
      break;
     case KMessageBox::No://<=> Discard
      break;
     case KMessageBox::Cancel:
      return false;
      break;
    }
   }
  }

   //if at least one parameter has been modified, prompt the user to save the information
   if(parameterView->isModified()){
    switch(KMessageBox::warningYesNoCancel(0,tr("Some parameters have changed, do you want to save the parameter file?"),tr("Parameters modification"),KGuiItem("Save"),KGuiItem("Discard"))){
     case KMessageBox::Yes://<=> Save
      slotSave();
      break;
     case KMessageBox::No://<=> Discard
      break;
     case KMessageBox::Cancel:
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
    KMessageBox::error(0,tr("The current file could not be saved possibly because of insufficient file access permissions."
    " You may consider saving your session file to another location."), tr("I/O Error !"));
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
   KMessageBox::error(0,tr("The current file could not be saved possibly because of insufficient file access permissions."
   " You may consider saving your session file to another location using the Save As entry in the File menu."), tr("I/O Error !"));
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
   KMessageBox::error(0,tr("The current file could not be saved possibly because of insufficient file access permissions."), tr("I/O Error !"));
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
        KMessageBox::error(0,tr("The current file could not be saved as the default parameter file."), tr("I/O Error !"));
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
		KDockWidget* dock = dockManager->findWidgetParentDock(managerView);
		dock->undock();
		dock->hide();
		delete dock;
	}
	resetState();

	//Reopen the document

    QString url = QString(filePath);

	QFileInfo file(filePath);

	if(!file.exists()){
        KMessageBox::sorry (this,tr("The selected parameter file could not be reloaded as it appears to have been removed from disk."), tr("Sorry!"));
        slotStatusMsg(tr("Ready."));
		return;
	}

    //Open the file (that will also initialize the document)
	int returnStatus = doc->openDocument(url);
	if(returnStatus == ndManagerDoc::PARSE_ERROR){
		QApplication::restoreOverrideCursor();
        KMessageBox::error (this,tr("The selected parameter file could not be initialize due to parsing error."), tr("Error!"));
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
		process.setUseShell(true);
		process << "find " + queryInputDialog->getPath() + " -name '*xml' -exec xpathReader --html {} \"" + queryInputDialog->getQuery() + "\" \\; | sed 'N;s/<tr>/<tr class=\"tr1\">/;s/<tr>/<tr class=\"tr2\">/'";
        connect(&process,SIGNAL(receivedStdout(QProcess*,char*,int)),this,SLOT(slotQueryResult(QProcess*,char*,int)));
		QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
        process.start(QProcess::Block,QProcess::Stdout);
		QApplication::restoreOverrideCursor();

		// Read HTML footer from file
		KStandardDirs d;
		QFile htmlFile(d.findResource("data","ndmanager/queryreport.html"));
		if(htmlFile.open(QIODevice::ReadOnly))
		{
			Q3TextStream stream(&htmlFile);
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

void ndManager::updateGUI(Kate::View* view){
 Q3PtrList<KXMLGUIClient> clients = guiFactory()->clients();

 //A part has already be added, remove it to add the current one
 if(clients.count() > 1){
  guiFactory()->removeClient(clients.at(1));
  clients = guiFactory()->clients();
 }

 //Load the ui.rc file only once
 if(kateXmlDocument.toString().isEmpty()){
  QString path = locate("appdata","ndmanager_katepartui.rc");
  QFile file(path);

  if(!file.open(QIODevice::ReadOnly)) return;
  //actually load the file in a tree in  memory
  if(!kateXmlDocument.setContent(&file)){
   file.close();
   return;
  }
  file.close();
 }

 //set the ndmanger version of katepartui.rc as the XMLGUI for the current view.
 view->setXMLGUIBuildDocument(kateXmlDocument);

 if(clients.count() == 1) guiFactory()->addClient(view);
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

void ndManager::fileModification(Q3ValueList<QString> extensions){
 if(managerView != 0L){
  managerView->updateFileList(extensions);
 }
}

void ndManager::scriptModification(const Q3ValueList<QString>& scriptNames){
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
    Q3ValueList<QString> programModified = parameterView->modifiedProgramDescription();
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
    Q3ValueList<QString> programModified = parameterView->modifiedProgramDescription();
    if(programModified.size()!= 0) managerView->updateDocumentInformation(doc->url(),false);
    else{
     Q3ValueList<QString> scriptModified = parameterView->modifiedScripts();
     if(scriptModified.size()!= 0) managerView->updateDocumentInformation(doc->url(),false);
     else managerView->updateDocumentInformation(doc->url(),true);
    }
   }
  }
 }
}





#include "ndmanager.moc"
