#include "Migration1To2.h"

Migration1To2::Migration1To2(int from, int to)
    : Migration(from, to)
{
}

Migration1To2::~Migration1To2()
{
}

bool Migration1To2::migration(QSqlDatabase& dataBase)
{
    return true;
}
