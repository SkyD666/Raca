#pragma once

#include "BaseTable.h"
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlDriver>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>

class DbInfoTable : public BaseTable {
public:
    static inline const QString name = "DbInfo";
    static const QList<QPair<QString, QString>> columnName;

    DbInfoTable(QSqlDatabase* database, int appNeedVersion);

    int getDbVersion();

    bool exists();

    bool initTable() override;

protected:
    int appNeedVersion;

    int dbVersion = -1;

    bool createTable() override;
};
