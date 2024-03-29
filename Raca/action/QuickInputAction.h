#pragma once

#include "HotkeyAction.h"
#include "QuickInputDialog.h"

class QuickInputAction : public HotkeyAction {
    Q_OBJECT
public:
    explicit QuickInputAction(QAction* action = nullptr, QObject* parent = nullptr);

    bool callback() override;
    const QString name() const override;
    const QString displayName() const override;
    const QString defaultHotkeyStr() const override;

private:
    QuickInputDialog* dialog = nullptr;

    static bool isShow;
};
