#include "AddArticleAction.h"

AddArticleAction::AddArticleAction(std::function<bool()> callback, QAction* action, QObject* parent)
    : HotkeyAction { action, parent }
    , mCallback(callback)
{
}

bool AddArticleAction::callback()
{
    return mCallback();
}

const QString AddArticleAction::name() const
{
    return "AddArticle";
}

const QString AddArticleAction::displayName() const
{
    return tr("添加小作文");
}

const QString AddArticleAction::defaultHotkeyStr() const
{
    return "Ctrl+Shift+=";
}
