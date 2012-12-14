#ifndef VIDEOLAYOUT_H
#define VIDEOLAYOUT_H

#include "ui_videolayout.h"


class VideoLayout : public QWidget, public Ui_VideoLayout
{
    Q_OBJECT

public:
    explicit VideoLayout( QWidget* parent = 0 );
};

#endif
