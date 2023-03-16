#include "TagTable.h"
#include <QDateTime>
#include <QRandomGenerator>
#include <QSqlRecord>

const QMap<QString, QString> TagTable::columnName = {
    { "articleUuid", QObject::tr("段落UUID") },
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

bool TagTable::insertData(QString articleUuid, QList<QString>& tags)
{
    if (!database->isOpen() && !database->open()) {
        return false;
    }
    QSqlQuery query(*database);

    query.prepare("INSERT OR IGNORE INTO Tag (articleUuid, tag, createTime) "
                  "VALUES (:articleUuid, :tag, :createTime)");
    query.bindValue(":articleUuid", QVariantList(tags.count(), articleUuid));
    query.bindValue(":tag", tags);
    query.bindValue(":createTime", QVariantList(tags.count(), QDateTime::currentMSecsSinceEpoch()));

    if (!query.execBatch()) {
        QSqlError lastError = query.lastError();
        qDebug() << lastError << lastError.driverText();
        return false;
    }
    return true;
}

bool TagTable::getDataById(QList<Tag>& list, QString articleUuid)
{
    if (!database->isOpen() && !database->open()) {
        return false;
    }

    QSqlQuery query(*database);
    if (query.prepare("SELECT * FROM " + TagTable::name + " WHERE articleUuid LIKE :articleUuid")) {
        query.bindValue(":articleUuid", articleUuid);
        if (query.exec()) {
            while (query.next()) {
                list.append(Tag(query.value("articleUuid").toString(),
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

bool TagTable::removeData(QString articleUuid)
{
    if (!database->isOpen() && !database->open()) {
        return false;
    }
    QSqlQuery query(*database);

    query.prepare("DELETE FROM Tag WHERE articleUuid LIKE :articleUuid");
    query.bindValue(":articleUuid", articleUuid);

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
                              "articleUuid TEXT NOT NULL, "
                              "tag TEXT NOT NULL, "
                              "createTime LONG NOT NULL, "
                              "PRIMARY KEY (articleUuid, tag))");

    if (success) {
        return true;
    } else {
        QSqlError lastError = query.lastError();
        QString err = lastError.driverText();
        return false;
    }
}
