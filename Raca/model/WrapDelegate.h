#pragma once

#include <QStyledItemDelegate>
#include <QPainter>

class WrapDelegate : public QStyledItemDelegate {
    Q_OBJECT

public:
    WrapDelegate(QObject* parent);
    ~WrapDelegate();

    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

    void setEditorData(QWidget* editor, const QModelIndex& index) const override;

    void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override;

    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
};
