#include "SettingsDialog.h"
#include "GlobalData.h"
#include <QStringList>
#include <model/HotkeyDelegate.h>
#include <model/ReadOnlyDelegate.h>

const QString SettingsDialog::pageName[] = { tr("常规"), tr("搜索"), tr("全局热键") };
const QString SettingsDialog::pageIcon[] = { ":/ic_settings.svg", ":/ic_search.svg", ":/ic_key_f.svg" };

SettingsDialog::SettingsDialog(QWidget* parent)
    : QDialog(parent)
{
    ui.setupUi(this);

    setAttribute(Qt::WA_DeleteOnClose);

    GlobalData::removeHotkeys();

    initConnect();
}

SettingsDialog::~SettingsDialog()
{
    GlobalData::registerHotkeys();
}

void SettingsDialog::initConnect()
{
    {
        int i = 0;
        for (auto name : pageName) {
            QListWidgetItem* item = new QListWidgetItem(name, ui.lwPage);
            item->setIcon(QIcon(pageIcon[i]));
            ui.lwPage->addItem(item);
            i++;
        }
    }
    connect(ui.lwPage, &QListWidget::currentRowChanged, ui.stackedWidget, &QStackedWidget::setCurrentIndex);

#if defined(Q_OS_WIN)
    ui.cbStartWithOS->setVisible(true);
#else
    ui.cbStartWithOS->setVisible(false);
#endif
    ui.cbStartWithOS->setChecked(GlobalData::startWithOS());
    connect(ui.cbStartWithOS, &QCheckBox::stateChanged, this, [=](int state) {
        GlobalData::setStartWithOS(state);
    });

    ui.cbMinimizeToTray->setChecked(GlobalData::minimizeToTray);
    connect(ui.cbMinimizeToTray, &QCheckBox::stateChanged, this, [=](int state) {
        GlobalData::minimizeToTray = state;
    });

    for (auto& action : AddedAction::getAllActions()) {
        ui.cbAddedAction->addItem(AddedAction::getDisplayName(action), action);
        if (GlobalData::addedAction == action) {
            ui.cbAddedAction->setCurrentIndex(ui.cbAddedAction->count() - 1);
        }
    }
    connect(ui.cbAddedAction, &QComboBox::currentIndexChanged, this, [=](int) {
        GlobalData::addedAction = ui.cbAddedAction->currentData().toString();
    });

    ui.cbUseRegex->setChecked(GlobalData::useRegex);
    connect(ui.cbUseRegex, &QCheckBox::stateChanged, this, [=](int state) {
        GlobalData::useRegex = state;
    });

#if defined(Q_OS_LINUX)
    if (!qGuiApp->nativeInterface<QNativeInterface::QX11Application>()) {
        ui.twHotkey->setEnabled(false);
        ui.twHotkey->setToolTip(tr("全局热键不支持Wayland"));
    }
#endif
    ui.twHotkey->setHorizontalHeaderLabels(QStringList() << tr("功能") << tr("全局热键"));
    ui.twHotkey->setItemDelegateForColumn(0, new ReadOnlyDelegate(ui.twHotkey));
    ui.twHotkey->setItemDelegateForColumn(1, new HotkeyDelegate(ui.twHotkey));
    int row = ui.twHotkey->rowCount();
    QTableWidgetItem* displayNameItem = nullptr;
    QTableWidgetItem* hotkeyStrItem = nullptr;
    for (auto& hotkey : GlobalData::hotkeys) {
        ui.twHotkey->insertRow(row);
        displayNameItem = new QTableWidgetItem(hotkey->displayName());
        displayNameItem->setToolTip(hotkey->displayName());
        ui.twHotkey->setItem(row, 0, displayNameItem);
        hotkeyStrItem = new QTableWidgetItem(hotkey->hotkeyStr(), QTableWidgetItem::UserType + 1);
        hotkeyStrItem->setData(Qt::UserRole + 1, hotkey->name());
        ui.twHotkey->setItem(row, 1, hotkeyStrItem);
        row = ui.twHotkey->rowCount();
    }
    connect(ui.twHotkey, &QTableWidget::itemChanged, this, [=](QTableWidgetItem* item) {
        if (item->type() == QTableWidgetItem::UserType + 1) {
            GlobalData::hotkeys[item->data(Qt::UserRole + 1).toString()]->setHotkeyStr(item->text());
        }
    });
}
