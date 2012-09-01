/***************************************************************************
 *   Copyright (C) 2006 by Michael Zugaro                                  *
 *   michael.zugaro@college-de-france.fr                                   *
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
// application specific includes
#include "queryinputdialog.h"
//Added by qt3to4:
#include <Q3VBoxLayout>
#include <QLabel>
#include <QDialogButtonBox>
#include <QPushButton>


//General C++ include files



QueryInputDialog::QueryInputDialog(QWidget *parent,const QString& caption,const QString& urltext) :
    QDialog(parent)
{
    setWindowTitle(caption);
    setModal(true);

    QHBoxLayout *lay = new QHBoxLayout;
    setLayout(lay);
    page = new QWidget(this);
    lay->addWidget(page);
    layout = new Q3VBoxLayout(page,0);

    QLabel *label1 = new QLabel(tr("Query"),page,"query_label");
    layout->addWidget(label1);

    query = new QLineEdit(page,"query");
    query->setMinimumWidth(fontMetrics().maxWidth()*20);
    query->setFocus();
    layout->addWidget(query);

    QLabel *label2 = new QLabel(tr("Path"),page,"path_label");
    layout->addWidget(label2);

    path = new QLineEdit(page,"path");
    path->setMinimumWidth(fontMetrics().maxWidth()*20);
    path->setText(urltext);
    layout->addWidget(path);

    layout->addStretch(10);

    //connections
    connect(path,SIGNAL(textChanged (const QString &)),this,SLOT(pathChanged(const QString &)));
    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                     | QDialogButtonBox::Cancel);
    layout->addWidget(buttonBox);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
}

QueryInputDialog::~QueryInputDialog()
{
}

void QueryInputDialog::pathChanged(const QString & newPath){

    if(newPath != QString("")){
        //Enable the OK button
        buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
    }
    else if(newPath == QString("")){
        //Disable the OK button
        buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
    }
}

#include "queryinputdialog.moc"
