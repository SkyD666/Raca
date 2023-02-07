#include "GlobalData.h"
#include "QuickInputDialog.h"
#include "database/ArticleTable.h"
#include <QApplication>
#include <QSettings>
#include <QStyleFactory>

const QString AddedAction::NoAction = "NoAction";
const QString AddedAction::CloseAddDialog = "CloseAddDialog";
const QString AddedAction::ClearAddDialog = "ClearAddDialog";

QString GlobalData::quickInputHotkeyStr = "Ctrl+I";
QHotkey* GlobalData::quickInputHotkey = nullptr;
bool GlobalData::minimizeToTray = true;
bool GlobalData::useRegex = false;
QHash<QString, bool> GlobalData::searchDomain = {};
QString GlobalData::addedAction = AddedAction::CloseAddDialog;

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
    searchDomain = { { ArticleTable::columnName[1].first, true },
        { ArticleTable::columnName[2].first, true } };
    readSettings();
    setHotkeys();
    setDarkMode();
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

    settings.beginGroup("Hotkey");
    quickInputHotkeyStr = settings.value("QuickInput", "Ctrl+I").toString();
    settings.endGroup();

    settings.beginGroup("Search");
    useRegex = settings.value("UseRegex", false).toBool();
    int size = settings.beginReadArray("SearchDomain");
    for (int i = 0; i < size; i++) {
        settings.setArrayIndex(i);
        QString colName = settings.value("ColumnName", "").toString();
        if (!colName.isEmpty()) {
            searchDomain[colName] = settings.value("Include", false).toBool();
        }
    }
    settings.endArray();
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
    settings.setValue("QuickInput", quickInputHotkeyStr);
    settings.endGroup();

    settings.beginGroup("Search");
    settings.setValue("UseRegex", useRegex);
    settings.beginWriteArray("SearchDomain");
    {
        QList<QString> keys = searchDomain.keys();
        int i = 0;
        for (QString key : keys) {
            settings.setArrayIndex(i++);
            settings.setValue("ColumnName", key);
            settings.setValue("Include", searchDomain[key]);
        }
    }
    settings.endArray();
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

void GlobalData::setHotkeys()
{
    if (!quickInputHotkey) {
        quickInputHotkey = new QHotkey(QKeySequence(GlobalData::quickInputHotkeyStr), true, qApp);
    }
    if (quickInputHotkey->isRegistered()) {
        QObject::connect(quickInputHotkey, &QHotkey::activated, qApp, [&]() {
            QuickInputDialog* dialog = new QuickInputDialog;
            dialog->show();
            dialog->activateWindow();
        });
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
