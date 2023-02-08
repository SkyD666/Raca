#include "TableInfo.h"
#include "ArticleTable.h"
#include "TagTable.h"
#include <algorithm>

QMap<QString, QString> TableInfo::tableName2DisplayName = {};
QMap<QString, QMap<QString, QString>> TableInfo::columnName2DisplayName = {};

TableInfo::TableInfo()
{
}

const QMap<QString, QMap<QString, QString>>& TableInfo::getColumnName2DisplayName()
{
    if (columnName2DisplayName.count() == 0) {
        columnName2DisplayName = {
            { ArticleTable::name, ArticleTable::columnName },
            { TagTable::name, TagTable::columnName },
        };
    }
    return columnName2DisplayName;
}

const QMap<QString, QString>& TableInfo::getTableName2DisplayName()
{
    if (tableName2DisplayName.count() == 0) {
        tableName2DisplayName = {
            { ArticleTable::name, ArticleTable::displayName },
            { TagTable::name, TagTable::displayName }
        };
    }
    return tableName2DisplayName;
}
