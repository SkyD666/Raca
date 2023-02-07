#include "TagTable.h"
#include <QDateTime>
#include <QRandomGenerator>
#include <QSqlRecord>

const QString TagTable::name = "Tag";
const QList<QPair<QString, QString>> TagTable::columnName = {
    qMakePair("id", QObject::tr("ID")),
    qMakePair("tag", QObject::tr("标签")),
    qMakePair("createTime", QObject::tr("创建时间"))
};

TagTable::TagTable(QSqlDatabase* database)
    : BaseTable(database)
{
    TagTable::initTable();
}

TagTable::~TagTable()
{
}

bool TagTable::insertData(int id, QList<QString>& tags)
{
    if (!database->isOpen() && !database->open()) {
        return false;
    }
    QSqlQuery query(*database);

    query.prepare("INSERT OR IGNORE INTO Tag (id, tag, createTime) "
                  "VALUES (:id, :tag, :createTime)");
    query.bindValue(":id", QVariantList(tags.count(), id));
    query.bindValue(":tag", tags);
    query.bindValue(":createTime", QVariantList(tags.count(), QDateTime::currentMSecsSinceEpoch()));

    if (!query.execBatch()) {
        QSqlError lastError = query.lastError();
        qDebug() << lastError << lastError.driverText();
        return false;
    }
    return true;
}

bool TagTable::getDataById(QList<Tag>& list, int id)
{
    if (!database->isOpen() && !database->open()) {
        return false;
    }

    QSqlQuery query(*database);
    if (query.prepare("SELECT * FROM " + TagTable::name + " WHERE id = :id")) {
        query.bindValue(":id", id);
        if (query.exec()) {
            while (query.next()) {
                list.append(Tag(query.value("id").toInt(),
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

bool TagTable::removeData(int id)
{
    if (!database->isOpen() && !database->open()) {
        return false;
    }
    QSqlQuery query(*database);

    query.prepare("DELETE FROM Tag WHERE id = :id");
    query.bindValue(":id", id);

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
                              "id INTEGER NOT NULL, "
                              "tag TEXT NOT NULL, "
                              "createTime LONG NOT NULL, "
                              "PRIMARY KEY (id, tag))");

    if (success) {
        return true;
    } else {
        QSqlError lastError = query.lastError();
        QString err = lastError.driverText();
        return false;
    }
}
