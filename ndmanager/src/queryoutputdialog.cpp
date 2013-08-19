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
#include "queryoutputdialog.h"

#include <QTextStream>
#include <QMessageBox>
#include <QFileDialog>
#include <QHBoxLayout>

#include <QWebSettings>

QueryOutputDialog::QueryOutputDialog(const QString& htmlText,const QString& queryResult,QWidget *parent,const QString& caption,const QString& urltext) :
    QPageDialog(parent),
    htmlText(htmlText),
    queryResult(queryResult)
{
    setButtons(Ok|User1|User2);
    setDefaultButton(Ok);
    setFaceType(Plain);
    setWindowTitle(caption);

    setButtonText( User1, tr("Save As Text") );
    setButtonText( User2, tr("Save As HTML") );
    QWidget * w = new QWidget(this);
    QHBoxLayout *lay = new QHBoxLayout;
    html = new QWebView(w);
    lay->addWidget(html);
    w->setLayout(lay);

    addPage(w,QString());

    html->setHtml(htmlText);
    html->reload();
    html->settings()->setAttribute(QWebSettings::JavascriptEnabled, false);
    html->settings()->setAttribute(QWebSettings::JavaEnabled,false);
    html->settings()->setAttribute(QWebSettings::PluginsEnabled,false);
    html->settings()->setAttribute(QWebSettings::LocalContentCanAccessFileUrls, true);
    html->settings()->setAttribute(QWebSettings::LocalContentCanAccessRemoteUrls,false);
    resize(800,600);
    connect(this, SIGNAL(user1Clicked()), SLOT(slotUser1()));
    connect(this, SIGNAL(user2Clicked()), SLOT(slotUser2()));
}

QueryOutputDialog::~QueryOutputDialog()
{
}

void QueryOutputDialog::slotUser1()
{
    const QString filename = QFileDialog::getSaveFileName(this,tr("Save query"),QString(),"*");
    if(filename.isEmpty())
      return;

    if(QFile::exists(filename) && QMessageBox::question(this,QString(),tr("File already exists. Overwrite?"),QMessageBox::Yes|QMessageBox::No) == QMessageBox::No)
        return;
    QFile textFile(filename);
    if(textFile.open(QIODevice::WriteOnly))
    {
        QTextStream stream(&textFile);
        stream << queryResult;
        textFile.close();
        if(textFile.error() == QFile::WriteError )
            QMessageBox::critical(this,QString(),tr("Could not save the report. This may be due to incorrect write permissions."));
    }
    else
        QMessageBox::critical(this,QString(),tr("Could not save the report. This may be due to incorrect write permissions."));
}

void QueryOutputDialog::slotUser2()
{
    const QString filename = QFileDialog::getSaveFileName(this,tr("Save query as HTML"),QString(),"*.html");
    if(filename.isEmpty())
      return;
    if(QFile::exists(filename) &&
            QMessageBox::question(this,QString(),tr("File already exists. Overwrite?"),QMessageBox::Yes|QMessageBox::No) == QMessageBox::No)
        return;

    QFile htmlFile(filename);
    if(htmlFile.open(QIODevice::WriteOnly)) {
        QTextStream stream(&htmlFile);
        stream << htmlText;
        htmlFile.close();
        if(htmlFile.error() == QFile::WriteError ) {
            QMessageBox::critical(this,QString(),tr("Could not save the report. This may be due to incorrect write permissions."));
        }
    } else {
        QMessageBox::critical(this,QString(),tr("Could not save the report. This may be due to incorrect write permissions."));
    }
}

