/***************************************************************************
                          editormanager.h  -  description
                             -------------------
    begin                : Sun May 18 2003
    copyright            : (C) 2003 by Elad Lahav
    email                : elad@eldarshany.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef EDITORMANAGER_H
#define EDITORMANAGER_H

#include <qwidget.h>
#include <kparts/partmanager.h>
#include <klibloader.h>
#include <ktexteditor/document.h>

/**
 * Creates text editor parts, used to open source files.
 * The EditorManager is responsible for creating parts, and managing their
 * GUI integration.
 * @author Elad Lahav
 */

class EditorManager : public KParts::PartManager
{
   Q_OBJECT

public: 
/**
 * Class constructor.
 * @param	pParent	The parent widget
 * @param	szName	The widget's name
 */
	EditorManager(QWidget* pParent = 0, const char* szName = 0);
/**
 * Class destructor.
 */
	~EditorManager();

/**
 * Creates a new document part.
 * @return	A pointer to the new document
 */
	KTextEditor::Document* add();
        
/**
 * Deletes a document part.
 * @param	pDoc	The document to remove
 */        
	void remove(KTextEditor::Document* pDoc);
};

#endif
