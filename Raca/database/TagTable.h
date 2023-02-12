#pragma once

#include "BaseTable.h"
#include <QDir>
#include <QString>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlDriver>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>

struct Tag {
    Tag(int id, QString tag, long long createTime)
    {
        this->id = id;
        this->tag = tag;
        this->createTime = createTime;
    };
    int id;
    QString tag;
    long long createTime;
};

class TagTable : public BaseTable {

public:
    static inline const QString name = "Tag";
    static inline const QString displayName = QObject::tr("标签");
    static const QMap<QString, QString> columnName;
    static const QList<QString> defaultSearchColumnName;

    TagTable(QSqlDatabase* database);
    ~TagTable();

    bool insertData(int id, QList<QString>& tag);
    bool getDataById(QList<Tag>& list, int id);
    bool removeData(int id);

protected:
    bool initTable() override;

    bool createTable() override;
};
