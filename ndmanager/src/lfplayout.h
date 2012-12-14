#ifndef LFPLAYOUT_H
#define LFPLAYOUT_H

#include "ui_lfplayout.h"


class LfpLayout : public QWidget, public Ui_LfpLayout
{
    Q_OBJECT
public:
    explicit LfpLayout( QWidget* parent = 0 );

};

#endif
