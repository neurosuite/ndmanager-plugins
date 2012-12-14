#ifndef CLUSTERSLAYOUT_H
#define CLUSTERSLAYOUT_H

#include "ui_clusterslayout.h"


class ClustersLayout : public QWidget, public Ui_ClustersLayout
{
    Q_OBJECT
public:
    explicit ClustersLayout( QWidget* parent = 0 );
};

#endif
