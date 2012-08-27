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

// include files for KDE
#include "config-ndmanager.h"
// include files for QT
#include <qdir.h>
#include <qstring.h>
#include <QApplication>

//Application specific include files
#include "ndmanager.h"

QString version;

int main(int argc, char **argv)
{
    QApplication::setOrganizationName("sourceforge");
    QApplication::setOrganizationDomain("sourceforge.net");
    QApplication::setApplicationName("ndmanager");
    QApplication app(argc, argv);
#if KDAB_PENDING
    KCmdLineArgs::addCmdLineOptions(options);
    KApplication app;

    version = VERSION;

    // see if we are starting with session management
    if (app.isRestored()){RESTORE(ndManager);}
    else{
     // no session.. just start up normally
     ndManager* manager = new ndManager();
     KCmdLineArgs* args = KCmdLineArgs::parsedArgs();

     //If there is an argument provided it is the name of the file.
     if(args->count()){
       QString file = args->arg(0);
       if(file.left(1) != "/"){
        QString url = QString();
        url.setPath((QDir::currentPath()).append("/"));
        url.setFileName(file);
        manager->openDocumentFile(url);
       }
       else  manager->openDocumentFile(file);
     }
     args->clear();
     manager->show();
    }

#endif
    ndManager* manager = new ndManager();
    manager->show();
    return app.exec();
}
