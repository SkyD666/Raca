#include "GlobalData.h"
#include "MainWindow.h"
#include <QtWidgets/QApplication>

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);
    QApplication::setOrganizationName("SkyD666");
    QApplication::setApplicationName("Raca");
    QApplication::setApplicationVersion("0.10");
    // 必须设置为false。否则在没有顶层窗口或者隐藏主窗口后，如果有子窗口弹出等，若关闭该弹出的子窗口，就会导致主窗口也被关闭。
    a.setQuitOnLastWindowClosed(false);
    GlobalData::init();
    MainWindow w;
    if (argc > 1 && a.arguments().at(1) == "-autorun") {
        w.showMinimized(); // 先showMinimized，再setWindowState(Qt::WindowNoState)，避免闪烁一下后隐藏
        w.setVisible(false);
        w.setWindowState(Qt::WindowNoState);
    } else {
        w.show();
    }
    bool result = a.exec();
    GlobalData::destory();
    return result;
}
