#include "HotkeyAction.h"

HotkeyAction::HotkeyAction(QAction* action, QObject* parent)
    : QObject { parent }
    , action(action)
{
    if (action) {
        connect(action, &QObject::destroyed, this, [=]() {
            this->action = nullptr;
        });
    }
}

void HotkeyAction::setHotkeyPtr(QHotkey* hotkeyPtr)
{
    this->mHotkeyPtr = hotkeyPtr;
}

QHotkey* HotkeyAction::hotkeyPtr() const
{
    return mHotkeyPtr;
}

void HotkeyAction::setHotkeyStr(QString hotkey)
{
    this->mHotkeyStr = hotkey;
    if (action) {
        action->setShortcut(mHotkeyStr);
    }
}

QString HotkeyAction::hotkeyStr() const
{
    return mHotkeyStr;
}
