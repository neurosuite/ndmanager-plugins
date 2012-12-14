#ifndef NEUROSCOPEVIDEOLAYOUT_H
#define NEUROSCOPEVIDEOLAYOUT_H

#include "ui_neuroscopevideolayout.h"


class NeuroscopeVideoLayout : public QWidget, public Ui_NeuroscopeVideoLayout
{
    Q_OBJECT

public:
    explicit NeuroscopeVideoLayout( QWidget* parent = 0 );

};

#endif
