#ifndef SPIKELAYOUT_H
#define SPIKELAYOUT_H

#include "ui_spikelayout.h"


class SpikeLayout : public QWidget, public Ui_SpikeLayout
{
    Q_OBJECT

public:
    explicit SpikeLayout( QWidget* parent = 0 );
};

#endif
