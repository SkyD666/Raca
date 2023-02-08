#pragma once

#include "database/DataBaseManager.h"
#include "model/ArticleSqlTableModel.h"
#include "ui_MainWindow.h"
#include <QFutureWatcher>
#include <QSystemTrayIcon>
#include <QtSql/QSqlTableModel>
#include <QtWidgets/QMainWindow>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget* parent = Q_NULLPTR);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent* ev) override;

    void exportToCsv();

private:
    Ui::RacaClass ui;

    DataBaseManager* dataBaseManager;

    ArticleSqlTableModel* model;

    QMenu systemTrayMenu;
    QSystemTrayIcon systemTray;
    bool exiting;

    QFutureWatcher<QList<Tag>> refreshTagsDisplayWatcher;
    QFutureWatcher<QPair<bool, QString>> exportFinishedWatcher;

    void initTray();
    void initMenu();
    void initConnect();
    void resetTableModel();
    void search();
    void openAddDialog(int id = INT_MIN);
    void refreshDetailInfoDisplay(const QModelIndex& current);
    void refreshTagsDisplay(int id);
};
