#include "gifdialog.h"
#include "ui_gifdialog.h"
#include <QMovie>
#include <QDebug>
#include <QFile>

GifDialog::GifDialog(QString location, QString time, QString path, QWidget *parent, MyRightTreeItem* trigger) :
    QDialog(parent),
    ui(new Ui::GifDialog)
{
    ui->setupUi(this);
    filepath = path;

    this->setAttribute(Qt::WA_DeleteOnClose);

    this->setWindowModality(Qt::WindowModal);

    QMovie *m = new QMovie(filepath, QByteArray(), this);
    m->setScaledSize(QSize(300,300));
    ui->lbGif->setMovie(m);
    m->start();

    ui->leLocation->setText(location);
    ui->leTime->setText(time);

    this->trigger = trigger;
}

GifDialog::~GifDialog()
{
    delete ui;
}

void GifDialog::on_pbConfirm_clicked()
{
    QFile file(filepath);
    file.rename(filepath.replace(NEW_RECORD, WATCHED));
    trigger->setIcon(QIcon());
    trigger->setBindData(filepath.replace(NEW_RECORD, WATCHED));
    this->close();
}
