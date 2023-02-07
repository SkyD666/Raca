#pragma once

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlDriver>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>

class Migration {
public:
    Migration(int from, int to);
    virtual ~Migration();

    virtual bool migration(QSqlDatabase& dataBase) = 0;

    bool operator==(const Migration& a)
    {
        return from == a.from && to == a.to;
    }

    bool operator<(const Migration& a)
    {
        return from < a.from || from == a.from && to < a.to;
    }

    int getFrom() const;
    int getTo() const;

protected:
    int from;
    int to;
};
