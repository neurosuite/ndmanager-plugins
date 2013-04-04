#include "colortable.h"
#include <QMouseEvent>
#include <QColorDialog>

ColorTable::ColorTable(QWidget *parent) :
    QTableWidget(parent)
{
}

ColorTable::~ColorTable()
{

}

void ColorTable::mousePressEvent ( QMouseEvent * event)
{
    if(event->button() == Qt::MidButton) {
        QTableWidgetItem *item = itemAt(event->pos());
        if(item) {
            const QColor color(item->text());
            const QColor result = QColorDialog::getColor(color,0);
            if (result.isValid()) {
                item->setText(result.name());
                return;
            }
        }
    }
    QTableWidget::mousePressEvent(event);
}
