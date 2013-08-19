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
#ifndef FILESPAGE_H
#define FILESPAGE_H

//include files for the application
#include "filepage.h"

// include files for QT
#include <qwidget.h>
#include <QFrame>

#include <QList>



//forward declaration
class QTabWidget;
class QPushButton;

/**
* Container used to group the new files created out of the raw data contained in the .dat file.
* @author Lynn Hazan
*/
class FilesPage : public QFrame
{
    Q_OBJECT
public:
    explicit FilesPage(QWidget* parent = 0);
    ~FilesPage();

    /**Adds a tab containg a new file.
 * @param title caption for the new tab, it corresponds to the extension.
 * @return a pointer on the newly created page.
 */
    FilePage* addFile(const QString& title);

    /**Fills the list @p fileList with pointers to the FilePage.
 * @param fileList list to be filled with the current FilePage.
 */
    void getFilePages(QList<FilePage*>& fileList);

    /**Returns a list containing the extensions of the all specific files.
 * @return extension list.
 */
    QStringList getFileExtensions() const;

    /**True if at least one the FilePage has been modified, false otherwise.*/
    bool isModified()const;

public slots:
    /**Adds a new tab for a new file.*/
    void addNewFile();

    /**Removes the current tab.*/
    void removeFile();

    /**Changes the caption of the tab corresponding to the FilePage which has its extension changed.
 * @param caption the new caption.
 * @param filePage a pointer on the FilePage which has its extension changed.
 */
    void changeCaption(const QString& caption,FilePage* filePage);

    /**Resets the internal modification status of the the FilePages to false.*/
    void resetModificationStatus();

signals:
    /** This signal is used to update the dropdown list containing the file extensions in the managerView.
 * @param extensions list containing the extensions of all the specific files.
 */
    void fileModification(const QStringList& extensions);

private:
    QTabWidget* tabWidget;
    QPushButton* addButton;
    QPushButton* removeButton;
};

#endif
