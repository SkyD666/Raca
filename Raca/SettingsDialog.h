#pragma once

#include "ui_SettingsDialog.h"
#include <QDialog>

class SettingsDialog : public QDialog {
    Q_OBJECT

public:
    SettingsDialog(QWidget* parent);
    ~SettingsDialog();

    const static QString pageName[];

private:
    Ui::SettingsDialog ui;

    void initConnect();
};
