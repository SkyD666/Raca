#pragma once

#include <QDir>
#include <QSqlDatabase>

class BaseTable {
public:
    BaseTable(QSqlDatabase* database);

    bool exportToCSV(QDir dir, QString tableName);

protected:
    QSqlDatabase* database;

    virtual bool initTable() = 0;

    virtual bool createTable() = 0;
};
