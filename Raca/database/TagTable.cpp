#include "TagTable.h"
#include <QDateTime>
#include <QRandomGenerator>
#include <QSqlRecord>

const QMap<QString, QString> TagTable::columnName = {
    { "articleId", QObject::tr("ID") },
    { "tag", QObject::tr("标签") },
    { "createTime", QObject::tr("创建时间") }
};
const QList<QString> TagTable::defaultSearchColumnName = { "tag" };

TagTable::TagTable(QSqlDatabase* database)
    : BaseTable(database)
{
    TagTable::initTable();
}

TagTable::~TagTable()
{
}

bool TagTable::insertData(int articleId, QList<QString>& tags)
{
    if (!database->isOpen() && !database->open()) {
        return false;
    }
    QSqlQuery query(*database);

    query.prepare("INSERT OR IGNORE INTO Tag (articleId, tag, createTime) "
                  "VALUES (:articleId, :tag, :createTime)");
    query.bindValue(":articleId", QVariantList(tags.count(), articleId));
    query.bindValue(":tag", tags);
    query.bindValue(":createTime", QVariantList(tags.count(), QDateTime::currentMSecsSinceEpoch()));

    if (!query.execBatch()) {
        QSqlError lastError = query.lastError();
        qDebug() << lastError << lastError.driverText();
        return false;
    }
    return true;
}

bool TagTable::getDataById(QList<Tag>& list, int articleId)
{
    if (!database->isOpen() && !database->open()) {
        return false;
    }

    QSqlQuery query(*database);
    if (query.prepare("SELECT * FROM " + TagTable::name + " WHERE articleId = :articleId")) {
        query.bindValue(":articleId", articleId);
        if (query.exec()) {
            while (query.next()) {
                list.append(Tag(query.value("articleId").toInt(),
                    query.value("tag").toString(),
                    query.value("createTime").toLongLong()));
            }
            query.finish();
            return true;
        }
    }
    QSqlError lastError = query.lastError();
    qDebug() << lastError << lastError.driverText();
    query.finish();
    return false;
}

bool TagTable::removeData(int articleId)
{
    if (!database->isOpen() && !database->open()) {
        return false;
    }
    QSqlQuery query(*database);

    query.prepare("DELETE FROM Tag WHERE articleId = :articleId");
    query.bindValue(":articleId", articleId);

    if (!query.exec()) {
        QSqlError lastError = query.lastError();
        qDebug() << lastError << lastError.driverText();
        return false;
    }
    return true;
}

bool TagTable::initTable()
{
    if (!database->isOpen() && !database->open()) {
        return false;
    }

    TagTable::createTable();

    return true;
}

bool TagTable::createTable()
{
    if (!database->isOpen() && !database->open()) {
        return false;
    }

    QSqlQuery query(*database);
    bool success = query.exec("CREATE TABLE IF NOT EXISTS Tag ("
                              "articleId INTEGER NOT NULL, "
                              "tag TEXT NOT NULL, "
                              "createTime LONG NOT NULL, "
                              "PRIMARY KEY (articleId, tag))");

    if (success) {
        return true;
    } else {
        QSqlError lastError = query.lastError();
        QString err = lastError.driverText();
        return false;
    }
}
