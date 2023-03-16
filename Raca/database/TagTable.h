#pragma once

#include "BaseTable.h"
#include <QDir>
#include <QString>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlDriver>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>

struct Tag {
    Tag(QString articleUuid, QString tag, long long createTime)
    {
        this->articleUuid = articleUuid;
        this->tag = tag;
        this->createTime = createTime;
    };
    QString articleUuid;
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

    bool insertData(QString articleUuid, QList<QString>& tag);
    bool getDataById(QList<Tag>& list, QString articleUuid);
    bool removeData(QString articleUuid);

protected:
    bool initTable() override;

    bool createTable() override;
};
