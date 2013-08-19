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
#ifndef GENERALINFOPAGE_H
#define GENERALINFOPAGE_H

//include files for the application
#include <generalinfolayout.h>

// include files for QT
#include <qwidget.h>
#include <qdatetime.h>
#include <qlineedit.h>
#include <QCalendarWidget>


/**
@author Lynn Hazan
*/
class GeneralInfoPage : public GeneralInfoLayout
{
    Q_OBJECT
public:
    explicit GeneralInfoPage(QWidget *parent = 0);
    ~GeneralInfoPage();
    
    /**Sets the date.*/
    void setDate(const QDate& date){
        kDatePicker->setSelectedDate(date);
    }
    
    /**Sets the experimenters.*/
    void setExperimenters(const QString& experimenters){
        experimentersLineEdit->setText(experimenters);
    }
    
    /**SSets the description.*/
    void setDescription(const QString& description){
        descriptionTextEdit->setText(description);
    }
    
    /**Sets the notes.*/
    void setNotes(const QString& notes){
        notesTextEdit->setText(notes);
    }
    
    
    /**Gets the date.*/
    inline QDate getDate() const {
        return kDatePicker->selectedDate();
    }
    
    /**Gets the experimenters.*/
    inline QString getExperimenters() const {
        return experimentersLineEdit->text();
    }
    
    /**Gets the description.*/
    inline QString getDescription() const {
        return descriptionTextEdit->toPlainText();
    }
    
    /**Gets the notes.*/
    inline QString getNotes() const {
        return notesTextEdit->toPlainText();
    }
    
    /**True if at least one property has been modified, false otherwise.*/
    inline bool isModified()const{return modified;}
    
    /**Indicates that the initialisation is finished.*/
    void initialisationOver(){isInit = false;}

public slots:

    /** Will be called when any properties is modified.*/
    void propertyModified(){if(!isInit) modified = true;}

    /**Resets the internal modification status to false.*/
    void resetModificationStatus(){modified = false;}

private:
    bool modified;
    bool isInit;
};

#endif
