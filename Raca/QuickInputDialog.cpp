#include "QuickInputDialog.h"
#include "GlobalData.h"
#include "Util.h"
#include "database/DataBaseManager.h"
#include <QClipboard>
#include <QGraphicsDropShadowEffect>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QPalette>
#include <QToolTip>
#include <QtConcurrent>

QuickInputDialog::QuickInputDialog(QWidget* parent)
    : QDialog(parent, Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint)
{
    ui.setupUi(this);

    ui.widgetRounded->setStyleSheet(QString("#widgetRounded { background-color: %1; border-radius: 16px }")
                                        .arg(palette().color(QDialog::backgroundRole()).name()));
    ui.widgetRounded->setBackgroundRole(QDialog::backgroundRole());
    auto* effect = new QGraphicsDropShadowEffect(ui.widgetRounded);
    effect->setBlurRadius(10);
    effect->setOffset(0);
    ui.widgetRounded->setGraphicsEffect(effect);

    setAttribute(Qt::WA_DeleteOnClose);
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);

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
        if (row < list.count() && row >= 0) {
            QGuiApplication::clipboard()->setText(list[row].article);
            close();
        }
    });

    connect(ui.tableWidget, &QTableWidget::currentCellChanged, this,
        [=](int currentRow, int, int, int) {
            if (currentRow < list.count() && currentRow >= 0) {
                QToolTip::showText(mapToGlobal(this->rect().topRight()), list[currentRow].article, this);
            }
        });

    connect(&watcher, &QFutureWatcher<bool>::finished, this, [=]() {
        if (watcher.result()) {
            QTableWidgetItem* titleItem = nullptr;
            QTableWidgetItem* articleItem = nullptr;
            ui.tableWidget->setRowCount(0);
            if (list.count()) {
                ui.tableWidget->setFocus();
            }
            int row = ui.tableWidget->rowCount();
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
