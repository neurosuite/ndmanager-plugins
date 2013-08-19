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
#ifndef MANAGERVIEW_H
#define MANAGERVIEW_H

// include files for Qt
#include <qsplitter.h>
#include <qstring.h>
#include <qcombobox.h>
#include <qspinbox.h> 


#include <QFrame>
#include <QList>

//Include for the application
#include "launcherpage.h"


#include <QToolBar>
#include <QDockWidget>

class QVBoxLayout;
/**
@author Lynn Hazan
*/
class ManagerView : public QFrame
{
    Q_OBJECT
public:
    explicit ManagerView(QWidget *parent = 0);
    ~ManagerView();
    
    /** Updates the dropdown list containing the spike groups used to launch Klusters.
  * @param nbGroups the new number of spike groups.
  */
    //void updateSpikeGroupList(int nbGroups);

    /**Triggers the update of the dropdown list containing the file extensions.
  * @param extensions list containing the extensions of all the specific files.
  */
    //void updateFileList(const QStringList &extensions);

#if 0
    /**Triggers the update of the dropdown list containing the script names.
  * @param scriptNames list containing the script names.
  */
    void updateScriptList(const QStringList& scriptNames);
#endif
    /** Updates the parameter file url.
  * @param url url of the currently open parameter file.
  */
    void updateDocUrl(const QString& url);

    /** Updates the parameter file information.
  * @param url url of the currently open parameter file.
  * @param isUptoDate true if the parameter file is up to date, false if there are unsaved changes.
  */
    void updateDocumentInformation(const QString &url, bool isUptoDate);

signals:
    void beingDestroyed();
    void checkBeforeLaunchingPrograms();
    void checkBeforeLaunchingScripts();

public slots:  
    void konsoleBeingDestroyed(){emit beingDestroyed();}
    void neuroscopeFileChange(int index);

private slots: 
    /**Launches NeuroScope using, as a parameter, the file selected in the dropdown list containing the file extensions
 */
    //void launchNeuroscope();

    /**Launches Klusters using, as a parameter, the electrode group selected in the dropdown list containing the electrode ids
 */
    //void launchKlusters();

    /**Launches the script selected in the dropdown list containing the existing scripts.
 */
    //void launchScript();

    /**Stops the currently running script.
 */
    void stopScript();

private:
    QVBoxLayout* frameLayout;
    //QComboBox* neuroscopeComboBox;
    //QComboBox* klustersComboBox;
    //QComboBox* scriptsComboBox;
    QString parameterUrl;
    bool isUptoDate;
};

#endif
