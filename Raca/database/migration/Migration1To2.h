#pragma once

#include "Migration.h"

class Migration1To2 : public Migration {
public:
    Migration1To2(int from, int to);
    ~Migration1To2();

    bool migration(QSqlDatabase& dataBase);
};
