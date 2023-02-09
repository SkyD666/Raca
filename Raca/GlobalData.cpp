#include "GlobalData.h"
#include "database/ArticleTable.h"
#include <QApplication>
#include <QSettings>
#include <QStyleFactory>

const QString AddedAction::NoAction = "NoAction";
const QString AddedAction::CloseAddDialog = "CloseAddDialog";
const QString AddedAction::ClearAddDialog = "ClearAddDialog";

bool GlobalData::minimizeToTray = true;
bool GlobalData::useRegex = false;
QHash<QString, QHash<QString, bool>> GlobalData::searchDomain = {};
QString GlobalData::addedAction = AddedAction::CloseAddDialog;
bool GlobalData::hotkeysInited = false;
QMap<QString, HotkeyAction*> GlobalData::hotkeys = {};

GlobalData::GlobalData(QObject* parent)
    : QObject(parent)
{
}

GlobalData::~GlobalData()
{
}

void GlobalData::init()
{
    license = license.arg(QApplication::applicationDisplayName(), QApplication::applicationVersion());
    searchDomain[ArticleTable::name] = {};
    for (auto& column : ArticleTable::defaultSearchColumnName) {
        searchDomain[ArticleTable::name][column] = true;
    }
    searchDomain[TagTable::name] = {};
    for (auto& column : TagTable::defaultSearchColumnName) {
        searchDomain[TagTable::name][column] = true;
    }
    readSettings();
    setDarkMode();
}

void GlobalData::destory()
{
    writeSettings();
    removeHotkeys();
}

inline QString GlobalData::getSettingsFilePath()
{
    return QCoreApplication::applicationDirPath() + "/Config.ini";
}

void GlobalData::readSettings()
{
    QSettings settings(getSettingsFilePath(), QSettings::IniFormat);

    settings.beginGroup("General");
    minimizeToTray = settings.value("MinimizeToTray", true).toBool();
    addedAction = settings.value("AddedAction", AddedAction::CloseAddDialog).toString();
    settings.endGroup();

    settings.beginGroup("Search");
    useRegex = settings.value("UseRegex", false).toBool();
    {
        settings.beginGroup("SearchDomain");
        auto tables = settings.childGroups();
        for (auto& table : tables) {
            settings.beginGroup(table);
            auto columns = settings.childKeys();
            for (auto& column : columns) {
                if (!table.isEmpty() && !column.isEmpty()) {
                    if (!searchDomain.contains(table)) {
                        searchDomain[table] = QHash<QString, bool>();
                    }
                    searchDomain[table][column] = settings.value(column, false).toBool();
                }
            }
            settings.endGroup();
        }
        settings.endGroup();
    }
    settings.endGroup();
}

void GlobalData::writeSettings()
{
    QSettings settings(getSettingsFilePath(), QSettings::IniFormat);

    settings.beginGroup("General");
    settings.setValue("MinimizeToTray", minimizeToTray);
    settings.setValue("AddedAction", addedAction);
    settings.endGroup();

    settings.beginGroup("Hotkey");
    for (auto i = hotkeys.constBegin(); i != hotkeys.constEnd(); i++) {
        settings.setValue(i.key(), i.value()->hotkeyStr());
    }
    settings.endGroup();

    settings.beginGroup("Search");
    settings.setValue("UseRegex", useRegex);
    {
        settings.beginGroup("SearchDomain");
        for (auto i = searchDomain.constBegin(); i != searchDomain.constEnd(); i++) {
            settings.beginGroup(i.key());
            for (auto j = i.value().constBegin(); j != i.value().constEnd(); j++) {
                settings.setValue(j.key(), j.value());
            }
            settings.endGroup();
        }
        settings.endGroup();
    }
    settings.endGroup();
}

bool GlobalData::setStartWithOS(bool enable)
{
#if defined(Q_OS_WIN)
    QSettings settings("HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
    if (enable) {
        settings.setValue(QApplication::applicationName(),
            QString("\"%1\" -autorun").arg(QApplication::applicationFilePath().replace("/", "\\")));
    } else {
        settings.remove(QApplication::applicationName());
    }
    return true;
#elif defined(Q_OS_LINUX)
    return false;
#elif defined(Q_OS_MAC)
    return false;
#else
    return false;
#endif
}

bool GlobalData::startWithOS()
{
#if defined(Q_OS_WIN)
    QSettings settings("HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
    return settings.value(QApplication::applicationName()).toString()
        == QString("\"%1\" -autorun").arg(QApplication::applicationFilePath().replace("/", "\\"));
#elif defined(Q_OS_LINUX)
    return false;
#elif defined(Q_OS_MAC)
    return false;
#else
    return false;
#endif
}

void GlobalData::initHotkeys(QList<HotkeyAction*> actions)
{
    if (hotkeysInited)
        return;
    hotkeysInited = true;

    for (auto action : actions) {
        if (hotkeys.contains(action->name())) {
            delete hotkeys[action->name()];
        }
        hotkeys[action->name()] = action;
    }

    QSettings settings(getSettingsFilePath(), QSettings::IniFormat);
    settings.beginGroup("Hotkey");
    for (auto i = hotkeys.constBegin(); i != hotkeys.constEnd(); i++) {
        QString configHotkey = settings.value(i.key(), "None").toString();
        if (configHotkey != "None") {
            i.value()->setHotkeyStr(configHotkey);
        } else {
            i.value()->setHotkeyStr(i.value()->defaultHotkeyStr());
        }
    }
    settings.endGroup();

    registerHotkeys();
}

void GlobalData::registerHotkeys()
{
    for (auto i = hotkeys.constBegin(); i != hotkeys.constEnd(); i++) {
        QHotkey* hotkey = i.value()->hotkeyPtr();
        if (!hotkey) {
            hotkey = new QHotkey(QKeySequence(i.value()->hotkeyStr()), true, qApp);
            i.value()->setHotkeyPtr(hotkey);
        }

        if (hotkey->isRegistered()) {
            connect(hotkey, &QHotkey::activated, qApp, [=]() {
                i.value()->callback();
            });
        }
    }
}

void GlobalData::removeHotkeys()
{
    for (auto i = hotkeys.constBegin(); i != hotkeys.constEnd(); i++) {
        if (i.value()->hotkeyPtr()) {
            i.value()->hotkeyPtr()->setRegistered(false);
            i.value()->hotkeyPtr()->disconnect();
            delete i.value()->hotkeyPtr();
            i.value()->setHotkeyPtr(nullptr);
        }
    }
}

void GlobalData::setDarkMode()
{
#ifdef Q_OS_WIN
    QSettings settings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", QSettings::NativeFormat);
//    if (settings.value("AppsUseLightTheme") == 0) {
//        qApp->setStyle(QStyleFactory::create("Fusion"));
//        QPalette darkPalette;
//        QColor darkColor = QColor(45, 45, 45);
//        QColor disabledColor = QColor(127, 127, 127);
//        darkPalette.setColor(QPalette::Window, darkColor);
//        darkPalette.setColor(QPalette::WindowText, Qt::white);
//        darkPalette.setColor(QPalette::Base, QColor(18, 18, 18));
//        darkPalette.setColor(QPalette::AlternateBase, darkColor);
//        darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
//        darkPalette.setColor(QPalette::ToolTipText, Qt::white);
//        darkPalette.setColor(QPalette::Text, Qt::white);
//        darkPalette.setColor(QPalette::Disabled, QPalette::Text, disabledColor);
//        darkPalette.setColor(QPalette::Button, darkColor);
//        darkPalette.setColor(QPalette::ButtonText, Qt::white);
//        darkPalette.setColor(QPalette::Disabled, QPalette::ButtonText, disabledColor);
//        darkPalette.setColor(QPalette::BrightText, Qt::red);
//        darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));

//        darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
//        darkPalette.setColor(QPalette::HighlightedText, Qt::black);
//        darkPalette.setColor(QPalette::Disabled, QPalette::HighlightedText, disabledColor);

//        qApp->setPalette(darkPalette);

//        qApp->setStyleSheet("QToolTip { color: #ffffff; background-color: #2a82da; border: 1px solid white; }");
//    }
#endif
}

QString GlobalData::license = "<h3>%1 v%2</h3>" + tr("<p>一个在本地 记录、查找 抽象段落/评论区小作文 的工具</p>")
    + QString("<p>Copyright<font>&#169;</font> 2023 SkyD666</p>"
              R"(<p>This program is free software: you can redistribute it and/or modify
                 it under the terms of the GNU General Public License as published by
                 the Free Software Foundation, either version 3 of the License, or
                 (at your option) any later version.</p>
                 <p>This program is distributed in the hope that it will be useful,
                 but WITHOUT ANY WARRANTY; without even the implied warranty of
                 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
                 GNU General Public License for more details.</p>
                 <p>You should have received a copy of the GNU General Public License
                 along with this program. If not, see 
                 &lt;<a href='https://www.gnu.org/licenses/'>https://www.gnu.org/licenses/</a>&gt;.</p>)");
