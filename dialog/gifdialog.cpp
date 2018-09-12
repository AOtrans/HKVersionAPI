#include "gifdialog.h"
#include "ui_gifdialog.h"
#include <QMovie>
#include <QDebug>

GifDialog::GifDialog(QString location, QString time, QString path, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GifDialog)
{
    ui->setupUi(this);

    this->setWindowModality(Qt::WindowModal);

    QMovie *m = new QMovie(path, QByteArray(), this);
    m->setScaledSize(QSize(300,300));
    ui->lbGif->setMovie(m);
    m->start();

    ui->leLocation->setText(location);
    ui->leTime->setText(time);
}

GifDialog::~GifDialog()
{
    delete ui;
}
