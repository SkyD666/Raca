#include "WrapDelegate.h"
#include <QTextEdit>
#include <QRect>
#include <QStyleOptionViewItem>
#include <QtGlobal>

WrapDelegate::WrapDelegate(QObject* parent)
    : QStyledItemDelegate(parent)
{
}

WrapDelegate::~WrapDelegate()
{
}

QWidget* WrapDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    Q_UNUSED(option)
    Q_UNUSED(index)
    return new QTextEdit(parent);
}

void WrapDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
    qobject_cast<QTextEdit*>(editor)->setText(index.data().toString());
}

void WrapDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
    model->setData(index, qobject_cast<QTextEdit*>(editor)->toPlainText());
}

void WrapDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    auto text = index.model()->data(index, Qt::DisplayRole).toString();
    painter->drawText(option.rect, Qt::TextWordWrap | Qt::AlignVCenter, text);
}
