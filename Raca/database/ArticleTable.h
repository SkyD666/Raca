#pragma once

#include "BaseTable.h"
#include "TagTable.h"
#include <QDir>
#include <QObject>
#include <QPair>
#include <QString>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlDriver>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>

struct Article {
    Article(QString title = QString(),
        QString article = QString(),
        long long createTime = 0L,
        int id = -1)
    {
        this->title = title;
        this->article = article;
        this->createTime = createTime;
        this->id = id;
    };
    int id;
    QString title;
    QString article;
    long long createTime;
};

class ArticleTable : public BaseTable {

public:
    static inline const QString name = "Article";
    static inline const QString displayName = QObject::tr("段落");
    static const QMap<QString, QString> columnName;
    static const QList<QString> timestampColumnName;
    static const QList<QString> defaultSearchColumnName;

    ArticleTable(QSqlDatabase* database, TagTable* tagTable);
    ~ArticleTable();

    QString getFilter(QString keyword);

    QVariant insertData(Article data);

    bool deleteData(int id);

    bool getData(int id, Article& article);

    bool getDataListByKeyword(QList<Article>& list, QString keyword, bool regex = false);

private:
    TagTable* tagTable;

protected:
    bool initTable() override;

    bool createTable() override;
};
