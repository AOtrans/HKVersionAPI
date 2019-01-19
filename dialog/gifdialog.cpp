#include "gifdialog.h"
#include "ui_gifdialog.h"
#include <QDebug>
#include <QFile>
#include <QMessageBox>

extern Config*config;
GifDialog::GifDialog(QString location, QString time, QString path, QWidget *parent, MyRightTreeItem* trigger, QTreeView* view) :
    QDialog(parent),
    ui(new Ui::GifDialog)
{
    ui->setupUi(this);

    this->setAttribute(Qt::WA_DeleteOnClose);
    this->setWindowModality(Qt::WindowModal);
    ui->pbNext->setIcon(QIcon(config->RIGHT_ICON));
    ui->pbPre->setIcon(QIcon(config->LEFT_ICON));

    m = new QMovie(this);
    ui->lbGif->setMovie(m);
    this->view = view;

    reinit(location, time, path, trigger);

}



GifDialog::~GifDialog()
{
    delete ui;
}

void GifDialog::reinit(QString location, QString time, QString path, MyRightTreeItem* trigger)
{
    m->stop();

    filepath = path;
    this->trigger = trigger;

    m->setFileName(filepath);
    m->setScaledSize(QSize(300,300));
    m->start();

    ui->leLocation->setText(location);
    ui->leTime->setText(time);
}

void GifDialog::on_pbConfirm_clicked()
{
    QFile file(filepath);
    file.rename(filepath.replace(config->NEW_RECORD, config->WATCHED));
    trigger->setIcon(QIcon());
    trigger->setBindData(filepath.replace(config->NEW_RECORD, config->WATCHED));
    //this->close();
}

void GifDialog::on_pbNext_clicked()
{
    QModelIndex index = trigger->index().sibling(trigger->row()+1,trigger->column());

    if(index.isValid())
    {
        QStandardItemModel* model = (QStandardItemModel *)index.model();
        MyRightTreeItem *item = (MyRightTreeItem*)model->itemFromIndex(index);
        reinit(item->parent()->parent()->index().data().toString(), item->index().data().toString(), item->bindData(), item);

        view->setCurrentIndex(item->index());
    }
    else
    {
        QMessageBox::information(this, "info", "no more record");
    }
}

void GifDialog::on_pbPre_clicked()
{
    QModelIndex index = trigger->index().sibling(trigger->row()-1,trigger->column());
    if(index.isValid())
    {
        QStandardItemModel* model = (QStandardItemModel *)index.model();
        MyRightTreeItem *item = (MyRightTreeItem*)model->itemFromIndex(index);
        reinit(item->parent()->parent()->index().data().toString(), item->index().data().toString(), item->bindData(), item);

        view->setCurrentIndex(item->index());
    }
    else
    {
        QMessageBox::information(this, "info", "no more record");
    }
}
