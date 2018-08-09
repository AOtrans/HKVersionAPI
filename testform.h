#ifndef TESTFORM_H
#define TESTFORM_H

#include <QWidget>

#include "structs/commonstructs.h"
#include "tools/commonfuncs.h"
#include "tree/mytreeitem.h"
#include "dialog/devicedialog.h"
#include "dialog/displayframe.h"

#define XML_PATH "./Settings/devices.xml"
#define MAX_DISPLAY_FRAME 4
class DeviceData;
class DisplayFrame;
class MyTreeItem;
class ChannelData;

namespace Ui {
class TestForm;
}

class TestForm : public QWidget
{
    Q_OBJECT

    friend class DeviceDialog;
    friend class MyTreeItem;
public:
    explicit TestForm(int argc, char *argv[], QWidget *parent = 0);
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

    DisplayFrame* getFreeFrame();
    bool noMoreFramePlay();

private slots:
//    void on_pbRealPaly_clicked();

//    void on_pbPalyBack_clicked();

//    void on_pbCheckPlayBack_clicked();

//    void on_pbCapture_clicked();

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
    MyTreeItem *m_currentTreeItem = NULL;

    QList<DisplayFrame*> m_displayFrames;
};

#endif // TESTFORM_H
