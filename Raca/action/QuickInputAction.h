#pragma once

#include "HotkeyAction.h"

class QuickInputAction : public HotkeyAction {
    Q_OBJECT
public:
    explicit QuickInputAction(QAction* action = nullptr, QObject* parent = nullptr);

    bool callback() override;
    QString name() override;
    QString displayName() override;
    QString defaultHotkeyStr() override;
};
