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
#ifndef CLUSTERSPAGE_H
#define CLUSTERSPAGE_H

//include files for the application
#include <clusterslayout.h>

// include files for QT
#include <qwidget.h>
#include <qlineedit.h>
#include <qvalidator.h>



/**
@author Lynn Hazan
*/
class ClustersPage : public ClustersLayout
{
    Q_OBJECT
public:
    explicit ClustersPage(QWidget* parent = 0);
    ~ClustersPage();

    /**Sets the number of samples per spike waveform.*/
    void setNbSamples(int nb) {nbSamplesLineEdit->setText(QString::number(nb));}

    /**Sets the index of the peak sample in the spike waveform.*/
    void setPeakIndex(int index) {peakIndexLineEdit->setText(QString::number(index));}

    /**Returns the number of samples per spike waveform.*/
    inline int getNbSamples() const {return nbSamplesLineEdit->text().toInt();}

    /**Returns the index of the peak sample in the spike waveform.*/
    inline int getPeakIndex() const {return peakIndexLineEdit->text().toInt();}

    /**True if at least one property has been modified, false otherwise.*/
    inline bool isModified() const {return modified;}

    /**Indicates that the initialisation is finished.*/
    void initialisationOver(){isInit = false;}

public slots:

    /** Will be called when any properties is modified.*/
    void propertyModified(){ if(!isInit) modified = true;}

    /**Resets the internal modification status to false.*/
    void resetModificationStatus(){modified = false;}

private:
    QIntValidator intValidator;
    bool modified;
    bool isInit;
};

#endif
