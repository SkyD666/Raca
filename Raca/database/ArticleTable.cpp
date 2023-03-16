#include "ArticleTable.h"
#include "GlobalData.h"
#include "database/SqlUtil.h"
#include "database/TableInfo.h"
#include <QRandomGenerator>
#include <QSqlRecord>
#include <QUuid>
#include <QVariant>

const QMap<QString, QString> ArticleTable::columnName = {
    { "uuid", QObject::tr("UUID") },
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
            QString subSelect = QString("(SELECT DISTINCT articleUuid FROM %1 WHERE FALSE ").arg(table);
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
            filter += " OR uuid IN " + subSelect;
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

    QString& uuid = data.uuid;

    if (data.uuid.isEmpty()) {
        query.prepare("INSERT OR REPLACE INTO Article (uuid, title, article, createTime) "
                      "VALUES (:uuid, :title, :article, :createTime)");
        uuid = QUuid::createUuid().toString(QUuid::WithoutBraces);
        query.bindValue(":uuid", uuid);
        query.bindValue(":title", data.title);
        query.bindValue(":article", data.article);
        query.bindValue(":createTime", data.createTime);
    } else {
        query.prepare("UPDATE Article "
                      "SET title = :title, article = :article, createTime = :createTime "
                      "WHERE uuid LIKE :uuid");
        query.bindValue(":uuid", data.uuid);
        query.bindValue(":title", data.title);
        query.bindValue(":article", data.article);
        query.bindValue(":createTime", data.createTime);
    }

    if (!query.exec()) {
        QSqlError lastError = query.lastError();
        qDebug() << lastError << lastError.driverText();
        return QVariant(QMetaType(QMetaType::UnknownType));
    }
    return uuid;
}

bool ArticleTable::deleteData(QString uuid)
{
    if (!database->isOpen() && !database->open()) {
        return false;
    }
    QSqlQuery query(*database);
    tagTable->removeData(uuid);
    query.prepare("DELETE FROM Article WHERE uuid LIKE :uuid");
    query.bindValue(":uuid", uuid);
    query.exec();
    return true;
}

bool ArticleTable::getData(QString uuid, Article& article)
{
    if (!database->isOpen() && !database->open()) {
        return false;
    }

    QSqlQuery query(*database);
    if (query.prepare("SELECT * FROM " + ArticleTable::name + " WHERE uuid LIKE :uuid")) {
        query.bindValue(":uuid", uuid);
        if (query.exec() && query.next()) {
            article.uuid = query.value("uuid").toString();
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
                query.value(0).toString());
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
                              "uuid TEXT PRIMARY KEY, "
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
