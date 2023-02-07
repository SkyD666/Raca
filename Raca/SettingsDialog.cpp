#include "SettingsDialog.h"
#include "GlobalData.h"

const QString SettingsDialog::pageName[] = { tr("常规"), tr("搜索") };

SettingsDialog::SettingsDialog(QWidget* parent)
    : QDialog(parent)
{
    ui.setupUi(this);

    setAttribute(Qt::WA_DeleteOnClose);

    initConnect();
}

SettingsDialog::~SettingsDialog()
{
}

void SettingsDialog::initConnect()
{
    for (QString name : pageName) {
        ui.lwPage->addItem(name);
    }
    connect(ui.lwPage, &QListWidget::currentRowChanged, ui.stackedWidget, &QStackedWidget::setCurrentIndex);

    ui.cbStartWithOS->setChecked(GlobalData::startWithOS());
    connect(ui.cbStartWithOS, &QCheckBox::stateChanged, this, [=](int state) {
        GlobalData::setStartWithOS(state);
    });

    ui.cbMinimizeToTray->setChecked(GlobalData::minimizeToTray);
    connect(ui.cbMinimizeToTray, &QCheckBox::stateChanged, this, [=](int state) {
        GlobalData::minimizeToTray = state;
    });

    for (QString action : AddedAction::getAllActions()) {
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
}
