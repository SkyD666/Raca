#pragma once

#include <QStyledItemDelegate>

class ReadOnlyDelegate : public QStyledItemDelegate {
    Q_OBJECT

public:
    explicit ReadOnlyDelegate(QObject* parent = nullptr);

    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
};
