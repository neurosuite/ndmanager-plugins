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
#include <QVBoxLayout>
#include <QLabel>
#include <QDialogButtonBox>
#include <QPushButton>






QueryInputDialog::QueryInputDialog(QWidget *parent,const QString& caption,const QString& urltext) :
    QDialog(parent)
{
    setWindowTitle(caption);
    setModal(true);

    QHBoxLayout *lay = new QHBoxLayout;
    setLayout(lay);
    page = new QWidget(this);
    lay->addWidget(page);
    QVBoxLayout *layout = new QVBoxLayout(page);
    layout->setMargin(0);

    QLabel *label1 = new QLabel(tr("Query"),page);
    label1->setObjectName("query_label");
    layout->addWidget(label1);

    query = new QLineEdit(page);
    query->setObjectName("query");
    query->setMinimumWidth(fontMetrics().maxWidth()*20);
    query->setFocus();
    layout->addWidget(query);

    QLabel *label2 = new QLabel(tr("Path"),page);
    label2->setObjectName("path_label");
    layout->addWidget(label2);

    path = new QLineEdit(page);
    path->setObjectName("path");
    path->setMinimumWidth(fontMetrics().maxWidth()*20);
    path->setText(urltext);
    layout->addWidget(path);

    layout->addStretch(10);

    //connections
    connect(path,SIGNAL(textChanged(QString)),this,SLOT(pathChanged(QString)));
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

    buttonBox->button(QDialogButtonBox::Ok)->setEnabled(!newPath.isEmpty());
}

#include "queryinputdialog.moc"
