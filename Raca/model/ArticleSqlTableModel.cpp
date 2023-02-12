#include "model/ArticleSqlTableModel.h"
#include "database/ArticleTable.h"
#include <QDateTime>

ArticleSqlTableModel::ArticleSqlTableModel(QObject* parent, const QSqlDatabase& db)
    : QSqlTableModel(parent, db)
{
}

ArticleSqlTableModel::~ArticleSqlTableModel()
{
}

Qt::ItemFlags ArticleSqlTableModel::flags(const QModelIndex& index) const
{
    if (ArticleTable::timestampColumnName
            .contains(QSqlTableModel::headerData(index.column(), Qt::Horizontal, HeaderColumnNameRole))) {
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    } else {
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
    }
}

QVariant ArticleSqlTableModel::data(const QModelIndex& index, int role) const
{
    QVariant value = QSqlTableModel::data(index, role);

    if (role == Qt::DisplayRole
        && ArticleTable::timestampColumnName
               .contains(QSqlTableModel::headerData(index.column(), Qt::Horizontal, HeaderColumnNameRole))) {
        auto datetime = QDateTime::fromMSecsSinceEpoch(value.toLongLong());
        return QVariant(datetime.toString("yyyy-MM-dd hh:mm:ss"));
    }

    return QSqlTableModel::data(index, role);
}
