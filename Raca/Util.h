#pragma once

#include <QObject>
#include <QPoint>
#include <QVariant>

class Util : public QObject {
    Q_OBJECT

public:
    Util(QObject* parent);
    ~Util();

    static QString escapedCSV(QString unexc);

    static QPoint getEditCursorPos();

    static QVariant lastCaretWindow;

    static bool sendText(QString text, QVariant dst = lastCaretWindow);
};
