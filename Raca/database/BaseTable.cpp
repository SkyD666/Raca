#include "BaseTable.h"
#include "Util.h"
#include <QDebug>
#include <QRandomGenerator>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>

BaseTable::BaseTable(QSqlDatabase* database)
    : database(database)
{
}

bool BaseTable::exportToCSV(QDir dir, QString tableName)
{
    if (!database->open()) {
        return false;
    }

    QFile csvFile = dir.filePath(QString("%1_%2.csv")
                                     .arg(tableName)
                                     .arg(QRandomGenerator::global()->bounded(0, INT_MAX)));

    if (!csvFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    } else {
        QSqlQuery tableNameQuery(*database);
        tableNameQuery.prepare(QString("PRAGMA table_info(%1)").arg(tableName));
        if (!tableNameQuery.exec()) {
            QSqlError lastError = tableNameQuery.lastError();
            qDebug() << lastError << lastError.driverText();
            return false;
        }

        QTextStream stream(&csvFile);
        // export table name
        int columnIndex = 0;
        while (tableNameQuery.next()) {
            if (columnIndex > 0) {
                stream << ',';
            }
            stream << Util::escapedCSV("\"" + tableNameQuery.record().value(1).toString() + "\"");
            columnIndex++;
        }
        stream << "\n";

        // -----------------------

        QSqlQuery query(*database);
        query.prepare(QString("SELECT * FROM %1").arg(tableName));
        if (!query.exec()) {
            QSqlError lastError = query.lastError();
            qDebug() << lastError << lastError.driverText();
            return false;
        }

        while (query.next()) {
            const QSqlRecord record = query.record();
            for (int i = 0, recCount = record.count(); i < recCount; ++i) {
                if (i > 0)
                    stream << ',';
                QVariant v = record.value(i);
                if (v.typeId() == QMetaType::QString) {
                    stream << Util::escapedCSV("\"" + v.toString() + "\"");
                } else {
                    stream << v.toString();
                }
            }
            stream << '\n';
        }
        csvFile.close();
    }

    return true;
}
