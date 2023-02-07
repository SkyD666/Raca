#include "MainWindow.h"

#include "AddDialog.h"
#include "GlobalData.h"
#include "SettingsDialog.h"
#include "model/WrapDelegate.h"
#include <QAbstractItemModel>
#include <QCloseEvent>
#include <QFileDialog>
#include <QMessageBox>
#include <QtConcurrent>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , dataBaseManager(DataBaseManager::getInstance())
    , exiting(false)
{
    ui.setupUi(this);

    initTray();

    model = new ArticleSqlTableModel(this, dataBaseManager->getDataBase());
    model->setTable(ArticleTable::name);
    model->setEditStrategy(QSqlTableModel::OnFieldChange);

    ui.tvResult->setModel(model);
    ui.tvResult->setItemDelegate(new WrapDelegate(this));
    ui.tvResult->setSortingEnabled(true);
    resetTableModel();

    initMenu();
    initConnect();
}

MainWindow::~MainWindow()
{
    GlobalData::writeSettings();
}

void MainWindow::closeEvent(QCloseEvent* ev)
{
    if (GlobalData::minimizeToTray && !exiting) {
        hide();
        ev->ignore();
        return;
    }
    if (ev->isAccepted()) {
        qApp->exit();
    }
}

void MainWindow::exportToCsv()
{
    QFileDialog dialog;
    QString dirPath = dialog.getExistingDirectory(this, tr("选择导出文件夹"));
    if (dirPath.isEmpty()) {
        return;
    }
    QDir dir(dirPath);

    QFuture<QPair<bool, QString>> exportFinishedFuture = QtConcurrent::run([=]() {
        bool result = true;
        ArticleTable* articleTable = dataBaseManager->getInstance()->getArticleTable();
        TagTable* tagTable = dataBaseManager->getInstance()->getTagTable();
        QString msg;
        result &= articleTable->exportToCSV(dir, ArticleTable::name);
        if (result) {
            msg += tr("%1表导出成功\n").arg(ArticleTable::name);
        } else {
            msg += tr("%1表导出失败\n").arg(ArticleTable::name);
        }
        result &= tagTable->exportToCSV(dir, TagTable::name);
        if (result) {
            msg += tr("%1表导出成功\n").arg(TagTable::name);
        } else {
            msg += tr("%1表导出失败\n").arg(TagTable::name);
        }
        return qMakePair(result, msg);
    });
    exportFinishedWatcher.setFuture(exportFinishedFuture);
}

void MainWindow::initTray()
{
    QAction* actionShow = new QAction(QIcon(":/icon.svg"), tr("显示(&S)..."), &systemTrayMenu);
    connect(actionShow, &QAction::triggered, &systemTrayMenu, [=]() { show(); });
    QAction* actionExit = new QAction(tr("退出(&X)"), &systemTrayMenu);
    connect(actionExit, &QAction::triggered, &systemTrayMenu, [=]() {
        exiting = true;
        close();
    });
    systemTrayMenu.addAction(actionShow);
    systemTrayMenu.addSeparator();
    systemTrayMenu.addAction(actionExit);

    systemTray.setIcon(QIcon(":/icon.svg"));
    systemTray.setToolTip(QApplication::applicationName());
    systemTray.setContextMenu(&systemTrayMenu);

    connect(&systemTray, &QSystemTrayIcon::activated, this, [=](QSystemTrayIcon::ActivationReason reason) {
        switch (reason) {
        case QSystemTrayIcon::Context:
            systemTrayMenu.show();
            break;
        case QSystemTrayIcon::DoubleClick:
        case QSystemTrayIcon::Trigger:
        case QSystemTrayIcon::MiddleClick:
            this->show();
            this->setWindowState(Qt::WindowActive);
            break;
        case QSystemTrayIcon::Unknown:
            break;
        }
    });

    systemTray.show();
}

void MainWindow::initMenu()
{
    connect(ui.actionExportDataBase, &QAction::triggered, this, [=]() { exportToCsv(); });

    connect(ui.actionExit, &QAction::triggered, this, [=]() {
        exiting = true;
        close();
    });

    connect(ui.actionAdd, &QAction::triggered, this, [=]() { openAddDialog(); });

    ui.actionDeleteSelectedRow->setEnabled(false);
    connect(ui.actionDeleteSelectedRow, &QAction::triggered, this, [=]() {
        QFuture<QPair<int, int>> future = QtConcurrent::run([=]() {
            QModelIndexList indexes = ui.tvResult->selectionModel()->selectedIndexes();
            int succeed = 0, failed = 0;
            bool rtn = 0;
            for (QModelIndex& index : indexes) {
                rtn = dataBaseManager->getArticleTable()
                          ->deleteData(model->data(model->index(index.row(), 0)).toInt());
                if (!rtn) {
                    qDebug() << model->lastError().driverText();
                }
                succeed += rtn;
                failed += !rtn;
            }
            return qMakePair(succeed, failed);
        });
        if (!future.isFinished()) {
            future.waitForFinished();
            auto result = future.result();
            model->submitAll();
            QMessageBox::information(this, QString(),
                tr("成功删除%1条记录，失败%2条").arg(result.first).arg(result.second));
            search();
        }
    });

    ui.actionEdit->setEnabled(false);
    connect(ui.actionEdit, &QAction::triggered, this, [=]() {
        int row = ui.tvResult->currentIndex().row();
        openAddDialog(model->data(model->index(row, 0)).toInt());
    });

    ui.actionUseRegex->setChecked(GlobalData::useRegex);
    connect(ui.actionUseRegex, &QAction::toggled, this, [=](bool checked) {
        GlobalData::useRegex = checked;
    });

    QMenu* menuSearchDomain = new QMenu(tr("搜索域"), ui.menuSearch);
    ui.menuSearch->addMenu(menuSearchDomain);
    for (const QPair<QString, QString>& col : ArticleTable::columnName) {
        QAction* action = new QAction(col.second, menuSearchDomain);
        action->setData(QVariant(col.first));
        action->setCheckable(true);
        action->setChecked(GlobalData::searchDomain[col.first]);
        connect(action, &QAction::toggled, menuSearchDomain, [=](bool checked) {
            GlobalData::searchDomain[action->data().toString()] = checked;
        });
        menuSearchDomain->addAction(action);
    }

    connect(ui.actionSettings, &QAction::triggered, this, [=]() {
        auto dialog = new SettingsDialog(this);
        dialog->exec();
        ui.actionUseRegex->setChecked(GlobalData::useRegex);
    });

    connect(ui.actionAboutQt, &QAction::triggered, this, [=]() { qApp->aboutQt(); });

    connect(ui.actionAbout, &QAction::triggered, this, [=]() {
        QMessageBox::about(this, QString(), GlobalData::license);
    });
}

void MainWindow::initConnect()
{
    connect(ui.leSearch, &QLineEdit::returnPressed, this, &MainWindow::search);
    search();

    connect(ui.tvResult->selectionModel(), &QItemSelectionModel::currentRowChanged, this,
        [=](const QModelIndex& current, const QModelIndex& _) {
            int id = model->data(model->index(current.row(), 0)).toInt();
            ui.leTitle->setText(model->data(model->index(current.row(), 1)).toString());
            ui.teArticle->setText(model->data(model->index(current.row(), 2)).toString());
            ui.leCreateTime->setText(model->data(model->index(current.row(), 3)).toString());
            ui.lwTag->clear();

            QFuture<QList<Tag>> refreshTagsDisplayFuture = QtConcurrent::run([=]() {
                QList<Tag> tags;
                DataBaseManager::getInstance()->getTagTable()->getDataById(tags, id);
                return tags;
            });
            refreshTagsDisplayWatcher.setFuture(refreshTagsDisplayFuture);
        });

    connect(&refreshTagsDisplayWatcher, &QFutureWatcher<QList<Tag>>::finished, this, [=]() {
        auto tags = refreshTagsDisplayWatcher.result();
        for (Tag& tag : tags) {
            ui.lwTag->addItem(tag.tag);
        }
    });

    connect(ui.tvResult->selectionModel(), &QItemSelectionModel::selectionChanged, this,
        [=](const QItemSelection& selected, const QItemSelection& _) {
            ui.actionDeleteSelectedRow->setEnabled(selected.count());
            ui.actionEdit->setEnabled(selected.count());
        });

    connect(&exportFinishedWatcher, &QFutureWatcher<QPair<bool, QString>>::finished, this, [=]() {
        auto pair = exportFinishedWatcher.result();
        QMessageBox::information(this, QString(), pair.second);
    });
}

void MainWindow::resetTableModel()
{
    for (const QPair<QString, QString>& name : ArticleTable::columnName) {
        model->setHeaderData(model->fieldIndex(name.first), Qt::Horizontal, name.second);
    }
    // ui.tvResult->hideColumn(0);

    ui.tvResult->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui.tvResult->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    ui.tvResult->verticalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    ui.tvResult->verticalHeader()->setMaximumSectionSize(200);
}

void MainWindow::search()
{
    model->setFilter(dataBaseManager->getArticleTable()->getFilter(ui.leSearch->text()));
    if (!model->select()) {
        QSqlError lastError = model->lastError();
        qDebug() << lastError << lastError.driverText();
    }
}

void MainWindow::openAddDialog(int id)
{
    auto dialog = new AddDialog(this, id);
    if (dialog->exec()) {
        model->submitAll();
        search();
    }
    delete dialog;
}
