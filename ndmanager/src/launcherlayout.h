#ifndef LAUNCHERLAYOUT_H
#define LAUNCHERLAYOUT_H

#include "ui_launcherlayout.h"


class LauncherLayout : public QWidget, public Ui_LauncherLayout
{
    Q_OBJECT

public:
    explicit LauncherLayout( QWidget* parent = 0 );

};

#endif
