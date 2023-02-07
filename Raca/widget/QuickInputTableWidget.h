#pragma once

#include <QTableWidget>

class QuickInputTableWidget : public QTableWidget {
    Q_OBJECT

signals:
    void returnPressed();

public:
    QuickInputTableWidget(QWidget* parent = nullptr);

protected:
    void keyPressEvent(QKeyEvent* event) override;

    void mouseDoubleClickEvent(QMouseEvent* event) override;
};
