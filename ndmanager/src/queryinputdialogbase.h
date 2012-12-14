#ifndef QUERYINPUTDIALOGBASE_H
#define QUERYINPUTDIALOGBASE_H

#include "ui_queryinputdialogbase.h"


class Query : public QWidget, public Ui_Query
{
    Q_OBJECT

public:
    explicit Query( QWidget* parent = 0 );

};

#endif
