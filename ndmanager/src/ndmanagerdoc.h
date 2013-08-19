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
#ifndef NDMANAGERDOC_H
#define NDMANAGERDOC_H

// include files for QT
#include <qstring.h>


#include <QWidget>

//include files for the application

/**
@author Lynn Hazan
*/
class ndManagerDoc{

private:
    /**The url of the document .*/
    QString docUrl;
    
public:
    /**Constructor.
 * @param parent the parent QWidget.
 */
    explicit ndManagerDoc(QWidget* parent);
    ~ndManagerDoc();

    /**Information retun after a call to openFile/saveDocument/createFeatureFile*/
    enum OpenSaveCreateReturnMessage {OK=0,OPEN_ERROR=1,DOWNLOAD_ERROR=3,INCORRECT_FILE=4,SAVE_ERROR=5,
                                      UPLOAD_ERROR=6,INCORRECT_CONTENT=7,CREATION_ERROR=8,PARSE_ERROR=9,MISSING_FILE=10,ALREADY_OPENED=11};

    /**closes the actual document.*/
    void closeDocument();

    /** opens the document by filename.
  * @param url url of the file to open (dat file or eeg file).
  * @return an opensavecreatereturnmessage enum giving the open status.
  */
    OpenSaveCreateReturnMessage openDocument(const QString& url);

    /**Returns the QString of the document. */
    inline const QString& url() const{return docUrl;}

    /**Loads information in order to create a new the document .
  * @return an opensavecreatereturnmessage enum giving the initialization status.
  */
    OpenSaveCreateReturnMessage newDocument();

    /**Saves the current parameter file.
 @return an OpenSaveCreateReturnMessage enum giving the saving status.
 */
    inline OpenSaveCreateReturnMessage save(){return save(docUrl);}

    /**Saves the current parameter file on the disk at the location defined by url.
 * @param url url of the file where to save the parameter information.
 * @return an OpenSaveCreateReturnMessage enum giving the saving status.
 */
    OpenSaveCreateReturnMessage save(const QString &url);

    /**Saves the parameter file.
 * @param newUrl new url where to write the parameter fil to.
 * @return an OpenSaveCreateReturnMessage enum giving the saving status.
 */
    inline OpenSaveCreateReturnMessage saveAs(const QString& newUrl){
        docUrl = newUrl;
        return save(docUrl);
    }

    /**Save the current parameter file as the default one in the user .kde directory.*/
    OpenSaveCreateReturnMessage saveDefault();

    /**Saves the script @p scriptName.
 * @param scriptName name of the script to save.
 * @return an OpenSaveCreateReturnMessage enum giving the saving status.
 */
    OpenSaveCreateReturnMessage saveScript(const QString& scriptName);

    /**Renames the parameter file.
 * @param newUrl new url where to write the parameter fil to.
 */
    void rename(const QString& newUrl){
        docUrl = newUrl;
    }

private:

    /**Pointer on the parent widget (main window).*/
    QWidget* parent;

};

#endif
