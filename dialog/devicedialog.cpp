#include "devicedialog.h"
#include "ui_devicedialog.h"
#include <QDebug>
#include <QMessageBox>

DeviceDialog::DeviceDialog(DeviceData *ddata, TYPE t, const QStringList& checklist, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DeviceDialog)
{
    ui->setupUi(this);
    this->ddata = ddata;
    this->t = t;
    this->checklist = checklist;

    setAttribute(Qt::WA_DeleteOnClose);

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
        ui->leName->setText(ddata->getDeviceName());
        ui->leSerial->setText(ddata->getSerialNum());
        ui->leIp->setText(ddata->getIP());
        ui->lePort->setText(QString::number(ddata->getPort()));
        ui->leUser->setText(ddata->getUsrName());
    }
}

void DeviceDialog::on_pb_cancel_clicked()
{
    this->close();
}

void DeviceDialog::on_pb_confirm_clicked()
{
    if(t == ALT)
    {
        checklist.removeOne(ddata->getIP());
        if(checklist.indexOf(ui->leIp->text())!=-1)
        {
            QMessageBox::information(this, "info", ui->leIp->text()+" already in devices list");
            return;
        }

        ddata->setDeviceName(ui->leName->text() );
        ddata->setIP(ui->leIp->text() );
        ddata->setPort(ui->lePort->text().toInt() );
        ddata->setUsrName(ui->leUser->text() );

        ddata->setPasswd(ui->lePwd->text() );
        ddata->getChannelData().clear();

        emit altDevice(ddata);
    }
    else if(t == ADD)
    {
        if(checklist.indexOf(ui->leIp->text())!=-1)
        {
            QMessageBox::information(this, "info", ui->leIp->text()+" already in devices list");
            return;
        }

        ddata = new DeviceData;
        ddata->setDeviceName(ui->leName->text() );
        ddata->setIP(ui->leIp->text() );
        ddata->setPort(ui->lePort->text().toInt() );
        ddata->setUsrName(ui->leUser->text() );

        ddata->setPasswd(ui->lePwd->text() );

        emit addDevice(ddata);
    }
    else
    {
        qDebug() << "error dialog TYPE" << t;
    }

    this->close();
}
