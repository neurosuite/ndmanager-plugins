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

//include files for the application
#include "programpage.h"
#include "parameterpage.h"
#include "descriptionwriter.h"

// include files for QT
#include <qlayout.h>
#include <qtabwidget.h>
#include <qpushbutton.h>
#include <qfileinfo.h>
#include <qtextstream.h>
#include <qapplication.h>
#include <qregexp.h>
#include <qtextedit.h>
// include files for KDE
#include <klocale.h>        // for i18n()
#include <kiconloader.h>    // for KIconLoader
#include <ktextedit.h>
#include <kstandarddirs.h>
#include <kmessagebox.h>
#include <kfiledialog.h>

#include <ktexteditor/document.h>
#include <ktexteditor/view.h>
#include <kate/document.h>

//General C++ include files
#include <iostream>
using namespace std;

ProgramPage::ProgramPage(bool expertMode,QWidget *parent, const char* name)
 : QFrame(parent, name),sciptIsModified(false),isInit(true),programName(name),helpIsModified(false),descriptionNotSaved(true),expertMode(expertMode){
 QVBoxLayout* frameLayout = new QVBoxLayout(this,0,0);

 //Creat the upper part containing a tabWidget with 3 tabs, one with the parameters (ParameterPage), one with the script and one with the help.
 //In expert mode, the script tab does not exist.

 tabWidget = new QTabWidget(this);
// script = new KTextEdit(tabWidget);
 help = new QTextEdit(tabWidget);
 if(!expertMode) help->setReadOnly(true);
 parameters = new ParameterPage(expertMode,tabWidget);
 tabWidget->addTab(parameters,i18n("Parameters"));

 tabWidget->addTab(help,i18n("Help"));
 connect(tabWidget, SIGNAL(currentChanged(QWidget*)), this, SLOT(tabChange(QWidget*)));

 if(expertMode){
  editorMgr = new EditorManager(this);
  // Load a new Kate document part
  scriptDoc = editorMgr->add();
  if(scriptDoc != NULL){
   scriptView = static_cast<Kate::View*>(scriptDoc->createView(tabWidget));
   tabWidget->addTab(scriptView,i18n("Script"));
  }
 }

 frameLayout->addWidget(tabWidget);

 //Add the buttons
 QWidget* buttons = new QWidget(this);
 QGridLayout* gridLayout = new QGridLayout(buttons,1,1,0,6);
 frameLayout->addWidget(buttons);

 if(expertMode){
  saveParametersButton = new QPushButton(i18n("Save Script Description As ..."),buttons);
  saveParametersButton->setSizePolicy(
   QSizePolicy((QSizePolicy::SizeType)0,(QSizePolicy::SizeType)0,0,0,saveParametersButton->sizePolicy().hasHeightForWidth()));
  saveParametersButton->setMinimumSize(QSize(200,0));
  saveParametersButton->setMaximumSize(QSize(300,32767));
  gridLayout->addWidget(saveParametersButton,0,1);

  saveScriptButton = new QPushButton(i18n("Save Script As ..."),buttons);
  saveScriptButton->setSizePolicy(
    QSizePolicy((QSizePolicy::SizeType)0,(QSizePolicy::SizeType)0,0,0,saveScriptButton->sizePolicy().hasHeightForWidth()));
  saveScriptButton->setMinimumSize(QSize(124,0));
  saveScriptButton->setMaximumSize(QSize(130,32767));
  gridLayout->addWidget(saveScriptButton,0,3);

  removeButton = new QPushButton(i18n("Remove Script"),buttons);
  removeButton->setSizePolicy(QSizePolicy((QSizePolicy::SizeType)0,(QSizePolicy::SizeType)0,0,0,removeButton->sizePolicy().hasHeightForWidth()));
  removeButton->setMinimumSize(QSize(124,0));
  removeButton->setMaximumSize(QSize(124,32767));
  gridLayout->addWidget(removeButton,0,5);

  QSpacerItem* space1 = new QSpacerItem(112,16,QSizePolicy::Expanding,QSizePolicy::Minimum);
  gridLayout->addItem(space1,0,0);
  QSpacerItem* space2 = new QSpacerItem(29,16,QSizePolicy::Fixed,QSizePolicy::Minimum);
  gridLayout->addItem(space2,0,2);
  QSpacerItem* space3 = new QSpacerItem(29,16,QSizePolicy::Fixed,QSizePolicy::Minimum);
  gridLayout->addItem(space3,0,4);
  QSpacerItem* space4 = new QSpacerItem(112,16,QSizePolicy::Expanding,QSizePolicy::Minimum);
  gridLayout->addItem(space4,0,6);

  connect(saveScriptButton,SIGNAL(clicked()),this,SLOT(saveProgramScript()));
  connect(saveParametersButton,SIGNAL(clicked()),this,SLOT(saveProgramParameters()));
  connect(scriptView->getDoc(),SIGNAL(textChanged()),this,SLOT(scriptModified()));
  connect(help,SIGNAL(textChanged()),this,SLOT(helpModified()));
 }
 else{
  removeButton = new QPushButton(i18n("Remove Script"),buttons);
  removeButton->setSizePolicy(QSizePolicy((QSizePolicy::SizeType)0,(QSizePolicy::SizeType)0,0,0,removeButton->sizePolicy().hasHeightForWidth()));
  removeButton->setMinimumSize(QSize(124,0));
  removeButton->setMaximumSize(QSize(124,32767));
  gridLayout->addWidget(removeButton,0,1);

  QSpacerItem* space1 = new QSpacerItem(112,16,QSizePolicy::Expanding,QSizePolicy::Minimum);
  gridLayout->addItem(space1,0,0);
  QSpacerItem* space2 = new QSpacerItem(112,16,QSizePolicy::Expanding,QSizePolicy::Minimum);
  gridLayout->addItem(space2,0,2);
 }

 connect(removeButton,SIGNAL(clicked()),this,SLOT(removeProgram()));
 connect(parameters,SIGNAL(nameChanged(const QString&)),this,SLOT(nameChanged(const QString&)));
}


ProgramPage::~ProgramPage(){
}

QString ProgramPage::getHelp(){return help->text();}

void ProgramPage::setHelp(QString helpContent){
 help->setText(helpContent);
}

bool ProgramPage::saveProgramScript(){
 bool recall = false;
 //find the file corresponding to the program name
 QString name = parameters->getProgramName();
 QString path = KStandardDirs::findExe(name,getenv("PATH"),true);
 QString message = "";
 QString title = "";
 KURL scriptUrl;

 if(!path.isNull()) scriptUrl = KFileDialog::getSaveURL(path,i18n("*"), this, i18n("Save as..."));
 else scriptUrl = KFileDialog::getSaveURL(QString::null,i18n("*"), this, i18n("Save as..."));

 if(!scriptUrl.isEmpty()){
  path = scriptUrl.path();
  QFileInfo fileInfo(path);
  QFile file(path);
  if(!file.open(IO_WriteOnly)){
   message = QString("The file %1 could not be saved possibly because of insufficient file access permissions.").arg(name);
   title = "IO Error!";
   recall = true;
  }
  else{
   QTextStream stream(&file);
   stream<<scriptView->getDoc()->text();
   file.close();
  }
 }

 //If the change in the programName lineedit has not yet been validated, a call to nameChanged will be done after this call. Therefore
 //programName has to be updated so no message will be display in that call (otherwise there will be conflict in the events)
 if(programName != parameters->getProgramName()) programName = parameters->getProgramName();
 if(title != "") KMessageBox::error (this,i18n(message), i18n(title));
 if(recall) return saveProgramScript();
 else{
  if(title != "") return false;
  else{
   sciptIsModified = false;
   return true;
  }
 }
}

void ProgramPage::saveProgramParameters(){
 //Always ask the user where to save the information. If as save has already be done, set the save location as the default location
 //for the new save
 if(descriptionUrl.isEmpty()){
  KURL descriptionUrlTmp;
  descriptionUrlTmp.setPath(QDir::currentDirPath());
  QString name = parameters->getProgramName();
  name.append(".xml");
  descriptionUrlTmp.setFileName(name);
  descriptionUrl = KFileDialog::getSaveURL(descriptionUrlTmp.path(),i18n("*.xml|Xml Files"), this, i18n("Save as..."));
 }
 else{
  descriptionUrl = KFileDialog::getSaveURL(descriptionUrl.path(),i18n("*.xml|Xml Files"), this, i18n("Save as..."));
 }
 //a location has been chosen
 if(!descriptionUrl.isEmpty()){
  //Get the information
  ProgramInformation programInformation;
  programInformation.setProgramName(parameters->getProgramName());
  programInformation.setHelp(help->text());
  QMap<int, QValueList<QString> > parameterInformation = parameters->getParameterInformation();
  programInformation.setParameterInformation(parameterInformation);
  //Create the writer and save the data to disk
  DescriptionWriter descriptionWriter;
  descriptionWriter.setProgramInformation(programInformation);
  bool status = descriptionWriter.writeTofile(descriptionUrl);
  if(!status){
   QString message = QString("The file %1 could not be saved possibly because of insufficient file access permissions.").arg(descriptionUrl.fileName());
   KMessageBox::error (this,i18n(message), i18n("IO Error!"));
  }
  descriptionNotSaved = false;
 }
}


bool ProgramPage::areParametersModified()const{
 return parameters->isValueModified();
}

bool ProgramPage::isDescriptionModified()const{
 return (parameters->isDescriptionModified() || helpIsModified);
}

bool ProgramPage::isDescriptionModifiedAndNotSaved()const{
 return (descriptionNotSaved && (parameters->isDescriptionModified() || helpIsModified));
}

void ProgramPage::nameChanged(const QString& name){
 QString message = "";
 QString title = "";

 //If a call to saveProgram has been made, the check on the script has already be done.
 if(programName != parameters->getProgramName()){
   //Warn the programPage that the name has indeed been modified.
  parameters->programNameChanged();
  programName = parameters->getProgramName();

  //in expert mode, update the script
  if(expertMode){
   //find the file corresponding to the program name
   QString path = KStandardDirs::findExe(name,getenv("PATH"),true);

   if(!path.isNull()){
    QFileInfo fileInfo(path);
    QFile file(path);
    if(!file.open(IO_ReadOnly)){
     message = QString("The file %1 is not readable.").arg(name);
     title = "IO Error!";
     scriptDoc->closeURL();
     scriptView->getDoc()->setText("");
    }
    else{
     QTextStream stream(&file);
     QString firstLine = stream.readLine();
     int i = firstLine.find(QRegExp("^#!"));
     if(i != -1){
      scriptDoc->openURL(path);
      file.close();
      //Setting the content of the KTextEdit (named script) will trigger a scriptModified and therefore set sciptIsModified to true. The initial load of the script
      //should no be considered as a modification.
      sciptIsModified = false;
     }
     else{
      message =  QString("The file %1 does not appear to be a script file (a script file should begin with #!).").arg(name);
      title = "IO Error!";
      scriptDoc->closeURL();
      scriptView->getDoc()->setText("");
     }
    }
   }
   else{
    message =  QString("The file %1 could not be found in your PATH.").arg(name);
    title = "IO Error!";
    scriptDoc->closeURL();
    scriptView->getDoc()->setText("");
    //Setting the content of the KTextEdit (named script) will trigger a scriptModified and therefore set sciptIsModified to true. The initial load of the script
    //should no be considered as a modification.
    sciptIsModified = false;
   }
  }
 }

 //will update the left iconList
 emit programNameChanged(this,name,message,title);
}

void ProgramPage::tabChange(QWidget * page){
 if(page == scriptView){
  emit scriptShown(scriptView);
 }
 else emit scriptHidden();
}


#include "programpage.moc"
