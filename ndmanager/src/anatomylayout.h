#ifndef ANATOMYLAYOUT_H
#define ANATOMYLAYOUT_H

#include "ui_anatomylayout.h"


class AnatomyLayout : public QWidget, public Ui_AnatomyLayout
{
    Q_OBJECT
public:
    explicit AnatomyLayout( QWidget* parent = 0 );
};

#endif
