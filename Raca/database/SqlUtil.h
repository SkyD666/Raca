#include <QString>
#include <QtSql/QSqlDatabase>

#pragma once

class SqlUtil {
public:
    SqlUtil();

    static QString escapeField(QString field, QSqlDatabase* db);
};
