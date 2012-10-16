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
#include <QVector>

UnitListPage::UnitListPage(QWidget* parent) :
    UnitListLayout(parent),nbUnits(0),isIncorrect(false),incorrectRow(0),modified(false)
{
#ifdef KDAB_PENDING
    for(int i = 0;i<unitTable->numCols();++i)
        unitTable->setColumnStretchable(i,true);


    //Set that the click on the header of a column sorts that column
    unitTable->setSorting(true);

    connect(unitTable, SIGNAL(currentChanged(int,int)),this, SLOT(currentChanged()));
    connect(unitTable, SIGNAL(valueChanged(int,int)),this, SLOT(unitChanged(int,int)));
    connect(unitTable, SIGNAL(pressed(int,int,int,QPoint)),this, SLOT(currentChanged()));
    connect(unitTable, SIGNAL(clicked(int,int,int,QPoint)),this,SLOT(currentChanged()));
    connect(unitTable, SIGNAL(doubleClicked(int,int,int,QPoint)),this,SLOT(currentChanged()));
#endif
    //install a filter on the unitTable in order to validate the entries
    unitTable->installEventFilter(this);

    connect(addUnitButton,SIGNAL(clicked()),this,SLOT(addUnit()));
    connect(removeUnitButton,SIGNAL(clicked()),this,SLOT(removeUnit()));
}

UnitListPage::~UnitListPage(){}


bool UnitListPage::eventFilter(QObject* object,QEvent* event)
{
    QString name = object->name();

    if (name.indexOf("unitTable") != -1 && isIncorrect)
    {
        unitTable->selectRow(incorrectRow);
        unitTable->setCurrentCell(incorrectRow,incorrectColumn);
        return true;
    }
    else if (name.indexOf("unitTable") != -1 && event->type() == QEvent::Leave)
    {
        if(unitTable->currentRow() != -1)
        {
            int row = unitTable->currentRow();
            int column = unitTable->currentColumn();
            QWidget* widget = unitTable->cellWidget(row,column);
            if(widget != 0 && widget->metaObject()->className() == ("QLineEdit"))
            {
                if(!static_cast<QLineEdit*>(widget)->text().isEmpty())
                    unitTable->setCellWidget(row,column,widget);
                return true;
            }
            else return QWidget::eventFilter(object,event);
        }
        else return QWidget::eventFilter(object,event);
    }
    else return QWidget::eventFilter(object,event);
}

void UnitListPage::setUnits(const QMap<int, QStringList >& units)
{
    unitTable->clearContents();
    unitTable->setRowCount(units.count());

    QMap<int,QStringList >::const_iterator iterator;
    //The iterator gives the keys sorted.
    for (iterator = units.constBegin(); iterator != units.constEnd(); ++iterator)
    {
        const QStringList info = iterator.value();
        for (uint i=0;i<info.count();++i)
        {
            //UnitTableItem* item = new UnitTableItem(unitTable,Q3TableItem::OnTyping,info[i]);
            unitTable->setItem(iterator.key(),i,new QTableWidgetItem(info.at(i)));
        }
    }//end of units loop
}

void UnitListPage::getUnits(QMap<int, QStringList >& units)const
{ 
    int unitId = 1;
    for(int i =0; i<unitTable->rowCount();++i)
    {
        QStringList info;
        for(int j = 0;j < unitTable->columnCount(); ++j)
        {
            QString text = unitTable->item(i,j)->text();
            info.append(text.simplified());
        }
        units.insert(unitId,info);
        unitId++;
    }
}

void UnitListPage::removeUnit()
{
    #ifdef KDAB_PENDING
    if (isIncorrect) return;
    modified = true;
    int nbSelections = unitTable->numSelections();

    if (nbSelections > 0)
    {
        QList< QVector<int> > rowsToRemove;
        //Look up the rows to be removed
        for (int j = 0; j < nbSelections;++j)
        {
            Q3TableSelection selection = unitTable->selection(j);
            bool active = selection.isActive();
            if (active)
            {
                int nbRows = selection.bottomRow() - selection.topRow() + 1;
                QVector<int> rows(nbRows);
                for(int i = 0; i < nbRows;++i) rows[i] = selection.topRow() + i;
                rowsToRemove.append(rows);
            }
        }
        //Actually remove the rows
        QList< QVector<int> >::iterator iterator;
        for (iterator = rowsToRemove.begin(); iterator != rowsToRemove.end(); ++iterator) unitTable->removeRows(*iterator);
    }
    #endif
}

void UnitListPage::setNbUnits(int nbUnits)
{
    #ifdef KDAB_PENDING
    this->nbUnits = nbUnits;
    for(int i =0; i<unitTable->numRows();++i)
        unitTable->removeRow(i);
    unitTable->setNumRows(nbUnits);
#endif
}

void UnitListPage::addUnit()
{
    if(isIncorrect)
        return;
    modified = true;
    #ifdef KDAB_PENDING
    unitTable->insertRows(unitTable->numRows());

    //Use of the the 3 parameter constructor to be qt 3.1 compatible
    for(int i=0;i<unitTable->numCols();++i)
    {
        UnitTableItem* item = new UnitTableItem(unitTable,Q3TableItem::WhenCurrent,"");
        item->setWordWrap(true);
        unitTable->setItem(unitTable->numRows() - 1,i,item);
    }
#endif
}

void UnitListPage::currentChanged()
{
    if(isIncorrect)
    {
        unitTable->selectRow(incorrectRow);
        unitTable->setCurrentCell(incorrectRow,incorrectColumn);
    }
}

void UnitListPage::unitChanged(int row,int column)
{
    #ifdef KDAB_PENDING
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
#endif
}


#include "unitlistpage.moc"
