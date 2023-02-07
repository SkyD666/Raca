#include "DbInfoTable.h"

const QString DbInfoTable::name = "DbInfo";
const QList<QPair<QString, QString>> DbInfoTable::columnName = {
    qMakePair("key", QObject::tr("Key")),
    qMakePair("value", QObject::tr("Value")),
};

DbInfoTable::DbInfoTable(QSqlDatabase* database, int appNeedVersion)
    : BaseTable(database)
    , appNeedVersion(appNeedVersion)
    , dbVersion(-1)
{
}

int DbInfoTable::getDbVersion()
{
    if (dbVersion != -1) {
        return dbVersion;
    }
    if (!database->isOpen() && !database->open()) {
        return -1;
    }
    if (!exists()) {
        return -1;
    }
    QSqlQuery query(*database);

    query.prepare("SELECT value FROM DbInfo WHERE key = 'dbVersion'");

    if (query.exec() && query.next()) {
        dbVersion = query.value("value").toInt();
        return dbVersion;
    }

    QSqlError lastError = query.lastError();
    qDebug() << lastError << lastError.driverText();
    return -1;
}

bool DbInfoTable::exists()
{
    if (!database->isOpen() && !database->open()) {
        return false;
    }
    QSqlQuery query(*database);

    query.prepare("SELECT COUNT(*) FROM sqlite_master WHERE type='table' AND name = 'DbInfo'");

    if (query.exec() && query.next()) {
        return query.value(0).toInt();
    }

    QSqlError lastError = query.lastError();
    qDebug() << lastError << lastError.driverText();
    return false;
}

bool DbInfoTable::initTable()
{
    if (!database->isOpen() && !database->open()) {
        return false;
    }
    return createTable();
}

bool DbInfoTable::createTable()
{
    if (!database->isOpen() && !database->open()) {
        return false;
    }

    QSqlQuery query(*database);
    bool success = query.exec("CREATE TABLE IF NOT EXISTS DbInfo ("
                              "key TEXT NOT NULL PRIMARY KEY, "
                              "value TEXT NOT NULL)");

    success &= query.exec("INSERT OR REPLACE INTO DbInfo (key, value) "
        + QString("VALUES ('dbVersion', '%2')").arg(QString::number(appNeedVersion)));

    if (success) {
        return true;
    } else {
        QSqlError lastError = query.lastError();
        qDebug() << lastError.driverText();
        return false;
    }
}
