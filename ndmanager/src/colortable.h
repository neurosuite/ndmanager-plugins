#ifndef COLORTABLE_H
#define COLORTABLE_H

#include <QTableWidget>

class ColorTable : public QTableWidget
{
    Q_OBJECT
public:
    explicit ColorTable(QWidget *parent = 0);
    ~ColorTable();
Q_SIGNALS:
    void modified();
protected:
    void mousePressEvent (QMouseEvent * event);
};

#endif // COLORTABLE_H
