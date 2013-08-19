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
//include files for the application
#include "parameterpage.h"

// include files for QT
#include <qwidget.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <QComboBox>

#include <QEvent>
#include <QVector>
#include <QList>
#include <QDebug>
#include <QStyledItemDelegate>

class ColumnDelegate : public QStyledItemDelegate
{
public:
    ColumnDelegate(QObject *parent)
        : QStyledItemDelegate(parent)
    {

    }
    QWidget *createEditor ( QWidget * parent, const QStyleOptionViewItem & option, const QModelIndex & index ) const
    {
        return 0;
    }

};

ParameterPage::ParameterPage(bool expertMode,QWidget *parent)
    : ParameterLayout(parent),
      valueModified(false),
      descriptionModified(false),
      mExpertMode(expertMode)
{
    status<<tr("Mandatory")<<tr("Optional")<<tr("Dynamic");
    ddList.append(2);
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    parameterTable->horizontalHeader()->setResizeMode( QHeaderView::ResizeToContents );
#else
    parameterTable->horizontalHeader()->setSectionResizeMode( QHeaderView::ResizeToContents );
#endif
    //If the export mode is not set, only the value column is editable
    if (!expertMode) {
        parameterTable->setItemDelegateForColumn(0,new ColumnDelegate(this));
        parameterTable->setItemDelegateForColumn(2,new ColumnDelegate(this));
        parameterTable->setItemDelegateForColumn(1,new ColumnDelegate(this));

        addButton->setEnabled(false);
        removeButton->setEnabled(false);
        nameLineEdit->setReadOnly(true);
    } else {
        connect(addButton,SIGNAL(clicked()),this,SLOT(addParameter()));
        connect(removeButton,SIGNAL(clicked()),this,SLOT(removeParameter()));
        connect(nameLineEdit,SIGNAL(returnPressed()),this,SLOT(changeCaption()));
        connect(nameLineEdit,SIGNAL(editingFinished()),this,SLOT(changeCaption()));
    }

    //install a filter on the groupTable in order to validate the entries
    parameterTable->installEventFilter(this);

    connect(parameterTable, SIGNAL(currentCellChanged(int,int,int,int)),this, SLOT(propertyModified(int,int)));//does not seem to work (see the hack with the eventfiler)
}


ParameterPage::~ParameterPage(){}

bool ParameterPage::eventFilter(QObject* object,QEvent* event){
    QString name = object->objectName();

    //hack, if the event is KeyRelease this means that there was a modification
    if(name.indexOf("parameterTable") != -1 && event->type() == QEvent::KeyRelease){
        if(parameterTable->currentColumn() == 1)
            valueModified = true;
        else
            descriptionModified = true;

        return true;
    }
    else return QWidget::eventFilter(object,event);
}

void ParameterPage::propertyModified(int, int column){
    if(column == 1)
        valueModified = true;
    else
        descriptionModified = true;
}

QMap<int, QStringList > ParameterPage::getParameterInformation(){
    QMap<int, QStringList > parameterInformation;
    int paramNb = 1;
    for(int i =0; i<parameterTable->rowCount();++i){
        QStringList information;
        QString item = parameterTable->item(i,0)->text();
        QString name = item.simplified();
        if(name == " ")
            continue;
        information.append(name);
        for(int j = 1;j < parameterTable->columnCount(); ++j){
            QString text;
            if(ddList.contains(j)) {
                text = (static_cast<QComboBox*>(parameterTable->cellWidget(i,j)))->currentText();
            }
            else
                text = parameterTable->item(i,j)->text();
            information.append(text.simplified());
        }

        parameterInformation.insert(paramNb,information);
        paramNb++;
    }
    return parameterInformation;
}

void ParameterPage::setParameterInformation(const QMap<int, QStringList >& parameters){
    //Clean the parameterTable, just in case, before creating empty rows.
    parameterTable->clearContents();
    parameterTable->setRowCount(parameters.count());

    QMap<int,QStringList >::ConstIterator iterator;
    //The iterator gives the keys sorted.
    for(iterator = parameters.constBegin(); iterator != parameters.constEnd(); ++iterator){
        const QStringList parameterInfo = iterator.value();

        for(uint i=0;i<parameterInfo.count();++i){
            if(ddList.contains(i)){
                QComboBox *combo = createCombobox();
                parameterTable->setCellWidget(iterator.key(),i,combo);
                connect(combo, SIGNAL(activated(int)), SLOT(slotValueModified()));
            } else {
                parameterTable->setItem(iterator.key(),i,new QTableWidgetItem(parameterInfo[i]));
            }

            //parameterTable->adjustColumn(i);
        }
    }//end of parameters loop
}


void ParameterPage::addParameter(){
    descriptionModified = true;
    parameterTable->insertRow(parameterTable->rowCount());

    parameterTable->setItem(parameterTable->rowCount() - 1,0,new QTableWidgetItem());

    parameterTable->setItem(parameterTable->rowCount() - 1,1,new QTableWidgetItem());

    //Add the comboxItem in the status column
    parameterTable->setCellWidget(parameterTable->rowCount() - 1,2,createCombobox());
}

QComboBox *ParameterPage::createCombobox()
{
    QComboBox *combo = new QComboBox;
    combo->addItems(status);
    combo->setEnabled(mExpertMode);
    return combo;
}

void ParameterPage::removeParameter(){
    descriptionModified = true;
    const QList<QTableWidgetSelectionRange> range = parameterTable->selectedRanges();
    if(!range.isEmpty()) {
        QList<int> lst;
        Q_FOREACH(const QTableWidgetSelectionRange&r, range) {
            const int nbRows = r.bottomRow() - r.topRow() + 1;
            for(int i = 0; i < nbRows;++i){
                int val = (r.topRow() + i);
                if(!lst.contains(val)) {
                    lst<< val;
                }
            }
        }
        qSort(lst);
        for(int i = lst.count()-1; i>=0; --i) {
            parameterTable->removeRow(lst.at(i));
        }
    }
}

void ParameterPage::changeCaption()
{
    const QString name = nameLineEdit->text();
    if(name.isEmpty() && !name.contains("New Script-"))
        emit nameChanged(tr("Unknown"));
    else
        emit nameChanged(name);
}



