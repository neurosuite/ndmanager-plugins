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

    QStringList args;
    for (int i = 1; i < argc; ++i) {
        args.push_back(QString::fromLocal8Bit(argv[i]));
    }
    QApplication app(argc, argv);
    ndManager* manager = new ndManager();
    manager->show();
    if(args.count()){
        QString file = args.at(0);
        if(file.left(1) != "/"){
            QString url;
            url = QDir::currentPath()+ QDir::separator() + file;
            manager->openDocumentFile(url);
        } else {
            manager->openDocumentFile(file);
        }
    }

    return app.exec();
}
