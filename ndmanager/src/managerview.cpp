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
#include "managerview.h"
// include files for Qt
#include <qdir.h> 
#include <qstringlist.h> 
#include <qlayout.h>
#include <QVBoxLayout>
#include <QList>
#include <QFrame>
#include <QProcess>
#include <QDebug>
#include <QAction>
#include <QMessageBox>



ManagerView::ManagerView(QWidget *parent)
    : QFrame(parent),isUptoDate(true)
{
    frameLayout = new QVBoxLayout(this);
    frameLayout->setSpacing(0);
    frameLayout->setMargin(0);
}


ManagerView::~ManagerView(){
}

void ManagerView::neuroscopeFileChange(int){

}
#if 0
void ManagerView::updateSpikeGroupList(int nbGroups){
    klustersComboBox->clear();
    QStringList klustersFiles;
    for (int i = 0; i < nbGroups;++i){
        klustersFiles<<QString::number(i+1);
    }

    klustersComboBox->addItems(klustersFiles);
}
#endif
#if 0
void ManagerView::updateFileList(const QStringList& extensions){
    neuroscopeComboBox->clear();
    QStringList neuroscopeFiles;
    neuroscopeFiles<<".dat"<<".eeg";
    neuroscopeFiles<<extensions;
    neuroscopeComboBox->addItems(neuroscopeFiles);
}
#endif

#if 0
void ManagerView::updateScriptList(const QStringList& scriptNames){
    scriptsComboBox->clear();
    scriptsComboBox->addItems(scriptNames);
}
#endif
#if 0
void ManagerView::launchNeuroscope(){
    emit checkBeforeLaunchingPrograms();
    QFileInfo parameterUrlFileInfo(parameterUrl);
    //The parameter file is new or has been imported from an existing file.
    if(parameterUrlFileInfo.fileName() == "Untitled"){
        QMessageBox::critical (this,tr("Unsaved file!"),tr("In order to launch NeuroScope, the parameter file has to be saved first.") );
        return;
    }
    else{
        if(!isUptoDate){
            QMessageBox::critical (this, tr("Unsaved file!"),tr("The parameter file contains unsaved data, please save before launching NeuroScope."));
            return;
        }
        //Launch NeuroScope.
        else{
            QString fileName = parameterUrlFileInfo.fileName();
            QString baseName = fileName.left(fileName.length()-4);
            QString fileToUse = baseName.append(neuroscopeComboBox->currentText());
            QProcess::startDetached("neuroscope", QStringList()<<QString(parameterUrlFileInfo.absolutePath()+ QDir::separator() + fileToUse));
        }
    }
}

void ManagerView::launchKlusters(){
    if(klustersComboBox->count() == 0){
        QMessageBox::critical (this, tr("Undefined spike groups"),tr("No spike groups have been defined, please define at least one spike group before launching Klusters."));
        return;
    }

    emit checkBeforeLaunchingPrograms();
    QFileInfo parameterUrlFileInfo(parameterUrl);
    //The parameter file is new or has been imported from an existing file.
    if(parameterUrlFileInfo.fileName() == "Untitled"){
        QMessageBox::critical (this, tr("Unsaved file!"),tr("In order to launch Klusters, the parameter file has to be saved first."));
        return;
    }
    else{
        if(!isUptoDate){
            QMessageBox::critical (this, tr("Unsaved file!"),tr("The parameter file contains unsaved data, please save before launching Klusters."));
            return;
        }
        //Launch Klusters.
        else{
            QString fileName = parameterUrlFileInfo.fileName();
            QString baseName = fileName.left(fileName.length()-4);
            QString fileToUse = baseName.append(".spk.").append(klustersComboBox->currentText());
            QProcess::startDetached("klusters",QStringList()<<QString(parameterUrlFileInfo.absolutePath()+QDir::separator() + fileToUse));
        }
    }
}
#endif
#if 0
void ManagerView::launchScript(){
    emit checkBeforeLaunchingScripts();
    QString script = scriptsComboBox->currentText();
    //The parameter file is new or has been imported from an existing file.
    QFileInfo parameterUrlFileInfo(parameterUrl);
    if(parameterUrlFileInfo.fileName() == "Untitled"){
        const  QString message = tr("In order to launch %1, the parameter file has to be saved first.").arg(script);
        QMessageBox::critical (this, tr("Unsaved file!"),message);
        return;
    }
    else{
        if(!isUptoDate){
            const QString message = tr("The parameter file contains unsaved data or plugin, please save before launching %1.").arg(script);
            QMessageBox::critical (this, tr("Unsaved data!"),message);
            return;
        }
        //Launch the script.
        else{
            QString fileName = parameterUrlFileInfo.fileName();
            QString baseName = fileName.left(fileName.length()-4);

            konsole->runCommand(script+" "+baseName);
        }
    }
}
#endif

void ManagerView::stopScript(){
}


void ManagerView::updateDocUrl(const QString &url){
    parameterUrl = url;
}

void ManagerView::updateDocumentInformation(const QString& url,bool isUptoDate){
    parameterUrl = url;
    this->isUptoDate = isUptoDate;
}

