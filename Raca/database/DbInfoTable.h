#pragma once

#include "BaseTable.h"
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlDriver>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>

class DbInfoTable : public BaseTable {
public:
    static const QString name;
    static const QList<QPair<QString, QString>> columnName;

    DbInfoTable(QSqlDatabase* database, int appNeedVersion);

    int getDbVersion();

    bool exists();

    bool initTable() override;

protected:
    int appNeedVersion;

    int dbVersion;

    bool createTable() override;
};
