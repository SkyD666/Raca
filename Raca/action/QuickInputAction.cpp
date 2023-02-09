#include "QuickInputAction.h"

#include "QuickInputDialog.h"

QuickInputAction::QuickInputAction(QAction* action, QObject* parent)
    : HotkeyAction { action, parent }
{
}

bool QuickInputAction::callback()
{
    QuickInputDialog* dialog = new QuickInputDialog;
    dialog->show();
    dialog->activateWindow();
    return true;
}

QString QuickInputAction::name() const
{
    return "QuickInput";
}

QString QuickInputAction::displayName() const
{
    return tr("快速输入");
}

QString QuickInputAction::defaultHotkeyStr() const
{
    return "Ctrl+I";
}
