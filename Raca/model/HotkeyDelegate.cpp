#include "HotkeyDelegate.h"

#include <QKeySequenceEdit>

HotkeyDelegate::HotkeyDelegate(QObject* parent)
    : QStyledItemDelegate { parent }
{
}

QWidget* HotkeyDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    Q_UNUSED(option)
    Q_UNUSED(index)
    return new QKeySequenceEdit(parent);
}

void HotkeyDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
    qobject_cast<QKeySequenceEdit*>(editor)->setKeySequence(QKeySequence(index.data().toString()));
}

void HotkeyDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
    model->setData(index, qobject_cast<QKeySequenceEdit*>(editor)->keySequence().toString());
}
