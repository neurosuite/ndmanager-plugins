/***************************************************************************
 *   Copyright (C) 2006 by Michael Zugaro                                  *
 *   michael.zugaro@college-de-france.fr                                   *
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
//include files for the application
#include "unitlistpage.h"


// include files for QT
#include <QList>
#include <qstringlist.h>
//Added by qt3to4:
#include <QEvent>
#include <Q3MemArray>

UnitListPage::UnitListPage(QWidget* parent, const char *name) :
UnitListLayout(parent,name),nbUnits(0),isIncorrect(false),incorrectRow(0),modified(false)
{
	for(int i = 0;i<unitTable->numCols();++i) unitTable->setColumnStretchable(i,true);
	
	//install a filter on the unitTable in order to validate the entries
	unitTable->installEventFilter(this);

	//Set that the click on the header of a column sorts that column
	unitTable->setSorting(true);
	
	connect(addUnitButton,SIGNAL(clicked()),this,SLOT(addUnit()));
	connect(removeUnitButton,SIGNAL(clicked()),this,SLOT(removeUnit()));
	connect(unitTable, SIGNAL(currentChanged(int,int)),this, SLOT(currentChanged()));
	connect(unitTable, SIGNAL(valueChanged(int,int)),this, SLOT(unitChanged(int,int)));
	connect(unitTable, SIGNAL(pressed(int,int,int,const QPoint&)),this, SLOT(currentChanged()));
	connect(unitTable, SIGNAL(clicked(int,int,int,const QPoint&)),this,SLOT(currentChanged()));
	connect(unitTable, SIGNAL(doubleClicked(int,int,int,const QPoint&)),this,SLOT(currentChanged()));
}

UnitListPage::~UnitListPage(){}


bool UnitListPage::eventFilter(QObject* object,QEvent* event)
{
	QString name = object->name();

	if (name.find("unitTable") != -1 && isIncorrect)
	{
		unitTable->selectRow(incorrectRow);
		unitTable->setCurrentCell(incorrectRow,incorrectColumn);
		return true;
	}
	else if (name.find("unitTable") != -1 && event->type() == QEvent::Leave)
	{		
		if(unitTable->currentRow() != -1)
		{
			int row = unitTable->currentRow();
			int column = unitTable->currentColumn();
			QWidget* widget = unitTable->cellWidget(row,column);
			if(widget != 0 && widget->isA("QLineEdit"))
			{				
				Q3TableItem* item = unitTable->item(row,column);				
				if(!static_cast<QLineEdit*>(widget)->text().isEmpty())
				item->setContentFromEditor(widget);
				return true;
			}
			else return QWidget::eventFilter(object,event); 
		} 
		else return QWidget::eventFilter(object,event); 
	}
	else return QWidget::eventFilter(object,event); 
}
	
void UnitListPage::setUnits(const QMap<int, QList<QString> >& units)
{
	for (int i =0; i<unitTable->numRows();++i) unitTable->removeRow(i);
	unitTable->setNumRows(units.count());
	
	QMap<int,QList<QString> >::const_iterator iterator;
	//The iterator gives the keys sorted.
	for (iterator = units.begin(); iterator != units.end(); ++iterator)
	{
		QList<QString> info = iterator.data();
		for (uint i=0;i<info.count();++i)
		{ 
			UnitTableItem* item = new UnitTableItem(unitTable,Q3TableItem::OnTyping,info[i]);
			item->setWordWrap(true);
			unitTable->setItem(iterator.key(),i,item); 
		}
	}//end of units loop
}

void UnitListPage::getUnits(QMap<int, QList<QString> >& units)const
{ 
	int unitId = 1;
	for(int i =0; i<unitTable->numRows();++i)
	{
		QList<QString> info;
		for(int j = 0;j < unitTable->numCols(); ++j)
		{
			QString text = unitTable->text(i,j);
			info.append(text.simplified());
		}
		units.insert(unitId,info);
		unitId++;
	}
}

void UnitListPage::removeUnit()
{
	if (isIncorrect) return;
	modified = true;
	int nbSelections = unitTable->numSelections();
		
	if (nbSelections > 0)
	{
		QList< Q3MemArray<int> > rowsToRemove;
		//Look up the rows to be removed
		for (int j = 0; j < nbSelections;++j)
		{
			Q3TableSelection selection = unitTable->selection(j);
			bool active = selection.isActive();
			if (active)
			{
				int nbRows = selection.bottomRow() - selection.topRow() + 1;
				Q3MemArray<int> rows(nbRows);
				for(int i = 0; i < nbRows;++i) rows[i] = selection.topRow() + i;
				rowsToRemove.append(rows);
			}
		}
		//Actually remove the rows
		QList< Q3MemArray<int> >::iterator iterator;
		for (iterator = rowsToRemove.begin(); iterator != rowsToRemove.end(); ++iterator) unitTable->removeRows(*iterator);
	}  
}


#include "unitlistpage.moc"
