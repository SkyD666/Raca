#pragma once

#include "HotkeyAction.h"

class AddArticleAction : public HotkeyAction {
    Q_OBJECT
public:
    explicit AddArticleAction(std::function<bool()> callback,
        QAction* action = nullptr,
        QObject* parent = nullptr);

    bool callback() override;
    QString name() const override;
    QString displayName() const override;
    QString defaultHotkeyStr() const override;

protected:
    std::function<bool()> mCallback;
};
