#include "SqlUtil.h"

#include <QSqlField>
#include <QtSql/QSqlDriver>

SqlUtil::SqlUtil()
{
}

QString SqlUtil::escapeField(QString field, QSqlDatabase* db)
{
    QSqlField f;
    f.setMetaType(QMetaType(QMetaType::QString));
    f.setValue(field);
    return db->driver()->formatValue(f);
}
