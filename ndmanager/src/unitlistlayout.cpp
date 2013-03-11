/****************************************************************************
** Form implementation generated from reading ui file '/home/lynn/CMBN/NDManager/ndmanager/src/unitlistlayout.ui'
**
** Created: jeu sep 6 14:47:39 2007
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.3.7   edited Aug 31 2005 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "unitlistlayout.h"
#include "unittable.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qtooltip.h>

#include <qimage.h>
#include <qpixmap.h>

#include <QGridLayout>


/*
 *  Constructs a UnitListLayout as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 */
UnitListLayout::UnitListLayout(QWidget* parent, const char* name)
    : QWidget( parent)
{
    if ( !name )
        setObjectName( "UnitListLayout" );
    UnitListLayoutLayout = new QGridLayout( this);

    layout21 = new QGridLayout;

    layout20 = new QGridLayout;
    spacer10_2_3_2 = new QSpacerItem( 354, 16, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout20->addItem( spacer10_2_3_2, 0, 1 );

    layout19 = new QGridLayout;
    spacer10_2_2 = new QSpacerItem( 20, 16, QSizePolicy::Fixed, QSizePolicy::Minimum );
    layout19->addItem( spacer10_2_2, 0, 0 );
    spacer10_2_2_2 = new QSpacerItem( 20, 16, QSizePolicy::Fixed, QSizePolicy::Minimum );
    layout19->addItem( spacer10_2_2_2, 0, 2 );
    spacer10_2_3 = new QSpacerItem( 136, 16, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout19->addItem( spacer10_2_3, 0, 4 );

    addUnitButton = new QPushButton( this);
    QSizePolicy policy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    policy.setHorizontalStretch(0);
    policy.setVerticalStretch(0);
    policy.setHeightForWidth(addUnitButton->sizePolicy().hasHeightForWidth());

    addUnitButton->setSizePolicy( policy );
    addUnitButton->setMinimumSize( QSize( 104, 0 ) );
    addUnitButton->setMaximumSize( QSize( 104, 32767 ) );

    layout19->addWidget( addUnitButton, 0, 1 );

    removeUnitButton = new QPushButton( this );
    policy = QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    policy.setHorizontalStretch(0);
    policy.setVerticalStretch(0);
    policy.setHeightForWidth(removeUnitButton->sizePolicy().hasHeightForWidth());
    removeUnitButton->setSizePolicy(policy);

    removeUnitButton->setMinimumSize( QSize( 104, 0 ) );
    removeUnitButton->setMaximumSize( QSize( 104, 32767 ) );

    layout19->addWidget( removeUnitButton, 0, 3 );

    layout20->addLayout( layout19, 3, 3, 3, 4 );

    unitTable = new UnitTable( this );
    unitTable->setColumnCount( unitTable->columnCount() + 1 );
    unitTable->setHorizontalHeaderItem(unitTable->columnCount() - 1,new QTableWidgetItem( tr("Group" ) ));
    unitTable->setColumnCount( unitTable->columnCount() + 1 );
    unitTable->setHorizontalHeaderItem(unitTable->columnCount() - 1,new QTableWidgetItem( tr("Cluster" ) ));
    unitTable->setColumnCount( unitTable->columnCount() + 1 );
    unitTable->setHorizontalHeaderItem(unitTable->columnCount() - 1,new QTableWidgetItem( tr( "Structure" ) ) );
    unitTable->setColumnCount( unitTable->columnCount() + 1 );
    unitTable->setHorizontalHeaderItem(unitTable->columnCount() - 1,new QTableWidgetItem(tr( "Type" ) ));
    unitTable->setColumnCount( unitTable->columnCount() + 1 );
    unitTable->setHorizontalHeaderItem(unitTable->columnCount() - 1,new QTableWidgetItem(tr( "I.D." ) ));
    unitTable->setColumnCount( unitTable->columnCount() + 1 );
    unitTable->setHorizontalHeaderItem(unitTable->columnCount() - 1,new QTableWidgetItem(tr( "Quality" ) ));
    unitTable->setColumnCount( unitTable->columnCount() + 1 );
    unitTable->setHorizontalHeaderItem(unitTable->columnCount() - 1,new QTableWidgetItem(tr( "Notes" ) ));

    policy = QSizePolicy((QSizePolicy::Policy)7, (QSizePolicy::Policy)7);
    policy.setHorizontalStretch(0);
    policy.setVerticalStretch(0);
    policy.setHeightForWidth(unitTable->sizePolicy().hasHeightForWidth());



    unitTable->setSizePolicy( policy );
    unitTable->setMinimumSize( QSize( 200, 100 ) );
    unitTable->setVerticalScrollBarPolicy( Qt::ScrollBarAsNeeded );
    unitTable->setRowCount( 0 );
    unitTable->setColumnCount( 7 );
    unitTable->setSelectionMode( QAbstractItemView::MultiSelection );
    unitTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    layout20->addWidget( unitTable, 1, 0, 1, 6 );

    textLabel4_2_2_4_2_2_2 = new QLabel( this );

    layout20->addWidget( textLabel4_2_2_4_2_2_2, 0, 0 );
    spacer96 = new QSpacerItem( 16, 19, QSizePolicy::Minimum, QSizePolicy::Fixed );
    layout20->addItem( spacer96, 2, 0 );

    layout21->addLayout( layout20, 0, 1, 2, 2 );
    spacer10_2_3_3 = new QSpacerItem( 30, 16, QSizePolicy::Fixed, QSizePolicy::Minimum );
    layout21->addItem( spacer10_2_3_3, 1, 1 );

    UnitListLayoutLayout->addLayout( layout21, 0, 0 );
    languageChange();
    resize( QSize(678, 218).expandedTo(minimumSizeHint()) );

    // tab order
    setTabOrder( unitTable, addUnitButton );
    setTabOrder( addUnitButton, removeUnitButton );
}

/*
 *  Destroys the object and frees any allocated resources
 */
UnitListLayout::~UnitListLayout()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void UnitListLayout::languageChange()
{
    setWindowTitle( tr( "Unit List" ) );
    addUnitButton->setText( tr( "&Add" ) );
    removeUnitButton->setText( tr( "Rem&ove" ) );
    unitTable->horizontalHeaderItem(0)->setText(tr( "Group" ) );
    unitTable->horizontalHeaderItem(1)->setText(tr( "Cluster" ) );
    unitTable->horizontalHeaderItem(2)->setText(tr( "Structure" ) );
    unitTable->horizontalHeaderItem(3)->setText(tr( "Type" ) );
    unitTable->horizontalHeaderItem(4)->setText(tr( "I.D." ) );
    unitTable->horizontalHeaderItem(5)->setText(tr( "Quality" ) );
    unitTable->horizontalHeaderItem(6)->setText(tr( "Notes" ) );
    textLabel4_2_2_4_2_2_2->setText( tr( "Units" ) );
}

