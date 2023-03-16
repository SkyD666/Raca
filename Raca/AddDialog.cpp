#include "AddDialog.h"
#include "GlobalData.h"
#include "database/DataBaseManager.h"
#include <QMessageBox>
#include <QSet>
#include <QtConcurrent>

AddDialog::AddDialog(QWidget* parent, QString uuid)
    : QDialog(parent)
    , uuid(uuid)
{
    ui.setupUi(this);

    initConnect();

    if (uuid.isEmpty()) {
        setWindowTitle(tr("添加"));
    } else {
        setWindowTitle(tr("修改"));
        getDataFromDb();
    }
}

AddDialog::~AddDialog() { }

void AddDialog::closeEvent(QCloseEvent* e)
{
    if (inserted) {
        accept();
    } else {
        reject();
    }
}

void AddDialog::getDataFromDb()
{
    ui.leTitle->setEnabled(false);
    ui.teArticle->setEnabled(false);
    ui.leTag->setEnabled(false);
    ui.lwTag->setEnabled(false);
    ui.pbAdd->setEnabled(false);
    ui.tbDeleteSelectedTag->setEnabled(false);

    DataBaseManager* db = DataBaseManager::getInstance();
    QFuture<QPair<bool, Article>> articleFuture = QtConcurrent::run([=]() {
        Article article;
        bool rtn = db->getArticleTable()->getData(uuid, article);
        QFuture<QPair<bool, QList<Tag>>> tagFuture = QtConcurrent::run([=]() {
            QList<Tag> tags;
            bool rtn = db->getTagTable()->getDataById(tags, uuid);
            return qMakePair(rtn, tags);
        });
        tagWatcher.setFuture(tagFuture);
        return qMakePair(rtn, article);
    });
    articleWatcher.setFuture(articleFuture);
}

void AddDialog::initConnect()
{
    connect(&addWatcher, &QFutureWatcher<QVariant>::finished, this, [=]() {
        QVariant result = addWatcher.result();
        QString title = tr("提示");
        if (result.isValid()) {
            QMessageBox::information(this, title, tr("添加成功！"));
            this->uuid = result.toString();
            inserted = true;
        } else {
            QMessageBox::critical(this, title, tr("添加失败！"));
        }
        if (GlobalData::addedAction == AddedAction::ClearAddDialog) {
            ui.leTitle->clear();
            ui.teArticle->clear();
        } else if (GlobalData::addedAction == AddedAction::CloseAddDialog) {
            close();
        }
    });

    connect(ui.pbAdd, &QAbstractButton::clicked, this, [=]() {
        if (ui.teArticle->toPlainText().isEmpty()) {
            QMessageBox::warning(this, QString(), tr("内容不能为空！"));
            return;
        }
        QFuture<QVariant> addFuture = QtConcurrent::run([=]() {
            QVariant rtn = DataBaseManager::getInstance()->getArticleTable()->insertData(
                Article(ui.leTitle->text(),
                    ui.teArticle->toPlainText(),
                    QDateTime::currentMSecsSinceEpoch(), uuid));
            if (rtn.isValid()) {
                QSet<QString> tags;
                for (int i = 0; i < ui.lwTag->count(); i++) {
                    tags << ui.lwTag->item(i)->text();
                }
                QList list = tags.values();
                auto tagTable = DataBaseManager::getInstance()->getTagTable();
                tagTable->removeData(rtn.toString());
                tagTable->insertData(rtn.toString(), list);
            }
            return rtn;
        });
        addWatcher.setFuture(addFuture);
    });

    connect(ui.leTag, &QLineEdit::returnPressed, this, [=]() {
        ui.lwTag->clearSelection();
        QString tag = ui.leTag->text();
        if (tag.isEmpty()) {
            return;
        }
        auto existItems = ui.lwTag->findItems(tag, Qt::MatchFixedString);
        if (existItems.count() == 0) {
            ui.lwTag->addItem(tag);
            ui.lwTag->item(ui.lwTag->count() - 1)->setSelected(true);
        } else {
            for (auto item : existItems) {
                item->setSelected(true);
            }
        }
        ui.leTag->clear();
    });

    connect(ui.tbDeleteSelectedTag, &QAbstractButton::clicked, this, [=]() {
        for (auto item : ui.lwTag->selectedItems()) {
            ui.lwTag->removeItemWidget(item);
            delete item;
        }
    });

    connect(&articleWatcher, &QFutureWatcher<QPair<bool, Article>>::finished, this, [=]() {
        QPair<bool, Article> pair = articleWatcher.result();
        if (pair.first) {
            Article& article = pair.second;
            ui.leTitle->setEnabled(true);
            ui.teArticle->setEnabled(true);
            ui.leTitle->setText(article.title);
            ui.teArticle->setText(article.article);
        }
    });

    connect(&tagWatcher, &QFutureWatcher<QPair<bool, QList<Tag>>>::finished, this, [=]() {
        QPair<bool, QList<Tag>> pair = tagWatcher.result();
        if (pair.first) {
            QList<Tag>& tags = pair.second;
            ui.leTag->setEnabled(true);
            ui.lwTag->setEnabled(true);
            ui.pbAdd->setEnabled(true);
            ui.tbDeleteSelectedTag->setEnabled(true);
            for (Tag tag : tags) {
                ui.lwTag->addItem(tag.tag);
            }
        }
    });
}
