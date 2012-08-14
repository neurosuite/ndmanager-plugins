/***************************************************************************
                          editormanager.cpp  -  description
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

#include "editormanager.h"
#include <ktexteditor/editorchooser.h>

EditorManager::EditorManager(QWidget* pParent, const char* szName) :
	KParts::PartManager(pParent, szName)
{
}

EditorManager::~EditorManager()
{
}

KTextEditor::Document* EditorManager::add()
{
	KTextEditor::Document* pDoc;
	
	// Create the document
	pDoc = KTextEditor::EditorChooser::createDocument(this);
	addPart(pDoc);

	return pDoc;
}

void EditorManager::remove(KTextEditor::Document* pDoc)
{
	removePart(pDoc);
	delete pDoc;
}
