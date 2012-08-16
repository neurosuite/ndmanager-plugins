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
#include "ndkonsole.h"
 
//Include files for KDE
#include <kde_terminal_interface.h> // Should be available in KDE > 3.2, but works only with 3.4
#include <klibloader.h> 


// include files for Qt
#include <qdir.h> 
#include <qlayout.h>
//Added by qt3to4:
#include <Q3VBoxLayout>
#include <QShowEvent>
#include <Q3CString>

//General C++ include files
#include <iostream>
using namespace std; 
 
NdKonsole::NdKonsole(QWidget *parent, const char *name)
 : QWidget(parent, name),part(0L),isPart(false),directory(""){
 layout = new Q3VBoxLayout(this);
 
}


NdKonsole::~NdKonsole(){
}

 int NdKonsole::loadConsoleIfNeeded(const QString url){
  if(isPart)  return OK;
  
  directory = url.directory();
  
  // fetch the Library.
  KLibFactory* factory = 0;
  factory = KLibLoader::self()->factory( "libkonsolepart" );
  if(factory == 0L){
    // inform the user that he should install konsole.
    return NO_KPART;
  }     
  
  // fetch the part.
//    part = dynamic_cast<KParts::ReadOnlyPart *>(
//        factory->create(this, "libkonsolepart", "KParts::ReadOnlyPart" ) );

part = static_cast<KParts::ReadOnlyPart*>( factory->create( this, "libkonsolepart", "TerminalEmulator") );


  if(!part) return PART_LOADING_ERROR;  
  isPart = true; 
      
  layout->addWidget(part->widget());
  connect (part,SIGNAL(destroyed(QObject*)),this, SLOT(slotDestroyed(QObject*)));
   
  //if(!part->inherits("TerminalInterface")){
//      cout<<"!terminal "<<endl;
  
     QString directoryToOpen = QString(directory);
     
//      cout<<"directory "<<directory<<" directoryToOpen "<<directoryToOpen.path()<<endl;
     
    // This probably happens because the konsole that is installed
    // comes from before KDE 3.2 , and the TerminalInterface is not
    // available.
    part->openURL(directoryToOpen);


// cout<<"result "<<result<<endl;

    return OK;
 // }
//Code which can be used with kde 3.4
// cast the part to the TerminalInterface..
//   else{
     //TerminalInterface* terminal = static_cast<TerminalInterface*>( part->qt_cast("TerminalInterface"));
     //terminal->showShellInDir(directory); 
     //return OK;
//   }

}

 void NdKonsole::slotDestroyed(QObject* part){    
  isPart = false; 
  part = 0L;
  
  emit beingDestroyed();  
}  
 
void NdKonsole::showEvent(QShowEvent *){
  if(!part) loadConsoleIfNeeded(QString(directory));
} 

bool NdKonsole::runCommand(QString command) const{
	return part->writeStream(Q3CString(command));
}


#include "ndkonsole.moc"
