#ifndef GENERALINFOLAYOUT_H
#define GENERALINFOLAYOUT_H

#include "ui_generalinfolayout.h"


class GeneralInfoLayout : public QWidget, public Ui_GeneralInfoLayout
{
    Q_OBJECT

public:
    explicit GeneralInfoLayout( QWidget* parent = 0 );

};

#endif
