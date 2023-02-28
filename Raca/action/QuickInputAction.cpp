#include "QuickInputAction.h"

#include "QuickInputDialog.h"

bool QuickInputAction::isShow = false;

QuickInputAction::QuickInputAction(QAction* action, QObject* parent)
    : HotkeyAction { action, parent }
{
}

bool QuickInputAction::callback()
{
    if (isShow) {
        if (dialog) {
            dialog->resetPosition();
            dialog->activateWindow();
        }
        return true;
    }
    isShow = true;
    dialog = new QuickInputDialog;
    dialog->show();
    dialog->activateWindow();
    connect(dialog, &QObject::destroyed, this, [=](QObject* obj) {
        obj->disconnect();
        dialog = nullptr;
        isShow = false;
    });
    return true;
}

const QString QuickInputAction::name() const
{
    return "QuickInput";
}

const QString QuickInputAction::displayName() const
{
    return tr("快速输入");
}

const QString QuickInputAction::defaultHotkeyStr() const
{
    return "Ctrl+I";
}
