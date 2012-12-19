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

#include <QGridLayout>
#include <QLabel>
class QSpacerItem;
class QPushButton;
class UnitTable;
class QLabel;

class UnitListLayout : public QWidget
{
    Q_OBJECT

public:
    explicit UnitListLayout( QWidget* parent = 0, const char* name = 0 );
    ~UnitListLayout();

    QPushButton* addUnitButton;
    QPushButton* removeUnitButton;
    UnitTable* unitTable;
    QLabel* textLabel4_2_2_4_2_2_2;

protected:
    QGridLayout* UnitListLayoutLayout;
    QGridLayout* layout21;
    QSpacerItem* spacer10_2_3_3;
    QGridLayout* layout20;
    QSpacerItem* spacer10_2_3_2;
    QSpacerItem* spacer96;
    QGridLayout* layout19;
    QSpacerItem* spacer10_2_2;
    QSpacerItem* spacer10_2_2_2;
    QSpacerItem* spacer10_2_3;

protected slots:
    virtual void languageChange();

};

#endif // UNITLISTLAYOUT_H
