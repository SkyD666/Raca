#include "QuickInputDialog.h"
#include "GlobalData.h"
#include "Util.h"
#include "database/DataBaseManager.h"
#include <QClipboard>
#include <QMouseEvent>
#include <QtConcurrent>

QuickInputDialog::QuickInputDialog(QWidget* parent)
    : QDialog(parent, Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint)
{
    ui.setupUi(this);

    setAttribute(Qt::WA_DeleteOnClose);

    QPoint p = Util::getEditCursorPos();
    if (!p.isNull()) {
        move(p + QPoint(0, 30));
    }

    ui.tableWidget->horizontalHeader()->setVisible(false);
    ui.tableWidget->setColumnCount(2);
    ui.tableWidget->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    ui.tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui.leInput->setFocus();

    connect(ui.tableWidget, &QuickInputTableWidget::returnPressed, this, [=]() {
        int row = ui.tableWidget->currentRow();
        if (row < list.count()) {
            QGuiApplication::clipboard()->setText(list[row].article);
            close();
        }
    });

    connect(&watcher, &QFutureWatcher<bool>::finished, this, [=]() {
        if (watcher.result()) {
            QTableWidgetItem* titleItem = nullptr;
            QTableWidgetItem* articleItem = nullptr;
            int row = ui.tableWidget->rowCount();
            if (list.count()) {
                ui.tableWidget->clear();
                ui.tableWidget->setFocus();
            }
            for (Article& a : list) {
                ui.tableWidget->insertRow(row);
                titleItem = new QTableWidgetItem(a.title);
                ui.tableWidget->setItem(row, 0, titleItem);
                articleItem = new QTableWidgetItem(a.article);
                ui.tableWidget->setItem(row, 1, articleItem);
                row = ui.tableWidget->rowCount();
            }
        }
    });

    connect(ui.leInput, &QLineEdit::returnPressed, this, [=]() {
        QFuture<bool> future = QtConcurrent::run([=]() {
            list.clear();
            bool rtn = DataBaseManager::getInstance()->getArticleTable()->getDataListByKeyword(list, ui.leInput->text(), GlobalData::useRegex);
            return rtn;
        });
        watcher.setFuture(future);
    });
}

QuickInputDialog::~QuickInputDialog()
{
}

void QuickInputDialog::mousePressEvent(QMouseEvent* event)
{
    mousePressedPos = event->pos();
    setCursor(Qt::ClosedHandCursor);
    QDialog::mousePressEvent(event);
}

void QuickInputDialog::mouseReleaseEvent(QMouseEvent* event)
{
    setCursor(Qt::OpenHandCursor);
    QDialog::mouseReleaseEvent(event);
}

void QuickInputDialog::mouseMoveEvent(QMouseEvent* event)
{
    move((event->globalPosition() - mousePressedPos).toPoint());
    QDialog::mouseMoveEvent(event);
}
