/***************************************************************************
 *   Copyright (C) 2006 by Michael Zugaro                                  *
 *   michael.zugaro@college-de-france.fr                                   *
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
//include files for the application
#include "unitlistpage.h"


// include files for QT
#include <QList>
#include <qstringlist.h>

#include <QEvent>
#include <QVector>
#include <QHeaderView>

UnitListPage::UnitListPage(QWidget* parent) :
    UnitListLayout(parent),isIncorrect(false),incorrectRow(0),modified(false)
{
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    unitTable->horizontalHeader()->setResizeMode( QHeaderView::ResizeToContents );
#else
    unitTable->horizontalHeader()->setSectionResizeMode( QHeaderView::ResizeToContents );
#endif
    //install a filter on the unitTable in order to validate the entries
    unitTable->installEventFilter(this);
    unitTable->setSortingEnabled(true);

    connect(unitTable, SIGNAL(currentCellChanged(int,int,int,int)),this, SLOT(currentChanged()));
    connect(unitTable, SIGNAL(cellPressed(int,int)),this, SLOT(currentChanged()));
    connect(unitTable, SIGNAL(cellClicked(int,int)),this,SLOT(currentChanged()));
    connect(unitTable, SIGNAL(cellDoubleClicked(int,int)),this,SLOT(currentChanged()));
    connect(unitTable, SIGNAL(cellChanged(int,int)),this, SLOT(unitChanged(int,int)));

    connect(addUnitButton,SIGNAL(clicked()),this,SLOT(addUnit()));
    connect(removeUnitButton,SIGNAL(clicked()),this,SLOT(removeUnit()));
}

UnitListPage::~UnitListPage(){}


bool UnitListPage::eventFilter(QObject* object,QEvent* event)
{
    QString name = object->objectName();

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
            UnitTableItem* item = new UnitTableItem(info.at(i));
            unitTable->setItem(iterator.key(),i,item);
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
    if (isIncorrect)
        return;
    modified = true;
    const QList<QTableWidgetSelectionRange> range = unitTable->selectedRanges();
    if(!range.isEmpty()) {
        QList<int> lst;
        Q_FOREACH(const QTableWidgetSelectionRange&r, range) {
            const int nbRows = r.bottomRow() - r.topRow()+1;
            for(int i = 0; i < nbRows;++i){
                const int val = (r.topRow() + i);
                if(!lst.contains(val)) {
                    lst<< val;
                }
            }
        }
        qSort(lst);
        for(int i = lst.count()-1; i>=0; --i) {
            unitTable->removeRow(lst.at(i));
        }
    }
}

void UnitListPage::setNbUnits(int nbUnits)
{
    for(int i =0; i<unitTable->rowCount();++i)
        unitTable->removeRow(i);
    unitTable->setRowCount(nbUnits);
}

void UnitListPage::addUnit()
{
    if(isIncorrect)
        return;
    modified = true;
    unitTable->insertRow(unitTable->rowCount());

    for(int i=0;i<unitTable->columnCount();++i) {
        UnitTableItem* item = new UnitTableItem("");
        unitTable->setItem(unitTable->rowCount() - 1,i,item);
    }
}

void UnitListPage::currentChanged()
{
    if(isIncorrect) {
        unitTable->selectRow(incorrectRow);
        unitTable->setCurrentCell(incorrectRow,incorrectColumn);
    }
}

void UnitListPage::unitChanged(int row,int column)
{
    const QString unit = unitTable->item(row,column)->text();
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
        //unitTable->adjustRow(row);
    }
}


