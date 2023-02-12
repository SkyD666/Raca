#pragma once

#include <QObject>
#include <QPoint>
#include <QRegion>
#include <QVariant>

class Util : public QObject {
    Q_OBJECT

public:
    Util(QObject* parent);
    ~Util();

    static QRegion roundedRect(const QRect& rect, int r);

    static QString escapedCSV(QString unexc);

    static QPoint getEditCursorPos();

    static QVariant lastCaretWindow;

    static bool sendText(QString text, QVariant dst = lastCaretWindow);
};
