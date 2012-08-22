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
#include "queryoutputdialog.h"
//Added by qt3to4:
#include <Q3TextStream>
#include <QMessageBox>

#include <QWebSettings>
QueryOutputDialog::QueryOutputDialog(QString htmlText,QString queryResult,QWidget *parent,const QString& caption,const QString& urltext) :
KDialogBase(parent,"Query Results",true,caption,Ok|User1|User2,Ok,true,KGuiItem(tr("Save As Text")),KGuiItem(tr("Save As HTML"))),
htmlText(htmlText),
queryResult(queryResult)
{
	vbox = new Q3VBox(this);
	setMainWidget(vbox);
	html = new QWebView(vbox);
	html->setHtml(htmlText);
	html->reload();
	html->settings()->setAttribute(QWebSettings::JavascriptEnabled, false);
        html->settings()->setAttribute(QWebSettings::JavaEnabled,false);
	html->settings()->setAttribute(QWebSettings::PluginsEnabled,false);
	html->settings()->setAttribute(QWebSettings::LocalContentCanAccessFileUrls, true);
	html->settings()->setAttribute(QWebSettings::LocalContentCanAccessRemoteUrls,false);
	resize(800,600);
	
}

QueryOutputDialog::~QueryOutputDialog()
{
}

void QueryOutputDialog::slotUser1()
{
    QString filename = QFileDialog::getSaveFileName(this,"saveQuery",QString(),"*");
    if(QFile::exists(filename) && KMessageBox::warningYesNo(this,tr("File already exists. Overwrite?")) == KMessageBox::No) return;
	QFile textFile(filename);
	if(textFile.open(QIODevice::WriteOnly))
	{
		Q3TextStream stream(&textFile);
		stream << queryResult;
		textFile.close();
        if(stream.device()->status() == IO_WriteError ) QMessageBox::critical(this,tr("Could not save the report. This may be due to incorrect write permissions."));
	}
    else QMessageBox::critical(this,tr("Could not save the report. This may be due to incorrect write permissions."));
}

void QueryOutputDialog::slotUser2()
{
    QString filename = QFileDialog::getSaveFileName(this,"saveQuery",QString(),"*.html");
    if(QFile::exists(filename) && KMessageBox::warningYesNo(this,tr("File already exists. Overwrite?")) == KMessageBox::No) return;
	QFile htmlFile(filename);
	if(htmlFile.open(QIODevice::WriteOnly))
	{
		Q3TextStream stream(&htmlFile);
		stream << htmlText;
		htmlFile.close();
        if(stream.device()->status() == IO_WriteError ) QMessageBox::critical(this,tr("Could not save the report. This may be due to incorrect write permissions."));
	}
    else QMessageBox::critical(this,tr("Could not save the report. This may be due to incorrect write permissions."));
}

#include "queryoutputdialog.moc"
