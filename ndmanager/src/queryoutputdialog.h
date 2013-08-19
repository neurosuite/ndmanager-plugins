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
#ifndef QUERYOUTPUTDIALOG_H
#define QUERYOUTPUTDIALOG_H

#include <qpagedialog.h>
#include <QWebView>


/**
Dialog used to display result from query.

    @author Michaël Zugaro <michael.zugaro@college-de-france.fr>
*/
class QueryOutputDialog : public QPageDialog
{
    Q_OBJECT
public:
    explicit QueryOutputDialog(const QString& htmlText,const QString& queryResult,QWidget *parent = 0,const QString& caption = tr("Query"),const QString& urltext = QString());
    ~QueryOutputDialog();
public slots:
    void slotUser1();
    void slotUser2();

private:
    QWebView        *html;
    QString		htmlText;
    QString		queryResult;
};

#endif
