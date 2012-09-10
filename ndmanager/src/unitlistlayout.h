/****************************************************************************
** Form interface generated from reading ui file '/home/lynn/CMBN/NDManager/ndmanager/src/unitlistlayout.ui'
**
** Created: lun aoû 20 15:36:44 2007
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.3.7   edited Aug 31 2005 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef UNITLISTLAYOUT_H
#define UNITLISTLAYOUT_H

#include <qvariant.h>
#include <qwidget.h>
//Added by qt3to4:
#include <Q3GridLayout>
#include <QLabel>
class Q3GridLayout;
class QSpacerItem;
class QPushButton;
class UnitTable;
class QLabel;

class UnitListLayout : public QWidget
{
    Q_OBJECT

public:
    UnitListLayout( QWidget* parent = 0, const char* name = 0, Qt::WFlags fl = 0 );
    ~UnitListLayout();

    QPushButton* addUnitButton;
    QPushButton* removeUnitButton;
    UnitTable* unitTable;
    QLabel* textLabel4_2_2_4_2_2_2;

protected:
    Q3GridLayout* UnitListLayoutLayout;
    Q3GridLayout* layout21;
    QSpacerItem* spacer10_2_3_3;
    Q3GridLayout* layout20;
    QSpacerItem* spacer10_2_3_2;
    QSpacerItem* spacer96;
    Q3GridLayout* layout19;
    QSpacerItem* spacer10_2_2;
    QSpacerItem* spacer10_2_2_2;
    QSpacerItem* spacer10_2_3;

protected slots:
    virtual void languageChange();

};

#endif // UNITLISTLAYOUT_H
