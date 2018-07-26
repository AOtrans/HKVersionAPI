#ifndef TESTFORM_H
#define TESTFORM_H

#include <QWidget>
#include <QFrame>
#include <QPainter>

#include "structs/commonstructs.h"
#include "tools/commonfuncs.h"
#include "tree/mytreeitem.h"
#include "dialog/devicedialog.h"

class DeviceData;

#include <QEvent>

class PainterEvent : public QObject
{
    Q_OBJECT
public:
    PainterEvent(QObject* parent):QObject(parent){}

    bool changeMat(cv::Mat mat)
    {
        frameMat = mat;
        return true;
    }

private:
    cv::Mat frameMat;

protected:
    bool eventFilter(QObject *obj, QEvent *event);
};


namespace Ui {
class TestForm;
}

class TestForm : public QWidget
{
    Q_OBJECT

    friend class DeviceDialog;
    friend class MyTreeItem;
public:
    explicit TestForm(QWidget *parent = 0);
    ~TestForm();

    void showVideo(cv::Mat img, ChannelData *cdata);
    void initTree();
private:
    void showRootMenu(QPoint &point);
    void showDeviceMenu(QPoint &point);

    bool testLogin(QString deviceid);

    void loginAllDevice();

    void startRealPlay(ChannelData *cdata);

    void stopRealPlay(ChannelData *cdata);

private slots:
    void on_pbRealPaly_clicked();

    void on_pbPalyBack_clicked();

    void on_pbCheckPlayBack_clicked();

    void on_pbCapture_clicked();

    void onTreeClicked(QPoint point);

    void showAddDialog(bool);

    void showAltDialog(bool);

    void deleteDevice(bool);

    void altDevice(DeviceData*);

    void addDevice(DeviceData*);

    void onTreeDoubleClicked(QModelIndex);

private:
    Ui::TestForm *ui;

    QMap<QString, DeviceData> m_deviceList;
    QStandardItemModel *m_treeModel = NULL;
    MyTreeItem *currentTreeItem = NULL;

    QString xmlPath = "./Settings/devices.xml";

    DeviceData *currentDevice = NULL;
    PainterEvent *m_filter = NULL;
};

#endif // TESTFORM_H
