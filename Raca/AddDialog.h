#pragma once

#include "database/ArticleTable.h"
#include "database/TagTable.h"
#include "ui_AddDialog.h"
#include <QDialog>
#include <QFutureWatcher>

class AddDialog : public QDialog {
    Q_OBJECT

public:
    AddDialog(QWidget* parent, int id = INT_MIN);
    ~AddDialog();

protected:
    virtual void closeEvent(QCloseEvent* e) override;

    void getDataFromDb();

private:
    Ui::AddDialog ui;

    int id;

    bool inserted;

    void initConnect();

    QFutureWatcher<QVariant> addWatcher;
    QFutureWatcher<QPair<bool, Article>> articleWatcher;
    QFutureWatcher<QPair<bool, QList<Tag>>> tagWatcher;
};
