#include "model/ArticleSqlTableModel.h"
#include "database/ArticleTable.h"
#include <QDateTime>

ArticleSqlTableModel::ArticleSqlTableModel(QObject* parent, const QSqlDatabase& db)
    : QSqlTableModel(parent, db)
    , createTimColIndex(-1)
{
    for (int i = 0; i < ArticleTable::columnName.count(); i++) {
        if (ArticleTable::columnName[i].first == "createTime") {
            createTimColIndex = i;
            break;
        }
    }
}

ArticleSqlTableModel::~ArticleSqlTableModel()
{
}

Qt::ItemFlags ArticleSqlTableModel::flags(const QModelIndex& index) const
{
    if (index.column() == createTimColIndex) {
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    } else {
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
    }
}

QVariant ArticleSqlTableModel::data(const QModelIndex& index, int role) const
{
    QVariant value = QSqlTableModel::data(index, role);

    if (role == Qt::DisplayRole && index.column() == createTimColIndex) {
        auto datetime = QDateTime::fromMSecsSinceEpoch(value.toLongLong());
        return QVariant(datetime.toString("yyyy-MM-dd hh:mm:ss"));
    }

    return QSqlTableModel::data(index, role);
}
