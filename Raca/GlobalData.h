#pragma once

#include <QHash>
#include <QObject>
#include <action/HotkeyAction.h>

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

    static QMap<QString, HotkeyAction*> hotkeys;

    static QString license;

    static void initHotkeys(QList<HotkeyAction*> actions);
    static void registerHotkeys();
    static void removeHotkeys();

    static void setDarkMode();

private:
    static bool hotkeysInited;
};

class AddedAction {
public:
    const inline static QString NoAction = "NoAction";
    const inline static QString CloseAddDialog = "CloseAddDialog";
    const inline static QString ClearAddDialog = "ClearAddDialog";

    static const QList<QString> getAllActions()
    {
        return QList<QString>() << NoAction << CloseAddDialog << ClearAddDialog;
    }

    static const QString getDisplayName(QString action)
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
