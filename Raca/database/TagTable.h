#pragma once

#include "BaseTable.h"
#include <QDir>
#include <QString>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlDriver>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>

struct Tag {
    Tag(int articleId, QString tag, long long createTime)
    {
        this->articleId = articleId;
        this->tag = tag;
        this->createTime = createTime;
    };
    int articleId;
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

    bool insertData(int articleId, QList<QString>& tag);
    bool getDataById(QList<Tag>& list, int articleId);
    bool removeData(int articleId);

protected:
    bool initTable() override;

    bool createTable() override;
};
