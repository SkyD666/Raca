#include "Migration.h"

Migration::Migration(int from, int to)
{
    this->from = from;
    this->to = to;
}

Migration::~Migration()
{

}

int Migration::getFrom() const
{
    return from;
}

int Migration::getTo() const
{
    return to;
}
