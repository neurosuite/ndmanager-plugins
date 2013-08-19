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
#ifndef FILEPAGE_H
#define FILEPAGE_H

//include files for the application
#include <filelayout.h>
#include <helper.h>

// include files for QT
#include <qwidget.h>
#include <qlineedit.h>
#include <qregexp.h>

#include <QEvent>
#include <QList>


/**
* Class used to represent a new file created out of the raw data contained in the .dat file.
* @author Lynn Hazan
*/
class FilePage : public FileLayout
{
    Q_OBJECT
public:
    /**Constructor.*/
    explicit FilePage(QWidget *parent = 0);

    /**Destructor.*/
    ~FilePage();

    /**Sets the sampling rate.*/
    void setSamplingRate(double rate){
        samplingRateLineEdit->setText(Helper::doubleToString(rate));}

    /**Sets the file extension.*/
    void setExtension(const QString& extension){
        this->extension = extension;
        extensionLineEdit->setText(extension);
    }

    /** Initializes the channel mapping table.
* @param channels map containing the list of original channels for each new channel.
*/
    void setChannelMapping(const QMap<int, QList<int> >& channels);

    /**Returns the channel mapping.
* @return map containing the list of original channels for each new channel.
*/ 
    QMap<int, QList<int> > getChannelMapping()const;

    /**Returns the sampling rate.*/
    inline double getSamplingRate() const{return samplingRateLineEdit->text().toDouble();}

    /**Returns the sampling rate.*/
    inline QString getExtension() const{return extensionLineEdit->text();}

    /**True if at least one property has been modified, false otherwise.*/
    inline bool isModified()const{return modified;}

signals:
    /** Siganls a change in a file extension.
 * @param extension the new extension for the current file.
 * @param filePage a pointer on the FilePage which has its extension changed.
 */
    void extensionChanged(const QString& extension,FilePage* filePage);

protected:
    /** Event filter to validate the entries in the mapping table.
 * @param object target object for the event.
 * @param event event sent.
 */
    bool eventFilter(QObject* object,QEvent* event);

public slots:

    /**Notifies that the file extension has changed.*/
    void changeCaption();

    /**Adds a new line to the mapping table.*/
    void addChannel();

    /**Removes the selected lines from the mapping table.*/
    void removeChannel();

    /**Validates the current entry in the mapping table.*/
    void slotValidate();

    /**Validates the current entry in the mapping table.*/
    void mappingChanged(int row,int column);

    /** Will be called when any properties is modified.*/
    void propertyModified(){if(!isInit) modified = true;}

    /**Indicates that the initialisation is finished.*/
    void initialisationOver(){isInit = false;}

    /**Resets the internal modification status to false.*/
    void resetModificationStatus(){modified = false;}

private:
    bool isIncorrectRow;
    int incorrectRow;
    bool modified;
    bool isInit;
    QString extension;
};

#endif
