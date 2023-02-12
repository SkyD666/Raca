#include <QAction>
#include <QHotkey>
#include <QObject>

#pragma once

class HotkeyAction : public QObject {
    Q_OBJECT
public:
    explicit HotkeyAction(QAction* action = nullptr, QObject* parent = nullptr);

    virtual bool callback() = 0;

    const virtual QString name() const = 0;
    const virtual QString displayName() const = 0;
    const virtual QString defaultHotkeyStr() const = 0;

    virtual void setHotkeyPtr(QHotkey* hotkeyPtr);
    virtual QHotkey* hotkeyPtr() const;
    virtual void setHotkeyStr(QString hotkey);
    virtual QString hotkeyStr() const;

protected:
    QAction* action;
    QHotkey* mHotkeyPtr = nullptr;
    QString mHotkeyStr = QString();
};
