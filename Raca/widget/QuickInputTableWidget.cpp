#include "QuickInputTableWidget.h"

#include <QKeyEvent>

QuickInputTableWidget::QuickInputTableWidget(QWidget* parent)
    : QTableWidget(parent)
{
}

void QuickInputTableWidget::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return) {
        emit returnPressed();
    } else {
        QTableWidget::keyPressEvent(event);
    }
}

void QuickInputTableWidget::mouseDoubleClickEvent(QMouseEvent* event)
{
    emit returnPressed();
}
