#pragma once

#include "HotkeyAction.h"

class AddArticleAction : public HotkeyAction {
    Q_OBJECT
public:
    explicit AddArticleAction(std::function<bool()> callback,
        QAction* action = nullptr,
        QObject* parent = nullptr);

    bool callback() override;
    const QString name() const override;
    const QString displayName() const override;
    const QString defaultHotkeyStr() const override;

protected:
    std::function<bool()> mCallback;
};
