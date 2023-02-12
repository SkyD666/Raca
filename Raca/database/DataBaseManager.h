#pragma once

#include "ArticleTable.h"
#include "DbInfoTable.h"
#include "TagTable.h"
#include <QObject>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlDriver>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>

class DataBaseManager : public QObject {
    Q_OBJECT

public:
    DataBaseManager(QString name = "app");
    ~DataBaseManager();

    static DataBaseManager* getInstance();

    const int appNeedDbVersion = 1;

    QSqlDatabase& getDataBase();

    bool isTableExist(QString table);

    ArticleTable* getArticleTable() const;
    TagTable* getTagTable() const;

protected:
    bool migration();
    bool checkVersionAndOpenDatabase();
    bool openDatabase();
    bool closeAndBackupDatabase();

    QString name;

private:
    QSqlDatabase dataBase;

    DbInfoTable* dbInfoTable;
    ArticleTable* articleTable;
    TagTable* tagTable;
};
