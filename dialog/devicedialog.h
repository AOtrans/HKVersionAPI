#ifndef DEVICEDIALOG_H
#define DEVICEDIALOG_H

#include <QDialog>
#include <structs/devicedata.h>
class DeviceData;
enum TYPE{ALT, ADD};

namespace Ui {
class DeviceDialog;
}

class DeviceDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DeviceDialog(DeviceData *ddata, TYPE t, QStringList checklist, QString title, QWidget *parent = 0);
    ~DeviceDialog();

    void initLe();
private slots:
    void on_pb_cancel_clicked();

    void on_pb_confirm_clicked();

signals:
    void addDevice(DeviceData*);
    void altDevice(DeviceData*);

private:
    Ui::DeviceDialog *ui;
    QStringList checklist;
    DeviceData *ddata;
    TYPE t;
};

#endif // DEVICEDIALOG_H
