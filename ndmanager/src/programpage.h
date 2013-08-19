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
#ifndef PROGRAMPAGE_H
#define PROGRAMPAGE_H

//include files for the application
#include "parameterpage.h"

// include files for QT
#include <qwidget.h>
#include <QFrame>

//forward declaration
class QTabWidget;
class QPushButton;
class QTextEdit;

/**
@author Lynn Hazan
*/
class ProgramPage : public QFrame
{
    Q_OBJECT
public:
    /**Constructor.
 * @param expertMode true if the file is opened in expert mode, false otherwise.
 * @param parent the parent QWidget.
 * @param name name of the widget (can be used for introspection).
*/
    ProgramPage(bool expertMode, QWidget* parent = 0, const QString &name = QString());
    ~ProgramPage();

    /**Returns the parameterPage contained in the page.*/
    ParameterPage* getParameterPage() const {return parameters;}


    /**Returns the script view contained in the page.*/
    QTextEdit* getScriptView(){return scriptView;}

    /**Returns the content of the help.*/
    QString getHelp() const;

    /**Sets the content of the help.*/
    void setHelp(const QString& helpContent);

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
    bool isScriptModified()const{return scriptIsModified;}

    /**Indicates that the initialisation is finished.*/
    void initialisationOver(){isInit = false;}

signals:
    void programToRemove(ProgramPage* programPage);
    void programNameChanged(ProgramPage* programPage,const QString& name,const QString& message,const QString& title);
    void scriptHidden();
    void scriptShown(QTextEdit* scriptView);

public slots:
    void removeProgram();

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
    void scriptModified(){if(!isInit) scriptIsModified = true;}

    /** Will be called when the help is modified.*/
    void helpModified(){if(!isInit) helpIsModified = true;}

    /**Resets the internal modification status to false.*/
    void resetModificationStatus(){
        scriptIsModified = false;
        helpIsModified = false;
        parameters->resetModificationStatus();
    }

private slots:

    /**Updates the menus and actions when the script becomes the active tab.*/
    void tabChange(int index);


private: 
    QTabWidget* tabWidget;
    QPushButton* removeButton;
    QPushButton* saveScriptButton;
    QPushButton* saveParametersButton;
    ParameterPage* parameters;
    QTextEdit* script;
    bool scriptIsModified;
    bool isInit;
    QString programName;
    QString descriptionUrl;
    QTextEdit* help;
    bool helpIsModified;
    QTextEdit* scriptView;
    bool descriptionNotSaved;

    /**True if the file is opened in expert mode, false otherwise*/
    bool expertMode;
};

#endif
