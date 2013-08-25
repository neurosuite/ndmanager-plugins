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

//include files for the application
#include "programpage.h"
#include "parameterpage.h"
#include "descriptionwriter.h"

// include files for QT
#include <qlayout.h>
#include <qtabwidget.h>
#include <qpushbutton.h>
#include <qfileinfo.h>
#include <QTextStream>
#include <qapplication.h>
#include <qregexp.h>
#include <QDebug>
#include <qtextedit.h>

#include <QGridLayout>
#include <QFrame>
#include <QList>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include "ndmanagerutils.h"


ProgramPage::ProgramPage(bool expertMode,QWidget *parent, const QString& name)
    : QFrame(parent),
      scriptIsModified(false),
      isInit(true),
      programName(name),
      helpIsModified(false),
      descriptionNotSaved(true),
      expertMode(expertMode)
{
    setObjectName(name);
    QVBoxLayout* frameLayout = new QVBoxLayout(this);
    frameLayout->setMargin(0);
    frameLayout->setSpacing(0);
    //Creat the upper part containing a tabWidget with 3 tabs, one with the parameters (ParameterPage), one with the script and one with the help.
    //In expert mode, the script tab does not exist.

    tabWidget = new QTabWidget(this);
    // script = new KTextEdit(tabWidget);
    help = new QTextEdit(tabWidget);
    help->setAcceptRichText(false);
    if(!expertMode)
        help->setReadOnly(true);
    parameters = new ParameterPage(expertMode,tabWidget);
    tabWidget->addTab(parameters,tr("Parameters"));

    tabWidget->addTab(help,tr("Help"));
    connect(tabWidget, SIGNAL(currentChanged(int)), this, SLOT(tabChange(int)));

    if(expertMode){
        scriptView = new QTextEdit(this);
        scriptView->setAcceptRichText(false);
        tabWidget->addTab(scriptView,tr("Plugin"));
    } else {
        scriptView = 0;
    }

    frameLayout->addWidget(tabWidget);

    //Add the buttons
    QWidget* buttons = new QWidget(this);
    QGridLayout* gridLayout = new QGridLayout(buttons);
    frameLayout->addWidget(buttons);

    if(expertMode){
        saveParametersButton = new QPushButton(tr("Save Plugin Description As ..."),buttons);

        QSizePolicy policy(QSizePolicy::Fixed,QSizePolicy::Fixed);
        policy.setHorizontalStretch(0);
        policy.setVerticalStretch(0);
        policy.setHeightForWidth(saveParametersButton->sizePolicy().hasHeightForWidth());
        saveParametersButton->setSizePolicy(policy);

        saveParametersButton->setMinimumSize(QSize(200,0));
        saveParametersButton->setMaximumSize(QSize(300,32767));
        gridLayout->addWidget(saveParametersButton,0,1);

        saveScriptButton = new QPushButton(tr("Save Plugin As ..."),buttons);
        policy = QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
        policy.setHorizontalStretch(0);
        policy.setVerticalStretch(0);
        policy.setHeightForWidth(saveScriptButton->sizePolicy().hasHeightForWidth());

        saveScriptButton->setSizePolicy(policy);
        saveScriptButton->setMinimumSize(QSize(124,0));
        saveScriptButton->setMaximumSize(QSize(130,32767));
        gridLayout->addWidget(saveScriptButton,0,3);

        removeButton = new QPushButton(tr("Remove Plugin"),buttons);
        policy = QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
        policy.setHorizontalStretch(0);
        policy.setVerticalStretch(0);
        policy.setHeightForWidth(removeButton->sizePolicy().hasHeightForWidth());

        removeButton->setSizePolicy(policy);
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
        connect(scriptView,SIGNAL(textChanged()),this,SLOT(scriptModified()));
        connect(help,SIGNAL(textChanged()),this,SLOT(helpModified()));
    } else {
        removeButton = new QPushButton(tr("Remove Plugin"),buttons);
        QSizePolicy policy(QSizePolicy::Fixed,QSizePolicy::Fixed);
        policy.setHorizontalStretch(0);
        policy.setVerticalStretch(0);
        policy.setHeightForWidth(removeButton->sizePolicy().hasHeightForWidth());

        removeButton->setSizePolicy(policy);
        removeButton->setMinimumSize(QSize(124,0));
        removeButton->setMaximumSize(QSize(124,32767));
        gridLayout->addWidget(removeButton,0,1);

        QSpacerItem* space1 = new QSpacerItem(112,16,QSizePolicy::Expanding,QSizePolicy::Minimum);
        gridLayout->addItem(space1,0,0);
        QSpacerItem* space2 = new QSpacerItem(112,16,QSizePolicy::Expanding,QSizePolicy::Minimum);
        gridLayout->addItem(space2,0,2);
    }

    connect(removeButton,SIGNAL(clicked()),this,SLOT(removeProgram()));
    connect(parameters,SIGNAL(nameChanged(QString)),this,SLOT(nameChanged(QString)));
}


ProgramPage::~ProgramPage(){
}

QString ProgramPage::getHelp() const
{
    return help->toPlainText();
}

void ProgramPage::setHelp(const QString& helpContent){
    help->setText(helpContent);
}

bool ProgramPage::saveProgramScript(){
    bool recall = false;
    //find the file corresponding to the program name
    QString name = parameters->getProgramName();
    QString path = NdManagerUtils::findExecutable(name);
    QString message;
    QString title;
    QString scriptUrl;

    if(!path.isNull())
        scriptUrl = QFileDialog::getSaveFileName( this, tr("Save as..."),path,QLatin1String("*.*"));
    else
        scriptUrl = QFileDialog::getSaveFileName(this, tr("Save as..."),QString(),QLatin1String("*.*"));

    if(!scriptUrl.isEmpty()){
        path = scriptUrl;
        QFile file(path);
        if(!file.open(QIODevice::WriteOnly)){
            message = tr("The file %1 could not be saved possibly because of insufficient file access permissions.").arg(name);
            title = tr("IO Error!");
            recall = true;
        }
        else{
            QTextStream stream(&file);
            stream<<scriptView->toPlainText();
            file.close();
        }
    }

    //If the change in the programName lineedit has not yet been validated, a call to nameChanged will be done after this call. Therefore
    //programName has to be updated so no message will be display in that call (otherwise there will be conflict in the events)
    if(programName != parameters->getProgramName()) programName = parameters->getProgramName();
    if(!title.isEmpty())
        QMessageBox::critical (this, title,message);
    if(recall)
        return saveProgramScript();
    else{
        if(!title.isEmpty())
            return false;
        else{
            scriptIsModified = false;
            return true;
        }
    }
}

void ProgramPage::saveProgramParameters(){
    //Always ask the user where to save the information. If as save has already be done, set the save location as the default location
    //for the new save
    if(descriptionUrl.isEmpty()){
        QString descriptionUrlTmp;
        descriptionUrlTmp = QDir::currentPath();
        QString name = parameters->getProgramName();
        name.append(".xml");
        descriptionUrlTmp += QDir::separator() + name;
        descriptionUrl = QFileDialog::getSaveFileName(this, tr("Save as..."),descriptionUrlTmp,tr("Xml Files (*.xml)"));
    }
    else{
        descriptionUrl = QFileDialog::getSaveFileName(this, tr("Save as..."),descriptionUrl,tr("Xml Files (*.xml)"));
    }
    //a location has been chosen
    if(!descriptionUrl.isEmpty()){
        //Get the information
        ProgramInformation programInformation;
        programInformation.setProgramName(parameters->getProgramName());
        programInformation.setHelp(help->toPlainText());
        QMap<int, QStringList > parameterInformation = parameters->getParameterInformation();
        programInformation.setParameterInformation(parameterInformation);
        //Create the writer and save the data to disk
        DescriptionWriter descriptionWriter;
        descriptionWriter.setProgramInformation(programInformation);
        bool status = descriptionWriter.writeTofile(descriptionUrl);
        if(!status){
            QString message = tr("The file %1 could not be saved possibly because of insufficient file access permissions.").arg(descriptionUrl);
            QMessageBox::critical (this, tr("IO Error!"),message);
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
    QString message;
    QString title;

    //If a call to saveProgram has been made, the check on the script has already be done.
    if(programName != parameters->getProgramName()){
        //Warn the programPage that the name has indeed been modified.
        parameters->programNameChanged();
        programName = parameters->getProgramName();

        //in expert mode, update the script
        if(expertMode){
            //find the file corresponding to the program name
            QString path = NdManagerUtils::findExecutable(name);

            if(!path.isNull()){
                QFile file(path);
                if(!file.open(QIODevice::ReadOnly)){
                    message = tr("The file %1 is not readable.").arg(name);
                    title = tr("IO Error!");
                    scriptView->clear();
                } else {
                    QTextStream stream(&file);
                    QString firstLine = stream.readLine();
                    const int i = firstLine.indexOf(QRegExp("^#!"));
                    if(i != -1){
                        scriptView->setPlainText(stream.readAll());
                        file.close();
                        //Setting the content of the KTextEdit (named script) will trigger a scriptModified and therefore set scriptIsModified to true. The initial load of the script
                        //should no be considered as a modification.
                        scriptIsModified = false;
                    } else {
                        message =  tr("The file %1 does not appear to be a plugin file (a script file should begin with #!).").arg(name);
                        title = tr("IO Error!");
                        scriptView->clear();
                    }
                }
            }
            else{
                message =  tr("The file %1 could not be found in your PATH.").arg(name);
                title = tr("IO Error!");
                scriptView->clear();
                //Setting the content of the KTextEdit (named script) will trigger a scriptModified and therefore set scriptIsModified to true. The initial load of the script
                //should no be considered as a modification.
                scriptIsModified = false;
            }
        }
    }

    //will update the left iconList
    emit programNameChanged(this,name,message,title);
}

void ProgramPage::tabChange(int index){
    QWidget *w = tabWidget->widget(index);
    if(w == scriptView){
        emit scriptShown(scriptView);
    } else {
        emit scriptHidden();
    }
}

void ProgramPage::removeProgram(){
    emit programToRemove(this);
}


