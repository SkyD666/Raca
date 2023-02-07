#pragma once

#include <QSqlTableModel>

class ArticleSqlTableModel : public QSqlTableModel {
    Q_OBJECT

public:
    ArticleSqlTableModel(QObject* parent = nullptr, const QSqlDatabase& db = QSqlDatabase());
    ~ArticleSqlTableModel();

    Qt::ItemFlags flags(const QModelIndex& index) const override;

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

private:
    int createTimColIndex;
};
