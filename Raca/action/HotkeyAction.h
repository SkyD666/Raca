#include <QAction>
#include <QHotkey>
#include <QObject>

#pragma once

class HotkeyAction : public QObject {
    Q_OBJECT
public:
    explicit HotkeyAction(QAction* action = nullptr, QObject* parent = nullptr);

    virtual bool callback() = 0;

    virtual QString name() = 0;
    virtual QString displayName() = 0;
    virtual QString defaultHotkeyStr() = 0;

    virtual void setHotkeyPtr(QHotkey* hotkeyPtr);
    virtual QHotkey* hotkeyPtr();
    virtual void setHotkeyStr(QString hotkey);
    virtual QString hotkeyStr();

protected:
    QAction* action;
    QHotkey* mHotkeyPtr;
    QString mHotkeyStr;
};
