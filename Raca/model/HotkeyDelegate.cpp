#include "HotkeyDelegate.h"

#include <QHBoxLayout>
#include <QKeySequenceEdit>
#include <QToolButton>

HotkeyDelegate::HotkeyDelegate(QObject* parent)
    : QStyledItemDelegate { parent }
{
}

QWidget* HotkeyDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    Q_UNUSED(option)
    Q_UNUSED(index)

    QWidget* widget = new QWidget(parent);
    auto* horizontalLayout = new QHBoxLayout(widget);
    horizontalLayout->setContentsMargins(0, 0, 0, 0);

    auto* editor = new QKeySequenceEdit(widget);
    editor->setObjectName("editor");
    connect(editor, &QKeySequenceEdit::editingFinished, this, [editor]() {
        if (editor->keySequence().count() > 1) {
            QKeyCombination value = editor->keySequence()[0];
            QKeySequence shortcut(value);
            editor->setKeySequence(shortcut);
        }
    });
    horizontalLayout->addWidget(editor);

    auto* toolButton = new QToolButton(widget);
    toolButton->setIcon(QIcon(":/ic_remove.svg"));
    connect(toolButton, &QToolButton::clicked, this, [editor]() {
        editor->clear();
    });
    horizontalLayout->addWidget(toolButton);

    return widget;
}

void HotkeyDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
    editor->findChild<QKeySequenceEdit*>("editor")->setKeySequence(QKeySequence(index.data().toString()));
}

void HotkeyDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
    model->setData(index, editor->findChild<QKeySequenceEdit*>("editor")->keySequence().toString());
}
