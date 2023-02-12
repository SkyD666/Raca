#include "Util.h"
#include "qvariant.h"
#include <QClipboard>
#include <QDebug>
#include <QGuiApplication>

#if defined(Q_OS_WIN)
#include <combaseapi.h>
#include <oleacc.h>
#include <oleauto.h>
#include <tchar.h>
#include <windows.h>
#include <winuser.h>
#elif defined(Q_OS_LINUX)
#elif defined(Q_OS_MAC)
#endif

QVariant Util::lastCaretWindow = QVariant();

Util::Util(QObject* parent)
    : QObject(parent)
{
}

Util::~Util()
{
}

QRegion Util::roundedRect(const QRect& rect, int r)
{
    QRegion region;
    // middle and borders
    region += rect.adjusted(r, 0, -r, 0);
    region += rect.adjusted(0, r, 0, -r);
    // top left
    QRect corner(rect.topLeft(), QSize(r * 2, r * 2));
    region += QRegion(corner, QRegion::Ellipse);
    // top right
    corner.moveTopRight(rect.topRight());
    region += QRegion(corner, QRegion::Ellipse);
    // bottom left
    corner.moveBottomLeft(rect.bottomLeft());
    region += QRegion(corner, QRegion::Ellipse);
    // bottom right
    corner.moveBottomRight(rect.bottomRight());
    region += QRegion(corner, QRegion::Ellipse);
    return region;
}

QString Util::escapedCSV(QString unexc)
{
    if (!unexc.contains(','))
        return unexc;
    return '\"' + unexc.replace('\"', QStringLiteral("\"\"")) + '\"';
}

QPoint Util::getEditCursorPos()
{
#if defined(Q_OS_WIN)
    CoInitialize(NULL);
    GUITHREADINFO pg = {};
    pg.cbSize = sizeof(pg);
    GetGUIThreadInfo(NULL, &pg);
    HWND hwnd = pg.hwndCaret ? pg.hwndCaret : GetForegroundWindow();
    lastCaretWindow.setValue(hwnd);
    IAccessible* acc = nullptr;
    HRESULT result = AccessibleObjectFromWindow(hwnd, OBJID_CARET, IID_IAccessible, (void**)(&acc));
    if (S_OK == result) {
        long x, y, w, h;
        VARIANT varCaret = {};
        varCaret.vt = VT_I4;
        varCaret.lVal = CHILDID_SELF;
        acc->accLocation(&x, &y, &w, &h, varCaret);
        acc->Release();
        CoUninitialize();
        return QPoint(x, y);
    } else {
        qDebug() << result;
    }
    CoUninitialize();
    return QPoint();
#elif defined(Q_OS_LINUX)
    return QPoint();
#elif defined(Q_OS_MAC)
    return QPoint();
#else
    return QPoint();
#endif
}

bool Util::sendText(QString text, QVariant dst)
{
#if defined(Q_OS_WIN)
    if (dst.isNull()) {
        return false;
    }
    HWND hwnd = dst.value<HWND>();
    IAccessible* acc = nullptr;
    HRESULT result = AccessibleObjectFromWindow(hwnd, OBJID_CARET, IID_IAccessible, (void**)(&acc));
    if (S_OK == result) {
        VARIANT varCaret = {};
        VariantInit(&varCaret);
        varCaret.vt = VT_I4;
        varCaret.lVal = CHILDID_SELF;
        wchar_t* array = new wchar_t[text.length() + 1];
        text.toWCharArray(array);
        array[text.length()] = 0;
        BSTR bstr = SysAllocString(array);
        HRESULT putResult = acc->put_accValue(varCaret, bstr);
        qDebug() << putResult;
        return putResult == S_OK;
    }
    return false;
#elif defined(Q_OS_LINUX)
    return false;
#elif defined(Q_OS_MAC)
    return false;
#else
    return false;
#endif
}
