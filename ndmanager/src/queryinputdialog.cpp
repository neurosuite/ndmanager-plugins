/***************************************************************************
 *   Copyright (C) 2006 by Michael Zugaro                                  *
 *   michael.zugaro@college-de-france.fr                                   *
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
// application specific includes
#include "queryinputdialog.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QFileDialog>
#include <QLineEdit>


QueryInputPathWidget::QueryInputPathWidget(QWidget *parent)
    : QWidget(parent)
{
    QHBoxLayout *lay = new QHBoxLayout;
    mLineEdit = new QLineEdit;
    mPushButton = new QPushButton;
    lay->addWidget(mLineEdit);
    connect(mLineEdit, SIGNAL(textChanged(QString)), this, SIGNAL(textChanged(QString)));
    mPushButton->setIcon(QPixmap(":/shared-icons/document-open"));
    lay->addWidget(mPushButton);
    connect(mPushButton, SIGNAL(clicked()), SLOT(slotSelectPath()));
    setLayout(lay);
}

QueryInputPathWidget::~QueryInputPathWidget()
{

}

void QueryInputPathWidget::slotSelectPath()
{
    const QString path = QFileDialog::getExistingDirectory(this, tr("Select path"), mLineEdit->text());
    if (!path.isEmpty()) {
        mLineEdit->setText(path);
    }
}

QString QueryInputPathWidget::path() const
{
    return mLineEdit->text();
}


void QueryInputPathWidget::setPath(const QString &path)
{
    mLineEdit->setText(path);
}



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

    path = new QueryInputPathWidget(page);
    path->setMinimumWidth(fontMetrics().maxWidth()*20);
    path->setPath(urltext);
    layout->addWidget(path);

    layout->addStretch(10);

    //connections
    connect(path, SIGNAL(textChanged(QString)), this, SLOT(pathChanged(QString)));
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

QString QueryInputDialog::getQuery() const
{
    return query->text();
}

QString QueryInputDialog::getPath() const
{
    return path->path();
}


