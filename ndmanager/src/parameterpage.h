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
#ifndef PARAMETERPAGE_H
#define PARAMETERPAGE_H

//include files for the application
#include <parameterlayout.h>

// include files for QT
#include <qstringlist.h> 
#include <qlineedit.h> 

#include <QEvent>
#include <QList>
class QComboBox;

/**
@author Lynn Hazan
*/
class ParameterPage : public ParameterLayout
{
    Q_OBJECT
public:
    /**Constructor.
 * @param expertMode true if the file is opened in expert mode, false otherwise.
 * @param parent the parent QWidget.
 * @param name name of the widget (can be used for introspection).
*/
    ParameterPage(bool expertMode, QWidget *parent = 0);
    ~ParameterPage();

    /**Sets the name of the program.
  * @param name name of the program.
  */
    void setProgramName(const QString& name){nameLineEdit->setText(name);}

    /**Gets the name of the program.*/
    QString getProgramName() const{return nameLineEdit->text();}

    /**Updates an internal status to reflect the change in program name.
  */
    void programNameChanged(){descriptionModified = true;}


    /** Initializes the parameter table.
 * @param parameters map containing the list of parameters.
 Each entry contains the name of the parameter, its value and its status (optional, mandatory or dynamic).
 */
    void setParameterInformation(const QMap<int, QStringList >& parameters);

    /**Returns the information for each parameter.
 * @return map containing the list of parameters. Each entry contains the name of the parameter, its value and its status (optional, mandatory or dynamic).
 */
    QMap<int, QStringList > getParameterInformation();

    /**True if at least one of the description property has been modified, false otherwise.*/
    bool isDescriptionModified()const{return descriptionModified;}

    /**True if the value of at least one of parameters has been modified, false otherwise.*/
    bool isValueModified()const{return valueModified;}

signals:
    void nameChanged(const QString& extension);

public slots:
    /**Handles the change of the program's name.*/
    void changeCaption();
    /**Adds a new line in the parameter table.*/
    void addParameter();
    /**Removes the selected lines from the parameter table.*/
    void removeParameter();

    /** Will be called when any properties is modified.*/
    void propertyModified(int row, int column);

    /**Resets the internal modification status to false.*/
    void resetModificationStatus(){
        valueModified = false;
        descriptionModified = false;
    }

    void slotValueModified() {
        valueModified = true;
    }

protected:
    /** Event filter to validate the entries in the group table.
 * @param object target object for the event.
 * @param event event sent.
 */
    bool eventFilter(QObject* object,QEvent* event);


private:
    QComboBox *createCombobox();
    QStringList status;
    /**Stores the list of column containing drop down list.*/
    QList<int> ddList;
    bool valueModified;
    bool descriptionModified;
    bool mExpertMode;
};

#endif
