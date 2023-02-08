#pragma once

#include <QString>

class TableInfo {
public:
    TableInfo();

    static const QMap<QString, QString>& getTableName2DisplayName();

    static const QMap<QString, QMap<QString, QString>>& getColumnName2DisplayName();

private:
    static QMap<QString, QString> tableName2DisplayName;
    static QMap<QString, QMap<QString, QString>> columnName2DisplayName;
};
