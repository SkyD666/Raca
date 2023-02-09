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

QString AddArticleAction::name()
{
    return "AddArticle";
}

QString AddArticleAction::displayName()
{
    return tr("添加小作文");
}

QString AddArticleAction::defaultHotkeyStr()
{
    return "Ctrl+Shift+=";
}
