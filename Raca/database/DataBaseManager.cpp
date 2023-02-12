#include "DataBaseManager.h"
#include <QCoreApplication>
#include <QFile>
#include <QtConcurrent>
#include <algorithm>
#include <database/migration/Migration1To2.h>

Q_GLOBAL_STATIC_WITH_ARGS(DataBaseManager, manager, ("app"))

DataBaseManager::DataBaseManager(QString name)
    : name(name)
{
    if (QSqlDatabase::contains(name)) {
        dataBase = QSqlDatabase::database(name);
        dataBase.open();
        dbInfoTable = new DbInfoTable(&dataBase, appNeedDbVersion);
    } else {
        checkVersionAndOpenDatabase();
    }

    tagTable = new TagTable(&dataBase);
    articleTable = new ArticleTable(&dataBase, tagTable);
}

DataBaseManager::~DataBaseManager()
{
    dataBase.close();
}

DataBaseManager* DataBaseManager::getInstance()
{
    return manager();
}

QSqlDatabase& DataBaseManager::getDataBase()
{
    return dataBase;
}

bool DataBaseManager::isTableExist(QString table)
{
    bool bRet = false;
    if (!dataBase.isOpen() && !dataBase.open()) {
        return bRet;
    }
    QSqlQuery query(dataBase);
    query.prepare("SELECT COUNT(*) FROM sqlite_master WHERE type='table' AND name=:tableName");
    query.bindValue(":tableName", table);
    if (query.exec() && query.next()) {
        if (query.value(0).toInt() > 0) {
            bRet = true;
        }
    }
    return bRet;
}

ArticleTable* DataBaseManager::getArticleTable() const
{
    return articleTable;
}

TagTable* DataBaseManager::getTagTable() const
{
    return tagTable;
}

// BFS
bool DataBaseManager::migration()
{
    // put Migrations here!!!
    QList<Migration*> migrations = {

    };

    QHash<int, QList<QPair<int, Migration*>>> g;
    QList<int> visited;

    int oldDbVersion = dbInfoTable->getDbVersion();

    QQueue<QPair<int, QList<Migration*>>> q;
    q.push_back(qMakePair(oldDbVersion, QList<Migration*>()));

    for (Migration* m : migrations) {
        if (!g.contains(m->getFrom())) {
            g[m->getFrom()] = QList<QPair<int, Migration*>>();
        }
        g[m->getFrom()].append(qMakePair(m->getTo(), m));
    }

    bool result = false;

    while (!q.empty()) {
        const QPair<int, QList<Migration*>> i = q.head();
        q.pop_front();
        if (visited.contains(i.first)) {
            continue;
        }
        visited.append(i.first);

        if (i.first == appNeedDbVersion) {
            if (i.second.count()) {
                result = true;
            }
            for (auto mi : i.second) {
                result &= mi->migration(dataBase);
                if (!result) {
                    break;
                }
            }
            break;
        }

        for (auto d : g[i.first]) {
            if (!visited.contains(d.first)) {
                auto list = i.second;
                list.append(d.second);
                q.push_back(qMakePair(d.first, list));
            }
        }
    }

    for (auto mi : migrations) {
        delete mi;
    }

    return result;
}

bool DataBaseManager::checkVersionAndOpenDatabase()
{
    bool result = true;

    openDatabase();

    int dbFileVersion = dbInfoTable->getDbVersion();
    if (dbFileVersion == -1 || dbFileVersion > appNeedDbVersion) {
        closeAndBackupDatabase();
        openDatabase();
    } else if (dbFileVersion < appNeedDbVersion) {
        if (!migration()) {
            closeAndBackupDatabase();
            openDatabase();
        }
    }
    return result;
}

bool DataBaseManager::openDatabase()
{
    bool result = true;
    if (dataBase.isOpen()) {
        dataBase.close();
        QString connectionName = dataBase.connectionName();
        dataBase = QSqlDatabase();
        QSqlDatabase::removeDatabase(connectionName);
    }
    dataBase = QSqlDatabase::addDatabase("QSQLITE");
    dataBase.setConnectOptions("QSQLITE_ENABLE_REGEXP");
    QString dbPath = QCoreApplication::applicationDirPath() + "/database/";
    QDir dbPathDir = QDir(dbPath);
    if (!dbPathDir.exists()) {
        result &= dbPathDir.mkdir(dbPath);
    }
    QString dbFilePath = dbPath + name + ".db";
    bool dbExists = QFile(dbFilePath).exists();
    dataBase.setDatabaseName(dbFilePath);
    dataBase.open();
    if (!dbInfoTable) {
        delete dbInfoTable;
    }
    dbInfoTable = new DbInfoTable(&dataBase, appNeedDbVersion);
    if (!dbExists) {
        dbInfoTable->initTable();
    }
    return result;
}

bool DataBaseManager::closeAndBackupDatabase()
{
    bool result = true;
    if (dataBase.isOpen()) {
        dataBase.close();
        QString connectionName = dataBase.connectionName();
        dataBase = QSqlDatabase();
        QSqlDatabase::removeDatabase(connectionName);
    }
    QString dbPath = QCoreApplication::applicationDirPath() + "/database/";
    QDir dbPathDir = QDir(dbPath);
    if (!dbPathDir.exists()) {
        result &= dbPathDir.mkdir(dbPath);
    }
    QString dbFilePath = dbPath + name + ".db";
    QFile dbFile(dbFilePath);
    if (dbFile.exists()) {
        result &= dbFile.rename(dbFile.fileName() + "_"
            + QString::number(QDateTime::currentMSecsSinceEpoch()) + "_"
            + QString::number(QRandomGenerator::global()->bounded(0, INT_MAX))
            + ".bak");
    }
    return result;
}
