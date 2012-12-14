#ifndef PARAMETERLAYOUT_H
#define PARAMETERLAYOUT_H

#include "ui_parameterlayout.h"


class ParameterLayout : public QWidget, public Ui_ParameterLayout
{
    Q_OBJECT

public:
    explicit ParameterLayout( QWidget* parent = 0 );

};

#endif
