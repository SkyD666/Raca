#pragma once

#include "database/ArticleTable.h"
#include "database/TagTable.h"
#include "ui_AddDialog.h"
#include <QDialog>
#include <QFutureWatcher>

class AddDialog : public QDialog {
    Q_OBJECT

public:
    AddDialog(QWidget* parent, QString uuid = "");
    ~AddDialog();

protected:
    virtual void closeEvent(QCloseEvent* e) override;

    void getDataFromDb();

private:
    Ui::AddDialog ui;

    QString uuid;

    bool inserted = false;

    void initConnect();

    QFutureWatcher<QVariant> addWatcher;
    QFutureWatcher<QPair<bool, Article>> articleWatcher;
    QFutureWatcher<QPair<bool, QList<Tag>>> tagWatcher;
};
