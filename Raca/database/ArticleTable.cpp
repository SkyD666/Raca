﻿#include "ArticleTable.h"
#include "GlobalData.h"
#include "database/SqlUtil.h"
#include "database/TableInfo.h"
#include <QRandomGenerator>
#include <QSqlRecord>
#include <QVariant>

const QString ArticleTable::name = "Article";
const QString ArticleTable::displayName = QObject::tr("段落");
const QMap<QString, QString> ArticleTable::columnName = {
    { "id", QObject::tr("ID") },
    { "title", QObject::tr("标题") },
    { "article", QObject::tr("内容") },
    { "createTime", QObject::tr("创建时间") }
};
const QList<QString> ArticleTable::timestampColumnName = { "createTime" };
const QList<QString> ArticleTable::defaultSearchColumnName = { "title", "article" };

ArticleTable::ArticleTable(QSqlDatabase* database, TagTable* tagTable)
    : BaseTable(database)
    , tagTable(tagTable)
{
    ArticleTable::initTable();
}

ArticleTable::~ArticleTable()
{
}

QString ArticleTable::getFilter(QString keyword)
{
    QString filter = "FALSE";

    // 转义输入，防止SQL注入
    if (GlobalData::useRegex) {
        keyword = SqlUtil::escapeField(keyword, database);
    } else {
        keyword = SqlUtil::escapeField("%" + keyword + "%", database);
    }

    const auto tables = GlobalData::searchDomain.keys();
    for (auto& table : tables) {
        const auto columns = GlobalData::searchDomain[table].keys();

        if (table == ArticleTable::name) {
            for (auto& column : columns) {
                if (columnName.contains(column)) {
                    if (!GlobalData::searchDomain[table][column]) {
                        continue;
                    }
                    if (GlobalData::useRegex) {
                        filter += " OR " + column + " REGEXP " + keyword;
                    } else {
                        filter += " OR " + column + " LIKE " + keyword;
                    }
                }
            }
        } else {
            QString subSelect = QString("(SELECT DISTINCT id FROM %1 WHERE FALSE ").arg(table);
            for (auto& column : columns) {
                if (TableInfo::getColumnName2DisplayName()[table].contains(column)) {
                    if (!GlobalData::searchDomain[table][column]) {
                        continue;
                    }
                    if (GlobalData::useRegex) {
                        subSelect += " OR " + column + " REGEXP " + keyword;
                    } else {
                        subSelect += " OR " + column + " LIKE " + keyword;
                    }
                }
            }
            subSelect += ")";
            filter += " OR id IN " + subSelect;
        }
    }

    if (filter == "FALSE") {
        filter += " OR TRUE";
    }
    return filter;
}

QVariant ArticleTable::insertData(Article data)
{
    if (!database->isOpen() && !database->open()) {
        return QVariant(QMetaType(QMetaType::UnknownType));
    }
    QSqlQuery query(*database);

    if (data.id == INT_MIN) {
        query.prepare("INSERT OR REPLACE INTO Article (title, article, createTime) "
                      "VALUES (:title, :article, :createTime)");
        query.bindValue(":title", data.title);
        query.bindValue(":article", data.article);
        query.bindValue(":createTime", data.createTime);
    } else {
        query.prepare("UPDATE Article "
                      "SET title = :title, article = :article, createTime = :createTime "
                      "WHERE id = :id");
        query.bindValue(":id", data.id);
        query.bindValue(":title", data.title);
        query.bindValue(":article", data.article);
        query.bindValue(":createTime", data.createTime);
    }

    if (!query.exec()) {
        QSqlError lastError = query.lastError();
        qDebug() << lastError << lastError.driverText();
        return QVariant(QMetaType(QMetaType::UnknownType));
    }
    return data.id == INT_MIN ? query.lastInsertId() : data.id;
}

bool ArticleTable::deleteData(int id)
{
    if (!database->isOpen() && !database->open()) {
        return false;
    }
    QSqlQuery query(*database);
    tagTable->removeData(id);
    query.prepare("DELETE FROM Article WHERE id=:id");
    query.bindValue(":id", id);
    query.exec();
    return true;
}

bool ArticleTable::getData(int id, Article& article)
{
    if (!database->isOpen() && !database->open()) {
        return false;
    }

    QSqlQuery query(*database);
    if (query.prepare("SELECT * FROM " + ArticleTable::name + " WHERE id = :id")) {
        query.bindValue(":id", id);
        if (query.exec() && query.next()) {
            article.id = query.value("id").toInt();
            article.title = query.value("title").toString();
            article.article = query.value("article").toString();
            article.createTime = query.value("createTime").toLongLong();
            query.finish();
            return true;
        }
    }
    QSqlError lastError = query.lastError();
    qDebug() << lastError << lastError.driverText();

    return false;
}

bool ArticleTable::getDataListByKeyword(QList<Article>& list, QString keyword, bool regex)
{
    if (!database->isOpen() && !database->open()) {
        return false;
    }

    QSqlQuery query(*database);
    bool preResult = false;
    preResult = query.prepare("SELECT * FROM Article WHERE " + getFilter(keyword));
    if (preResult && query.exec()) {
        while (query.next()) {
            Article data(query.value(1).toString(),
                query.value(2).toString(),
                query.value(3).toLongLong(),
                query.value(0).toInt());
            list.append(data);
        }
        return true;
    } else {
        QSqlError lastError = query.lastError();
        qDebug() << lastError << lastError.driverText();
    }
    return false;
}

bool ArticleTable::initTable()
{
    if (!database->isOpen() && !database->open()) {
        return false;
    }

    ArticleTable::createTable();

    return true;
}

bool ArticleTable::createTable()
{
    if (!database->isOpen() && !database->open()) {
        return false;
    }

    QSqlQuery query(*database);
    bool success = query.exec("CREATE TABLE IF NOT EXISTS Article ("
                              "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                              "title TEXT, "
                              "article TEXT NOT NULL, "
                              "createTime LONG NOT NULL)");

    if (success) {
        return true;
    } else {
        QSqlError lastError = query.lastError();
        qDebug() << lastError << lastError.driverText();
        return false;
    }
}
