#ifndef FILELAYOUT_H
#define FILELAYOUT_H

#include "ui_filelayout.h"


class FileLayout : public QWidget, public Ui_FileLayout
{
    Q_OBJECT

public:
    explicit FileLayout( QWidget* parent = 0 );

};

#endif
