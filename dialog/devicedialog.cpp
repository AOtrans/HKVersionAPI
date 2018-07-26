#include "devicedialog.h"
#include "ui_devicedialog.h"
#include <QDebug>

DeviceDialog::DeviceDialog(QString id, DeviceData ddata, TYPE t, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DeviceDialog)
{
    ui->setupUi(this);
    this->id = id;
    this->ddata = ddata;
    this->t = t;

    ui->lePwd->setEchoMode(QLineEdit::Password);
    initLe();
}

DeviceDialog::~DeviceDialog()
{
    delete ui;
}

void DeviceDialog::initLe()
{
    if(t == ALT)
    {
        ui->leName->setText(ddata.getDeviceName());
        ui->leSerial->setText(ddata.getSerialNum());
        ui->leIp->setText(ddata.getIP());
        ui->lePort->setText(QString::number(ddata.getPort()));
        ui->leUser->setText(ddata.getUsrName());
    }
}

void DeviceDialog::on_pb_cancel_clicked()
{
    this->close();
}

void DeviceDialog::on_pb_confirm_clicked()
{
    DeviceData *newdd = new DeviceData();

    newdd->setDeviceName(ui->leName->text() );
    newdd->setMapId(ddata.getMapId() );
    newdd->setIP(ui->leIp->text() );
    newdd->setPort(ui->lePort->text().toInt() );
    newdd->setUsrName(ui->leUser->text() );

    newdd->setPasswd(ui->lePwd->text() );
    //newdd->setListchanneldata(ddata.getListchanneldata());

    if(t == ALT)
    {
        emit altDevice(newdd);
    }
    else if(t == ADD)
    {
        emit addDevice(newdd);
    }
    else
    {
        qDebug() << "error dialog TYPE" << t;
    }

    this->close();
}
