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
#ifndef PROGRAMPAGE_H
#define PROGRAMPAGE_H

//include files for the application
#include "parameterpage.h"
#include "editormanager.h"

// include files for QT
#include <qwidget.h>
#include <q3frame.h>

//include files for KDE
#include <kurl.h> 
#include <kate/view.h>

//forward declaration
class QTabWidget;
class QPushButton;
class Q3TextEdit;

//General C++ include files
#include <iostream>
using namespace std;

/**
@author Lynn Hazan
*/
class ProgramPage : public Q3Frame
{
Q_OBJECT
public:
/**Constructor.
 * @param expertMode true if the file is opened in expert mode, false otherwise.  
 * @param parent the parent QWidget. 
 * @param name name of the widget (can be used for introspection).   
*/
 ProgramPage(bool expertMode,QWidget* parent = 0, const char* name = 0);
 ~ProgramPage();
 
 /**Returns the parameterPage contained in the page.*/
 inline ParameterPage* getParameterPage(){return parameters;};
 
 /**Returns the script document (Kate::Document, kpart) contained in the page.*/
 inline KTextEditor::Document* getScript(){return scriptDoc;}; 
 
 /**Returns the script view(Kate::View, kpart) contained in the page.*/
 inline Kate::View* getScriptView(){return scriptView;}; 
 
 /**Returns the content of the help.*/
 QString getHelp();
 
 /**Sets the content of the help.*/
 void setHelp(QString helpContent);
 
 /**True if the ParameterPage has been modified, false otherwise.*/
 bool areParametersModified()const;

 /**True if the program description has been modified, false otherwise.
 * The program description includes the program name, the number of parameters, their name and status, 
 * and the help.
 */
 bool isDescriptionModified()const;
 
 /**True if the program description has been modified and not saved, false otherwise.
 * The program description includes the program name, the number of parameters, their name and status, 
 * and the help.
 */
 bool isDescriptionModifiedAndNotSaved()const;
 
 /**True if the script has been modified, false otherwise.*/
 inline bool isScriptModified()const{return sciptIsModified;};
 
 /**Indicates that the initialisation is finished.*/
 inline void initialisationOver(){isInit = false;}
 
signals:
 void programToRemove(ProgramPage* programPage);
 void programNameChanged(ProgramPage* programPage,const QString& name,QString message,QString title);
 void scriptShown(Kate::View* scriptView);
 void scriptHidden();
 
 
public slots:
 inline void removeProgram(){
  emit programToRemove(this);
 };
 /**Saves the program parameters.*/
 void saveProgramParameters();
 
 /**Saves the currently loaded script.
 * @return true if the script has been correctly save false otherwise.
 */
 bool saveProgramScript();
  
 /**Takes the change if the program name into account.
 * @param name name of the program.
 */
 void nameChanged(const QString& name);
    
 /** Will be called when the script is modified.*/
 inline void scriptModified(){if(!isInit) sciptIsModified = true;};
 
 /** Will be called when the help is modified.*/
 inline void helpModified(){if(!isInit) helpIsModified = true;};
 
 /**Resets the internal modification status to false.*/
 inline void resetModificationStatus(){
  sciptIsModified = false;
  helpIsModified = false;
  parameters->resetModificationStatus();
 };
 
private slots:
  
 /**Updates the menus and actions when the script becomes the active tab.*/
 void tabChange(QWidget* widget);

 
private: 
 QTabWidget* tabWidget;
 QPushButton* removeButton;
 QPushButton* saveScriptButton; 
 QPushButton* saveParametersButton;      
 ParameterPage* parameters;
 Q3TextEdit* script; 
 bool sciptIsModified;
 bool isInit;
 QString programName;
 KURL descriptionUrl;
 Q3TextEdit* help;
 bool helpIsModified;
 EditorManager* editorMgr;
 KTextEditor::Document* scriptDoc;
 Kate::View* scriptView; 
 bool descriptionNotSaved;
 
 /**True if the file is opened in expert mode, false otherwise*/
 bool expertMode;  
};

#endif
