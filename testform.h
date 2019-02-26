#ifndef TESTFORM_H
#define TESTFORM_H
#include <QWidget>

#include "structs/commonstructs.h"
#include "tools/commonfuncs.h"
#include "tree/mylefttreeitem.h"
#include "tree/myrighttreeitem.h"
#include "dialog/devicedialog.h"
#include "dialog/displayframe.h"
#include "common.h"

class DeviceData;
class DisplayFrame;
class MyLeftTreeItem;
class ChannelData;

namespace Ui {
class TestForm;
}

class TestForm : public QWidget
{
    Q_OBJECT

    friend class DeviceDialog;
    friend class MyLeftTreeItem;
public:
    explicit TestForm(int argc, char *argv[], int w, int h, QWidget *parent = 0);
    ~TestForm();

    void showVideo(cv::Mat img, ChannelData *cdata);//feed a new img
    void initLeftTree();
    void initRightTree();
    QStringList getCheckList();

    void add2GridLayout(DisplayFrame *);
    void closeChannel(int id, int handle);

    void initSth();//sth todo before initalize
private:
    void showRootMenu(QPoint &point);
    void showDeviceMenu(QPoint &point);

    bool testLogin(QString deviceid);

    void loginAllDevices();
    void logoutAllDevices();

    void startRealPlay(ChannelData *cdata, QStandardItem *item);

    void stopRealPlay(ChannelData *cdata, QStandardItem *item);

    DisplayFrame* getFreeFrame();
    bool noMoreFramePlay();

    QStandardItem* findDateItem(QString date);
    QStandardItem* findDeviceItem(QString deviceSerial, QStandardItem* dateItem);
    QStandardItem* findChannelItem(QString channel, QStandardItem* deviceItem);

    void sortFrames();

private slots:
//    void on_pbRealPaly_clicked();

//    void on_pbPalyBack_clicked();

//    void on_pbCheckPlayBack_clicked();

//    void on_pbCapture_clicked();

    void onLeftTreeRightClicked(QPoint point);

    void showAddDialog(bool);

    void showAltDialog(bool);

    void deleteDevice(bool);

    void altDevice(DeviceData*);

    void addDevice(DeviceData*);

    void onLeftTreeDoubleClicked(const QModelIndex&);

    void onRightTreeDoubleClicked(const QModelIndex&);

    void on_pbcleft_clicked();

    void reloadDevice(bool);

    void on_pbcright_clicked();

    void expandTreeClicked(const QModelIndex&);

    void addRow(QStringList);

private:
    Ui::TestForm *ui;

    QMap<QString, QString> m_nameMap;

    QMap<QString, DeviceData> m_deviceList;
    QStandardItemModel *m_leftTreeModel = NULL;
    QStandardItemModel *m_rightTreeModel_1 = NULL;
    QStandardItemModel *m_rightTreeModel_2 = NULL;

    MyLeftTreeItem *m_currentTreeItem = NULL;

    bool rightTreeExpand = true;
    bool leftTreeExpand =true;

    QList<DisplayFrame*> m_displayFrames;
};

#endif // TESTFORM_H
