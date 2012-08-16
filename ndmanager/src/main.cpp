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
#include <kapplication.h>
#include <dcopclient.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>

// include files for QT
#include <qdir.h>
#include <qstring.h>

//Application specific include files
#include "ndmanager.h"
#include "kmessagebox.h"

static const char description[] =
    I18N_NOOP("Neurophysiological Data Processing Manager");

static KCmdLineOptions options[] =
{
    { "+[URL]", I18N_NOOP( "Document to open." ), 0 },
    KCmdLineLastOption
};

QString version;

int main(int argc, char **argv)
{
    KAboutData about("ndmanager", I18N_NOOP("NDManager"), VERSION, description,
                     KAboutData::License_GPL, "(C) 2004-2007 Lynn Hazan", 0, 0, "lynn.hazan@myrealbox.com");
    about.addAuthor( "Lynn Hazan", 0, "lynn.hazan@myrealbox.com" );
   about.addAuthor("Michael Zugaro",I18N_NOOP("Co-designed the interface."), "michael.zugaro@college-de-france.fr");
    KCmdLineArgs::init(argc, argv, &about);
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
        KURL url = KURL();
        url.setPath((QDir::currentDirPath()).append("/"));
        url.setFileName(file);
        manager->openDocumentFile(url);
       }
       else  manager->openDocumentFile(file);
     }
     args->clear();
     manager->show();
    }


    return app.exec();
}
