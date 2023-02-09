#pragma once

#include "HotkeyAction.h"

class AddArticleAction : public HotkeyAction {
    Q_OBJECT
public:
    explicit AddArticleAction(std::function<bool()> callback,
        QAction* action = nullptr,
        QObject* parent = nullptr);

    bool callback() override;
    QString name() override;
    QString displayName() override;
    QString defaultHotkeyStr() override;

protected:
    std::function<bool()> mCallback;
};
