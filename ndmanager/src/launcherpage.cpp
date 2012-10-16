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
#include "launcherpage.h"

// include files for Qt
#include <qdir.h> 
#include <qcombobox.h>
#include <qstringlist.h> 



#include <QDebug>



LauncherPage::LauncherPage(const QString& url,QWidget* parent)
    : LauncherLayout(parent){
    QDir dir(url);
    qDebug()<<"currentPath "<<url;
    QStringList neuroscopeFiles = dir.entryList(QLatin1String("*.dat;*.eeg;*.fil"));
    qDebug()<<"neuroscopeFiles.size() "<<neuroscopeFiles.size();
    neuroscopeFiles.append("");
    neuroscopeComboBox->insertStringList(neuroscopeFiles);
    QStringList klustersFiles = dir.entryList(QLatin1String("*.fet;*.spk;*.clu"));
    klustersFiles.append("");
    klustersComboBox->insertStringList(klustersFiles);

}


LauncherPage::~LauncherPage()
{
}


#include "launcherpage.moc"
