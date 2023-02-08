#pragma once

#include "../QHotkey/qhotkey.h"
#include <QHash>
#include <QObject>

struct Hotkey;
class AddedAction;

class GlobalData : public QObject {
    Q_OBJECT

public:
    GlobalData(QObject* parent);
    ~GlobalData();

    static void init();
    static void destory();

    static inline QString getSettingsFilePath();

    static void readSettings();

    static void writeSettings();

    static bool setStartWithOS(bool enable);

    static bool startWithOS();

    static bool useRegex;

    static bool minimizeToTray;

    static QString addedAction;

    static QHash<QString, QHash<QString, bool>> searchDomain;

    static QHash<QString, Hotkey> hotkeys;

    static QString license;

    static void registerHotkeys();
    static void removeHotkeys();

    static void setDarkMode();
};

struct Hotkey {
    Hotkey() {};
    Hotkey(QString name, QString displayName, QHotkey* hotkeyPtr,
        QString defaultHotkeyStr = "", QString hotkeyStr = "")
    {
        this->name = name;
        this->displayName = displayName;
        this->hotkeyStr = hotkeyStr;
        this->hotkeyPtr = hotkeyPtr;
        this->defaultHotkeyStr = defaultHotkeyStr;
    }
    QString name;
    QString displayName;
    QString hotkeyStr;
    QString defaultHotkeyStr;
    QHotkey* hotkeyPtr;
};

class AddedAction {
public:
    const static QString NoAction;
    const static QString CloseAddDialog;
    const static QString ClearAddDialog;

    static QList<QString> getAllActions()
    {
        return QList<QString>() << NoAction << CloseAddDialog << ClearAddDialog;
    }

    static QString getDisplayName(QString action)
    {
        if (action == CloseAddDialog) {
            return QObject::tr("关闭对话框");
        } else if (action == ClearAddDialog) {
            return QObject::tr("重置对话框内容");
        } else {
            return QObject::tr("无");
        }
    };
};
