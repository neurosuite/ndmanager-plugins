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
#ifndef UNITLISTPAGE_H
#define UNITLISTPAGE_H

//include files for the application
#include <unitlistlayout.h>
#include "unittable.h"
#include "unittableitem.h"

// include files for QT
#include <qwidget.h>
#include <qtable.h>
#include <qpushbutton.h>
#include <qmap.h>
#include <qregexp.h>
#include <qheader.h> 
#include <qmemarray.h> 
#include <qlineedit.h>

// include files for KDE


//General C++ include files
#include <iostream>
using namespace std;


/**
* Class used to represent the information linked to the list of units.
* @author Michael Zugaro
*/
class UnitListPage : public UnitListLayout
{
	Q_OBJECT

	public:
		/**Constructor.*/
		UnitListPage(QWidget* parent = 0, const char *name = 0);
		/**Destructor.*/
		~UnitListPage();
		/** Initializes the unit table.
		* @param units map containing the info for each unit.
		*/
		void setUnits(const QMap<int, QValueList<QString> >& units);
		/**Returns the composition of the unit table.
		* @param units map containing the info for each unit.
		*/ 
		void getUnits(QMap<int, QValueList<QString> >& units)const;
		/**Initializes the number of units classified.
		* @param nbUnits number of units.
		*/
		inline void setNbUnits(int nbUnits)
		{
			this->nbUnits = nbUnits;
			for(int i =0; i<unitTable->numRows();++i) unitTable->removeRow(i);
			unitTable->setNumRows(nbUnits);
		}; 
		/**True if at least one property has been modified, false otherwise.*/
		inline bool isModified()const{return modified;};
	
	protected:
		/** Event filter to validate the entries in the unit table.
		* @param object target object for the event.
		* @param event event sent.
		*/
		bool eventFilter(QObject* object,QEvent* event);
	
	public slots:
		/**Adds a new line to the unit table.*/
		inline void addUnit()
		{  
			if(isIncorrect) return;
			modified = true;
			unitTable->insertRows(unitTable->numRows());
			
			//Use of the the 3 parameter constructor to be qt 3.1 compatible
			for(int i=0;i<unitTable->numCols();++i)
			{
				UnitTableItem* item = new UnitTableItem(unitTable,QTableItem::WhenCurrent,"");
				item->setWordWrap(true);
				unitTable->setItem(unitTable->numRows() - 1,i,item);
			}
		};
		/**Removes the selected lines from the unit table.*/
		void removeUnit();
		/**selection moved to a different entry in the unit table.*/
		inline void currentChanged()
		{						
			if(isIncorrect)
			{ 
				unitTable->selectRow(incorrectRow);
				unitTable->setCurrentCell(incorrectRow,incorrectColumn);
			}
		};
		/**Validates an entry in the unit table.*/
		inline void unitChanged(int row,int column)
		{
			QString unit = unitTable->text(row,column); 
			//the group and cluster entries should only contain digits
			//the I.D. entry should only contain digits and '.'
			if((column==0||column==1)&&(unit.contains(QRegExp("[^\\d]"))!=0)||(column==4&&(unit.contains(QRegExp("[^\\d.]"))!=0)))
			{
				isIncorrect = true;
				incorrectRow = row;
				incorrectColumn = column;
/*				unitTable->selectRow(row);
				unitTable->setCurrentCell(row,column);*/
			}
			else
			{ 
				isIncorrect = false;
				unitTable->adjustRow(row);
			}
		};
		/** Will be called when any properties is modified.*/
		inline void propertyModified(){modified = true;};
		/**Resets the internal modification status to false.*/
		inline void resetModificationStatus(){modified = false;};
		
	private:

		int	nbUnits; 
		bool	isIncorrect;
		int	incorrectRow,incorrectColumn;
		bool	modified;
};



#endif
