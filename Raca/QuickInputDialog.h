#pragma once

#include "database/ArticleTable.h"
#include "ui_QuickInputDialog.h"
#include <QDialog>
#include <QFutureWatcher>
#include <QList>

class QuickInputDialog : public QDialog {
    Q_OBJECT

public:
    QuickInputDialog(QWidget* parent = Q_NULLPTR);
    ~QuickInputDialog();

private:
    Ui::QuickInputDialog ui;

    QPointF mousePressedPos;

    QFutureWatcher<bool> watcher;

    QList<Article> list;

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
};
