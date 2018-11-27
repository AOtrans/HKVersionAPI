#include "testform.h"
#include "ui_testform.h"
#include <QDebug>
#include <QImage>
#include <stdio.h>
#include <unistd.h>
#include <QMenu>
#include <QUuid>
#include <QProgressBar>
#include "tools/pyloader.h"
#include "dialog/gifdialog.h"
#include <QDir>
#include <QFileInfo>
#include <QHeaderView>
#include <QFileInfoList>
#include <QDate>
extern PYLoader py_loader;

TestForm::TestForm(int argc, char *argv[], int w, int h, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TestForm)
{
    ui->setupUi(this);

    initSth();

    QProgressBar bar;
    bar.setWindowTitle("Loading...");
    bar.setWindowFlags(Qt::WindowTitleHint | Qt::CustomizeWindowHint);
    bar.setMaximum(100);
    bar.setMinimum(0);
    bar.move((w-bar.width()/2)/2, (h-bar.height()/2)/2);
    bar.show();

    qApp->processEvents();
    bar.setValue(25);
    qApp->processEvents();

    //initialize python env
    if(!py_loader.initPY(argc, argv))
    {
        QMessageBox::warning(this, "error", "load PY func failed");
    }

    bar.setValue(50);
    qApp->processEvents();

    //create a defalut frame to show on main widget
    DisplayFrame *frame = new DisplayFrame(this, 0);
    m_displayFrames.append(frame);
    ui->gridLayout->addWidget(frame);

    bar.setValue(75);
    qApp->processEvents();

    //initialize HK SDK
    if(sdkInit(this))
    {
        //reload settings from xml file
        if(!analysis(m_deviceList, XML_PATH))
            QMessageBox::warning(this, "error", "load XML file failed");

        //try to login all devices&init trees
        loginAllDevices();
        initLeftTree();
        initRightTree();
    }


    bar.setValue(100);
    qApp->processEvents();
}

TestForm::~TestForm()
{
    qDebug() << "delete mainform";

    delete ui;

    logoutAllDevices();
    NET_DVR_Cleanup();
    py_loader.deinitPY();
}

//display singal image
void TestForm::showVideo(cv::Mat img, ChannelData *cdata)
{
//    cv::resize(img, img, cv::Size(cdata->frame->width(), cdata->frame->height()));
    cdata->frame->changeMat(img);
    //cdata->frame->update();
//    cdata->frame->getPainter()->changeMat(img);
//    cdata->frame->repaint();
}

void TestForm::initLeftTree()
{
    if(m_leftTreeModel == NULL)
        m_leftTreeModel = new QStandardItemModel(this);

    MyLeftTreeItem *rootItem = new MyLeftTreeItem("Devices", "", lROOT, NULL);


    foreach(QString mapId, m_deviceList.keys())
    {
        //binging the target device data and key inside the map
        DeviceData *ddata = &m_deviceList[mapId];
        MyLeftTreeItem *deviceItem = new MyLeftTreeItem(ddata->getDeviceName(), ddata->getMapId(), lDEVICE, ddata);

        QList<ChannelData> &clist = ddata->getChannelData();
        for(int i = 0; i<clist.size(); i++)
        {
            //binging target cdata inside the channel list, set Item mapid = ""
            ChannelData *cdata = &clist[i];
            MyLeftTreeItem *channelItem = new MyLeftTreeItem(cdata->getChannelName(), "", lCHANNEL, cdata);
            deviceItem->appendRow(channelItem);
        }

        rootItem->appendRow(deviceItem);
    }

    m_leftTreeModel->appendRow(rootItem);
    m_leftTreeModel->setHeaderData(0, Qt::Horizontal, "DevicesTree");

    ui->leftTreeView->setModel(m_leftTreeModel);

    //enable right clicked signal
    ui->leftTreeView->setContextMenuPolicy(Qt::CustomContextMenu);

    //connect clicked signals and slots
    connect(ui->leftTreeView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onLeftTreeRightClicked(QPoint)));
    connect(ui->leftTreeView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(onLeftTreeDoubleClicked(QModelIndex)));

    connect(ui->leftTreeView, SIGNAL(expanded(QModelIndex)), this, SLOT(expandTreeClicked(QModelIndex)));
    connect(ui->leftTreeView, SIGNAL(collapsed(QModelIndex)), this, SLOT(expandTreeClicked(QModelIndex)));

    ui->leftTreeView->expandAll();

}

void TestForm::initRightTree()
{
    if(m_rightTreeModel_1 == NULL)
        m_rightTreeModel_1 = new QStandardItemModel(this);
    if(m_rightTreeModel_2 == NULL)
        m_rightTreeModel_2 = new QStandardItemModel(this);

    MyRightTreeItem *rootItem_1 = new MyRightTreeItem("History List", rROOT);
    MyRightTreeItem *rootItem_2 = new MyRightTreeItem("Today List", rROOT);

    QDir root_dir(PATH_PREFIX);
    foreach (QFileInfo dateInfo, root_dir.entryInfoList(QStringList(), QDir::Dirs | QDir::NoDotAndDotDot)){
        MyRightTreeItem *dateItem = new MyRightTreeItem(dateInfo.fileName(), rDATE);

        if(QDate::currentDate().toString("yyyy-MM-dd") == dateInfo.fileName())
        {
            rootItem_2->appendRow(dateItem);
        }
        else
        {
            rootItem_1->appendRow(dateItem);
        }

        QDir date_dir(dateInfo.filePath());
        foreach (QFileInfo deviceInfo, date_dir.entryInfoList(QStringList(), QDir::Dirs | QDir::NoDotAndDotDot)){
            bool hasGifs = false;
            MyRightTreeItem *deviceItem = new MyRightTreeItem(m_nameMap[deviceInfo.fileName()], rDEVICE, deviceInfo.fileName());

            QDir device_dir(deviceInfo.filePath());
            foreach (QFileInfo channelInfo, device_dir.entryInfoList(QStringList(), QDir::Dirs | QDir::NoDotAndDotDot)){
                MyRightTreeItem *channelItem = new MyRightTreeItem(channelInfo.fileName(), rCHANNEL);

                QDir channel_dir(channelInfo.filePath());
                QStringList filter;
                filter << "*.gif";
                foreach (QFileInfo gifInfo, channel_dir.entryInfoList(filter, QDir::Files)){
                    QString fileName  = gifInfo.fileName().replace("\.gif", "");

                    QString tag = fileName.split("_").at(0);
                    QString label = fileName.split("_").at(1);
                    QDateTime dt = QDateTime::fromTime_t(fileName.split("_").at(2).toInt());

                    MyRightTreeItem *gifItem = NULL;

                    if(tag != WATCHED)
                    {
                        if(label == "1")
                            gifItem = new MyRightTreeItem(dt.toString("hh:mm:ss"), rGIF1, gifInfo.filePath());
                        else if(label == "2")
                            gifItem = new MyRightTreeItem(dt.toString("hh:mm:ss"), rGIF2, gifInfo.filePath());
                    }
                    else
                    {
                        gifItem = new MyRightTreeItem(dt.toString("hh:mm:ss"), rGIF3, gifInfo.filePath());
                    }

                    if(gifItem != NULL)
                        channelItem->appendRow(gifItem);

                    hasGifs = true;
                }

                deviceItem->appendRow(channelItem);
            }

            if(hasGifs)
                dateItem->appendRow(deviceItem);
            else
                delete deviceItem;
        }
    }


    m_rightTreeModel_1->appendRow(rootItem_1);
    m_rightTreeModel_2->appendRow(rootItem_2);

    m_rightTreeModel_1->setHeaderData(0, Qt::Horizontal, "History");
    m_rightTreeModel_2->setHeaderData(0, Qt::Horizontal, "Today");

    ui->rightTreeView_1->setModel(m_rightTreeModel_1);
    ui->rightTreeView_2->setModel(m_rightTreeModel_2);

    connect(ui->rightTreeView_1, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(onRightTreeDoubleClicked(QModelIndex)));
    connect(ui->rightTreeView_2, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(onRightTreeDoubleClicked(QModelIndex)));

    connect(ui->rightTreeView_1, SIGNAL(expanded(QModelIndex)), this, SLOT(expandTreeClicked(QModelIndex)));
    connect(ui->rightTreeView_2, SIGNAL(expanded(QModelIndex)), this, SLOT(expandTreeClicked(QModelIndex)));

    connect(ui->rightTreeView_1, SIGNAL(collapsed(QModelIndex)), this, SLOT(expandTreeClicked(QModelIndex)));
    connect(ui->rightTreeView_2, SIGNAL(collapsed(QModelIndex)), this, SLOT(expandTreeClicked(QModelIndex)));
}

QStringList TestForm::getCheckList()
{
    QStringList ls;

    foreach (QString key, m_deviceList.keys()) {
        ls.append(m_deviceList[key].getIP());
    }

    return ls;
}

void TestForm::add2GridLayout(DisplayFrame *frame)
{
    int count = ui->gridLayout->count();

    int row = count/FRAME_COL_COUNT;
    int col = count%FRAME_COL_COUNT;

    ui->gridLayout->addWidget(frame, row, col);
}

void TestForm::initSth()
{
//    QString save_path = QString(PATH_PREFIX) + "/" + QDate::currentDate().toString("yyyy-MM-dd");
//    QDir dir(save_path);

//    if(!dir.exists())
//        dir.mkpath(save_path);

    ui->pbcleft->setIcon(QIcon(LEFT_ICON));
    ui->pbcright->setIcon(QIcon(RIGHT_ICON));
}

void TestForm::showRootMenu(QPoint &point)
{
    QPoint showP = this->mapToGlobal(point);
    showP.setY(showP.y() + 30);

    QMenu menu;
    QAction *acAdd = new QAction("add Device", &menu);

    connect(acAdd, SIGNAL(triggered(bool)), this, SLOT(showAddDialog(bool)));

    menu.addAction(acAdd);
    menu.exec(showP);
}

void TestForm::showDeviceMenu(QPoint &point)
{
    QPoint showP = this->mapToGlobal(point);
    showP.setY(showP.y() + 30);

    QMenu menu;
    QAction *acDelete = new QAction("delete Device", &menu);
    connect(acDelete, SIGNAL(triggered(bool)), this, SLOT(deleteDevice(bool)));

    QAction *acAlt = new QAction("alt Device", &menu);
    connect(acAlt, SIGNAL(triggered(bool)), this, SLOT(showAltDialog(bool)));

    menu.addAction(acAlt);
    menu.addAction(acDelete);
    menu.exec(showP);
}

bool TestForm::testLogin(QString mapId)
{
    qDebug() << "-------------------Test login------------------------";

    DeviceData *device = &m_deviceList[mapId];

    //先要尝试登陆设备，然后还要判断通道情况
    device->m_iuserid = NET_DVR_Login_V30(device->getIP().toLatin1().data(),
                                          device->getPort(),
                                          device->getUsrName().toLatin1().data(),
                                          device->getPasswd().toLatin1().data(),
                                          &device->m_deviceinfo);

    //登录失败直接返回
    if (device->m_iuserid < 0)
    {
        qDebug() << device->getDeviceName() << "login fail ErrorCode:" << NET_DVR_GetLastError();
        return false;
    }
    else
    {
        QString snum = QString::fromLocal8Bit((char*)device->m_deviceinfo.sSerialNumber);
        qDebug() << "Device Serial:" << snum;
        //set SerialNum to make an unequal dirpath
        device->setSerialNum(snum);


        qDebug() << device->getDeviceName() << "login success";
        NET_DVR_DEVICECFG devicecfg;
        DWORD Bytesreturned;

        //DEVICECFG获取设备参数
        if (!NET_DVR_GetDVRConfig(device->m_iuserid,
                                  NET_DVR_GET_DEVICECFG,0, &devicecfg, sizeof(NET_DVR_DEVICECFG),&Bytesreturned))
        {
            qDebug() << "NET_DVR_GetDVRCFG SDK_LAST_ERROR:" << NET_DVR_GetLastError();
            return false;
        }
        else
        {
            m_nameMap.insert(snum, device->getDeviceName());

            qDebug() << "get DeviceCFG success, Device Name:" << QString((char*)devicecfg.sDVRName);
            qDebug() << "DeviceTYPE:" << devicecfg.byDVRType
                     << "DiskNum:" << devicecfg.byDiskNum
                     << "InputChannelNum" << devicecfg.byChanNum;

            //测试通道信息，填充通道数据
            NET_DVR_IPPARACFG ipcfg;
            DWORD Bytesreturned;

            //通道相关信息(IPPARACFG获取IP接入配置参数, 判断是DVR或Cameral, DVR是IP接入设备)
            if (!NET_DVR_GetDVRConfig(device->m_iuserid, NET_DVR_GET_IPPARACFG,0,
                                      &ipcfg, sizeof(NET_DVR_IPPARACFG),&Bytesreturned))
            {
                //Camera(has no ip connect)
                qDebug() << "(this is Cameral)";
                for (int i=devicecfg.byStartChan;i<=devicecfg.byChanNum ;i++)
                {
                    ChannelData *newChannel = new ChannelData(this);
                    QString name="Cameral";
                    QString num = QString::number(i, 10) ;
                    name.append(num);
                    //填充通道初始属性内容,初始设为TCP+主码流
                    newChannel->setChannelName(name);
                    newChannel->setSerial(snum);
                    newChannel->setChannelNum(i);
                    newChannel->setProtocolType(TCP);
                    newChannel->setStreamType(MAINSTREAM);
                    newChannel->setParentDevice(device);
                    //添加进设备节点
                    qDebug() << "get cameral:" <<name;
                    device->m_qlistchanneldata.append(*newChannel);

                    delete newChannel;
                    newChannel =NULL;
                }

            }
            else //DVR(has ip connect)
            {
                qDebug() << "(this is DVR)";

                //扫描添加模拟通道
                for (int i=0;i< MAX_ANALOG_CHANNUM;i++)
                {
                    if (1== ipcfg.byAnalogChanEnable[i])
                    {
                        ChannelData *newChannel = new ChannelData(this);
                        QString name="ANAOGCameral";
                        QString num = QString::number(i+1, 10) ;
                        name.append(num);
                        //填充通道初始属性内容,初始设为TCP+主码流
                        newChannel->setChannelName(name);
                        newChannel->setSerial(snum);
                        newChannel->setChannelNum(i+1);
                        newChannel->setProtocolType(TCP);
                        newChannel->setStreamType(MAINSTREAM);
                        newChannel->setParentDevice(device);

                        //添加进设备节点
                        qDebug() << "get Analog cameral:" <<name;
                        device->m_qlistchanneldata.append(*newChannel);

                        delete newChannel;
                        newChannel =NULL;
                    }
                }

                //扫描添加IP通道
                for (int i=0;i< MAX_IP_CHANNEL;i++)
                {
                    //QMessageBox::information(this,tr("ipcfg"), tr("ipcfg.struIPChanInfo[%1].byIPID=%2").arg(i).arg(ipcfg.struIPChanInfo[i].byIPID));

                    if (0 != ipcfg.struIPChanInfo[i].byIPID)
                    {
                        ChannelData *newChannel = new ChannelData(this);
                        QString name="IPCameral";
                        QString num = QString::number(ipcfg.struIPChanInfo[i].byIPID, 10) ;
                        name.append(num);
                        //填充通道初始属性内容,初始设为TCP+主码流
                        newChannel->setChannelName(name);
                        newChannel->setSerial(snum);
                        newChannel->setChannelNum(32+ipcfg.struIPChanInfo[i].byIPID);
                        newChannel->setProtocolType(TCP);
                        newChannel->setStreamType(MAINSTREAM);
                        newChannel->setParentDevice(device);

                        //添加进设备节点
                        qDebug() << "get IP cameral:" <<name;
                        device->m_qlistchanneldata.append(*newChannel);

                        delete newChannel;
                        newChannel =NULL;
                    }
                }
            }

            //get devicecfg success
        }

        //login success
        return true;
    }
}

void TestForm::loginAllDevices()
{
    foreach(QString mapId, m_deviceList.keys())
    {
        DeviceData &ddata = m_deviceList[mapId];

        if(!testLogin(mapId))
            QMessageBox::warning(this, "error", "Login device:"+ddata.getDeviceName()+" failed");
    }
}

void TestForm::logoutAllDevices()
{
    foreach(QString mapId, m_deviceList.keys())
    {
        DeviceData &ddata = m_deviceList[mapId];
        int uid = ddata.getUsrID();
        //if has login
        if(uid >= 0)
        {
            ddata.shutdown();
            NET_DVR_Logout(uid);
        }
    }
}

//void TestForm::on_pbRealPaly_clicked()
//{
//    qDebug() << "------------------RealPlay--------------------";

//    if(currentDevice == NULL)
//    {
//        qDebug() << "please login first";
//        return;
//    }

//    int userID = currentDevice->getUsrID();
//    ChannelData currentChannel = currentDevice->m_qlistchanneldata.at(0);

//    int channelNum = currentChannel.getChannelNum();
//    //computed by stream type and protocl type
//    int linkMode = currentChannel.getLinkMode();


//    //-----------------------------------------

//    //设置预览打开的相关参数
//    NET_DVR_CLIENTINFO *clientinfo = new NET_DVR_CLIENTINFO;

//    clientinfo->hPlayWnd = NULL;
//    clientinfo->lChannel = channelNum;
//    clientinfo->lLinkMode = linkMode;

//    char tmp[128] = {0};
//    sprintf(tmp, "%s", currentDevice->getMultiCast().toLatin1().data());
//    clientinfo->sMultiCastIP = tmp;

//    //取流
//    int realHandle = NET_DVR_RealPlay_V30(userID, clientinfo, DataCallBack,NULL,0);
//    qDebug("realPlay---Protocal:%d", clientinfo->lLinkMode);

//    if (realHandle < 0)
//    {
//        QMessageBox::information(this, tr("NET_DVR_RealPlay error"), tr("SDK_LASTERROR=%1").arg(NET_DVR_GetLastError()));
//    }
//    else
//    {
//        if (currentChannel.getChannelNum()>32)
//        {
//            NET_DVR_IPPARACFG ipcfg;
//            DWORD Bytesreturned;
//            if (!NET_DVR_GetDVRConfig(userID, NET_DVR_GET_IPPARACFG,0,
//                                      &ipcfg, sizeof(NET_DVR_IPPARACFG),&Bytesreturned))
//            {
//                QMessageBox::information(this, tr("NET_DVR_GetDVRConfig"), \
//                                         tr("SDK_LAST_ERROR=%1").arg(NET_DVR_GetLastError()));

//                return ;
//            }
//            if (ipcfg.struIPChanInfo[currentChannel.getChannelNum()-32-1].byEnable == 0)
//            {
//                QMessageBox::information(this,tr("NET_DVR_RealPlay error"), \
//                                         tr("该通道不在线，预览失败"));

//                return ;
//            }
//        }

//        //设置设备是否在预览状态标签值
//        currentDevice->setRealPlayLabel(1);
//    }
//}



////unknow problem ,cause too slow
//void TestForm::on_pbPalyBack_clicked()
//{
//    int userID = currentDevice->getUsrID();
//    ChannelData currentChannel = currentDevice->m_qlistchanneldata.at(0);

//    int channelNum = currentChannel.getChannelNum();

//    NET_DVR_TIME remoteplaystarttime;
//    NET_DVR_TIME remoteplaystoptime;

//    //get the begin and end date time
//    remoteplaystarttime.dwYear = 2018;
//    remoteplaystarttime.dwMonth = 7;
//    remoteplaystarttime.dwDay = 12;
//    remoteplaystarttime.dwHour = 0;
//    remoteplaystarttime.dwMinute = 0;
//    remoteplaystarttime.dwSecond = 0;

//    remoteplaystoptime.dwYear = 2018;
//    remoteplaystoptime.dwMonth = 7;
//    remoteplaystoptime.dwDay = 12;
//    remoteplaystoptime.dwHour = 23;
//    remoteplaystoptime.dwMinute = 59;
//    remoteplaystoptime.dwSecond = 59;

//    //----------------------------------------------------------
//    int playBackHandle = NET_DVR_PlayBackByTime(userID, channelNum, &remoteplaystarttime, &remoteplaystoptime, NULL);




//    NET_DVR_SetPlayDataCallBack_V40(playBackHandle, DataCallBack, NULL);

//    if (!NET_DVR_PlayBackControl(playBackHandle, NET_DVR_PLAYSTART, 0, NULL))
//    {
//        qDebug() << "NET_DVR_PlayBackControl failed" << tr("SDK_Last_Error =%1").arg(NET_DVR_GetLastError());
//        return;
//    }
//}

////check file exists from start to end
//void TestForm::on_pbCheckPlayBack_clicked()
//{

//    ChannelData currentChannel = currentDevice->m_qlistchanneldata.at(1);

//    int channelNum = currentChannel.getChannelNum();

//    uint filetype = 0xff;


//    uint fileattr = 0xff;


//    NET_DVR_TIME remotestarttime;
//    NET_DVR_TIME remotestoptime;

//    //get the begin and end date time
//    remotestarttime.dwYear = 2018;
//    remotestarttime.dwMonth = 7;
//    remotestarttime.dwDay = 1;
//    remotestarttime.dwHour = 0;
//    remotestarttime.dwMinute = 0;
//    remotestarttime.dwSecond = 0;

//    remotestoptime.dwYear = 2018;
//    remotestoptime.dwMonth = 7;
//    remotestoptime.dwDay = 12;
//    remotestoptime.dwHour = 23;
//    remotestoptime.dwMinute = 59;
//    remotestoptime.dwSecond = 59;

//    //---------------------------------------------------------------
//    bool bGetFile = true;
//    int ret =0;
//    int hFile =0;

//    //call the hcnetsdk interface
//    NET_DVR_FILECOND *m_pbpDvrFile = new NET_DVR_FILECOND;

//    //give the variant prepare value
//    m_pbpDvrFile->struStartTime = remotestarttime;
//    m_pbpDvrFile->struStopTime = remotestoptime;
//    m_pbpDvrFile->lChannel = channelNum;

//    m_pbpDvrFile->dwIsLocked = fileattr;
//    m_pbpDvrFile->dwFileType = filetype;

//    //Search fot file
//    hFile = NET_DVR_FindFile_V30(currentDevice->getUsrID(), m_pbpDvrFile);
//    if (hFile < 0)
//    {
//        QMessageBox::information(this,tr(" NET_DVR_FindFile_V30 "),
//            tr("SDK_LASTERROR=%1").arg(NET_DVR_GetLastError()));


//        return;
//    }
//    else
//    {

//        NET_DVR_FINDDATA_V30 *fileData = new NET_DVR_FINDDATA_V30;
//        while (bGetFile)
//        {
//            ret = NET_DVR_FindNextFile_V30(hFile, fileData);
//            switch (ret)
//            {
//                case NET_DVR_FILE_SUCCESS:
//                    qDebug() << "find file" << fileData->sFileName;
//                    break;

//                case NET_DVR_FILE_NOFIND:
//                    qDebug() << "no file find";
//                    NET_DVR_FindClose_V30(hFile);
//                    bGetFile = FALSE;
//                    break;

//                case NET_DVR_ISFINDING:
//                    QMessageBox::information(this,tr(" NET_DVR_FindFile_V30"),
//                        tr("Is finding now,wait a moment!"));

//                    usleep(200*1000);

//                    break;

//                case NET_DVR_NOMOREFILE:
//                    qDebug() << "no more file";
//                    NET_DVR_FindClose_V30(hFile);
//                    bGetFile = FALSE;
//                    break;

//                case NET_DVR_FILE_EXCEPTION:
//                    qDebug() << "File Exception!";
//                    NET_DVR_FindClose_V30(hFile);
//                    bGetFile = FALSE;
//                    break;

//                default:
//                    qDebug() << "Error occured";
//                    NET_DVR_FindClose_V30(hFile);
//                    bGetFile = FALSE;
//                    break;
//            }
//        }
//        return ;
//    }
//}

////test if can do capture(about 300ms, too slow)
//void TestForm::on_pbCapture_clicked()
//{
//    int userID = currentDevice->getUsrID();
////    ChannelData currentChannel = currentDevice->m_qlistchanneldata.at(0);

////    int channelNum = currentChannel.getChannelNum();

////    NET_DVR_JPEGPARA para;
////    para.wPicQuality = 0;
////    para.wPicSize = 5;

////    qDebug() << NET_DVR_CaptureJPEGPicture(userID, channelNum, &para, "./1.jpg");

//    char in[200]="<?xml version=\"1.0\" encoding=\"utf-8\"?> \
//            <JpegCaptureAbility version=\"2.0\"> \
//                    <channelNO>1</channelNO>\
//            </JpegCaptureAbility> ";
//    char out[2000];
//    qDebug()<<in;
//    qDebug() << NET_DVR_GetDeviceAbility(userID,DEVICE_JPEG_CAP_ABILITY,in,200,out,2000);
//    qDebug()<<out;

//    qDebug() << NET_DVR_GetLastError();
//}

void TestForm::onLeftTreeRightClicked(QPoint point)
{
    //locate current clicked item
    m_currentTreeItem = (MyLeftTreeItem*)m_leftTreeModel->itemFromIndex(ui->leftTreeView->indexAt(point));

    if(m_currentTreeItem == NULL)
        return;

    int type = m_currentTreeItem->itemType();

    switch (type) {
    case lROOT:
        showRootMenu(point);
        break;
    case lDEVICE:
        showDeviceMenu(point);
        break;
    case lCHANNEL:

        break;
    default:
        break;
    }

}

void TestForm::showAddDialog(bool)
{
    DeviceDialog *dialog = new DeviceDialog(NULL, ADD, getCheckList(), "add device", this);
    connect(dialog, SIGNAL(addDevice(DeviceData*)), this, SLOT(addDevice(DeviceData*)));
    dialog->exec();
}

void TestForm::showAltDialog(bool)
{
    if(m_deviceList[m_currentTreeItem->getMapId()].isPlaying())
    {
        QMessageBox::information(this, "infomation", "please close device first");
        return;
    }

    DeviceDialog *dialog = new DeviceDialog(&m_deviceList[m_currentTreeItem->getMapId()], ALT, getCheckList(), "add device", this);
    connect(dialog, SIGNAL(altDevice(DeviceData*)), this, SLOT(altDevice(DeviceData*)));
    dialog->exec();
}

void TestForm::deleteDevice(bool)
{
    if(QMessageBox::information(this, "infomation", "confirm to remove?", QMessageBox::Cancel, QMessageBox::Ok) == QMessageBox::Ok)
    {
        if(m_deviceList[m_currentTreeItem->getMapId()].isPlaying())
        {
            QMessageBox::information(this, "infomation", "please close device first");
            return;
        }

        m_deviceList.remove(m_currentTreeItem->getMapId());
        m_leftTreeModel->removeRow(m_currentTreeItem->row(), m_currentTreeItem->parent()->index());
        resetDeviceTreeXml(m_deviceList, XML_PATH);
    }
}

void TestForm::altDevice(DeviceData *ddata)
{
    //check new config
    if(!testLogin(ddata->getMapId()))
        QMessageBox::warning(this, "error", "Login device:"+ddata->getDeviceName()+" failed");

    //remove old channel items and create new channel items
    m_leftTreeModel->removeRows(0, m_currentTreeItem->rowCount(), m_currentTreeItem->index());
    m_currentTreeItem->setText(ddata->getDeviceName());

    QList<ChannelData> &clist = m_deviceList[ddata->getMapId()].getChannelData();
    for(int i = 0; i<clist.size(); i++)
    {
        ChannelData *cdata = &clist[i];
        MyLeftTreeItem *channelItem = new MyLeftTreeItem(cdata->getChannelName(), "", lCHANNEL, cdata);
        m_currentTreeItem->appendRow(channelItem);
    }

    resetDeviceTreeXml(m_deviceList, XML_PATH);
}

void TestForm::addDevice(DeviceData *newdd)
{
    newdd->setMapId(QUuid::createUuid().toString());
    m_deviceList.insert(newdd->getMapId(), *newdd);

    DeviceData *ddata = &m_deviceList[newdd->getMapId()];

    if(!testLogin(ddata->getMapId()))
        QMessageBox::warning(this, "error", "Login device:"+ddata->getDeviceName()+" failed");

    //create device item and channel items
    MyLeftTreeItem *deviceItem = new MyLeftTreeItem(ddata->getDeviceName(), ddata->getMapId(), lDEVICE, ddata);

    QList<ChannelData> &clist = m_deviceList[ddata->getMapId()].getChannelData();
    for(int i = 0; i<clist.size(); i++)
    {
        ChannelData *cdata = &clist[i];
        MyLeftTreeItem *channelItem = new MyLeftTreeItem(cdata->getChannelName(), "", lCHANNEL, cdata);
        deviceItem->appendRow(channelItem);
    }

    m_currentTreeItem->appendRow(deviceItem);

    resetDeviceTreeXml(m_deviceList, XML_PATH);
    delete newdd;
}

void TestForm::onLeftTreeDoubleClicked(const QModelIndex& index)
{
    ui->leftTreeView->blockSignals(true);
    //locate current clicked item
    MyLeftTreeItem *item = (MyLeftTreeItem*)m_leftTreeModel->itemFromIndex(index);

    //start realplay or stop realplay
    if(item->itemType() == lCHANNEL)
    {
        ChannelData *cdata = (ChannelData*)item->getBindData();
        if(cdata->isPlaying == true)
        {
            stopRealPlay(cdata, item);
        }
        else
        {
            //try to get a free frame to display
            DisplayFrame *frame = getFreeFrame();
            if(frame)
            {
                frame->setIsPlaying(true);
                cdata->frame = frame;
                cdata->isPlaying = true;
                startRealPlay(cdata, item);
            }
            else
            {
                QMessageBox::information(this, "info", "please close one cameral first");
            }
        }
    }
    ui->leftTreeView->blockSignals(false);
}

void TestForm::onRightTreeDoubleClicked(const QModelIndex& index)
{
    QStandardItemModel* model = (QStandardItemModel *)index.model();
    //locate current clicked item
    MyRightTreeItem *item = (MyRightTreeItem*)model->itemFromIndex(index);

    if(item->itemType() == rGIF1 || item->itemType() == rGIF2 || item->itemType() == rGIF3)
    {
        GifDialog *d = new GifDialog(item->parent()->parent()->index().data().toString(), item->index().data().toString(), item->bindData(), this, item);
        d->show();
    }
}

void TestForm::startRealPlay(ChannelData *cdata, QStandardItem *item)
{
    qDebug() << "------------------RealPlay--------------------";

    int userID = cdata->getParentDevice()->getUsrID();

    int channelNum = cdata->getChannelNum();
    //computed by stream type and protocl type
    int linkMode = cdata->getLinkMode();


    //--------------------------------------------------------------

    //设置预览打开的相关参数
    NET_DVR_CLIENTINFO *clientinfo = new NET_DVR_CLIENTINFO;

    clientinfo->hPlayWnd = NULL;
    clientinfo->lChannel = channelNum;
    clientinfo->lLinkMode = linkMode;

    char tmp[128] = {0};
    sprintf(tmp, "%s", cdata->getParentDevice()->getMultiCast().toLatin1().data());
    clientinfo->sMultiCastIP = tmp;

    //取流
    int realHandle = NET_DVR_RealPlay_V30(userID, clientinfo, DataCallBack, cdata,0);

    qDebug("realPlay---Protocal:%d", clientinfo->lLinkMode);

    if (realHandle < 0)
    {
        cdata->frame->setIsPlaying(false);
        cdata->frame = NULL;
        cdata->isPlaying = false;
        QMessageBox::information(this, tr("NET_DVR_RealPlay error"), tr("SDK_LASTERROR=%1").arg(NET_DVR_GetLastError()));
    }
    else
    {
        if (cdata->getChannelNum()>32)
        {
            NET_DVR_IPPARACFG ipcfg;
            DWORD Bytesreturned;
            if (!NET_DVR_GetDVRConfig(userID, NET_DVR_GET_IPPARACFG,0,
                                      &ipcfg, sizeof(NET_DVR_IPPARACFG),&Bytesreturned))
            {
                cdata->frame->setIsPlaying(false);
                cdata->frame = NULL;
                cdata->isPlaying = false;
                QMessageBox::information(this, tr("NET_DVR_GetDVRConfig"), \
                                         tr("SDK_LAST_ERROR=%1").arg(NET_DVR_GetLastError()));

                return ;
            }
            if (ipcfg.struIPChanInfo[cdata->getChannelNum()-32-1].byEnable == 0)
            {
                cdata->frame->setIsPlaying(false);
                cdata->frame = NULL;
                cdata->isPlaying = false;
                QMessageBox::information(this,tr("NET_DVR_RealPlay error"), \
                                         tr("该通道不在线，预览失败"));

                return ;
            }
        }

        cdata->setRealhandle(realHandle);
        cdata->frame->setBlackbg(false);
        item->setIcon(QIcon(CAMERAL_PLAYING_ICON));
        //1111111111111111111
        cdata->startLS();
        //1111111111111111111
    }
}

void TestForm::stopRealPlay(ChannelData *cdata, QStandardItem *item)
{
    cdata->stopLS();

    NET_DVR_StopRealPlay(cdata->getRealhandle());
    PlayM4_FreePort(cdata->getDecodePort());
    item->setIcon(QIcon(CAMERAL_ICON));

    DisplayFrame *frame = cdata->frame;

    if(!frame)
    {
        qDebug() << "error pframe NULL";
        return;
    }

    frame->reset();

    if(!noMoreFramePlay())
    {
        ui->gridLayout->removeWidget(frame);
        frame->hide();
    }

    sortFrames();

    cdata->reset();
}

DisplayFrame *TestForm::getFreeFrame()
{
    //try to get a free and not hidden frame
    foreach (DisplayFrame* frame, m_displayFrames) {
        if(!frame->getIsPlaying()&&!frame->isHidden())
        {
            return frame;
        }
    }

    //try to get a free and hidden frame
    foreach (DisplayFrame* frame, m_displayFrames) {
        if(!frame->getIsPlaying())
        {
            add2GridLayout(frame);
            frame->show();
            return frame;
        }
    }

    //try to create a new frame
    if(m_displayFrames.size() < MAX_DISPLAY_FRAME)
    {
        DisplayFrame *frame = new DisplayFrame(this, m_displayFrames.size());
        m_displayFrames.append(frame);
        add2GridLayout(frame);
        frame->show();
        return frame;
    }

    return NULL;
}

bool TestForm::noMoreFramePlay()
{
    foreach (DisplayFrame *frame, m_displayFrames) {
        if(frame->getIsPlaying())
        {
            return false;
        }
    }

    return true;
}

QStandardItem *TestForm::findDateItem(QString date)
{
    //find date in today list
    for(int i = 0; i < m_rightTreeModel_2->item(0)->rowCount(); i++)
    {
        if(m_rightTreeModel_2->item(0)->child(i)->text() == date)
        {
            return m_rightTreeModel_2->item(0)->child(i);
        }
    }

    //find date in history list
    for(int i = 0; i < m_rightTreeModel_1->item(0)->rowCount(); i++)
    {
        if(m_rightTreeModel_1->item(0)->child(i)->text() == date)
        {
            return m_rightTreeModel_1->item(0)->child(i);
        }
    }

    return NULL;
}

QStandardItem *TestForm::findDeviceItem(QString deviceSerial, QStandardItem *dateItem)
{
    for(int i = 0; i < dateItem->rowCount(); i++)
    {
        MyRightTreeItem* deviceItem = (MyRightTreeItem*)dateItem->child(i);

        if(deviceItem->bindData() == deviceSerial)
        {
            return deviceItem;
        }
    }

    return NULL;
}

QStandardItem *TestForm::findChannelItem(QString channel, QStandardItem *deviceItem)
{
    for(int i = 0; i < deviceItem->rowCount(); i++)
    {
        QStandardItem* channelItem = deviceItem->child(i);

        if(channelItem->text() == channel)
        {
            return channelItem;
        }
    }

    return NULL;
}

void TestForm::sortFrames()
{
    QVector<QWidget *> widgets;
    for(int i = 0; i < FRAME_ROW_COUNT; i++)
    {
        for(int j = 0; j < FRAME_COL_COUNT; j++)
        {
            QLayoutItem* w = ui->gridLayout->itemAtPosition(i, j);
            if(w != NULL)
            {
                widgets.append(w->widget());
                ui->gridLayout->removeWidget(w->widget());
            }
        }
    }

    for(int i = 0; i < widgets.size(); i++)
    {
        int row = i/FRAME_COL_COUNT;
        int col = i%FRAME_COL_COUNT;

        ui->gridLayout->addWidget(widgets.at(i), row, col);
    }
}

void TestForm::on_pbcleft_clicked()
{
    if(leftTreeExpand)
    {
        ui->leftTreeView->hide();
        leftTreeExpand = !leftTreeExpand;

        ui->pbcleft->setIcon(QIcon(RIGHT_ICON));
    }
    else
    {
        ui->leftTreeView->show();
        leftTreeExpand = !leftTreeExpand;

        ui->pbcleft->setIcon(QIcon(LEFT_ICON));
    }
}

void TestForm::on_pbcright_clicked()
{
    if(rightTreeExpand)
    {
        ui->frame->hide();
        rightTreeExpand = !rightTreeExpand;

        ui->pbcright->setIcon(QIcon(LEFT_ICON));
    }
    else
    {
        ui->frame->show();
        rightTreeExpand = !rightTreeExpand;

        ui->pbcright->setIcon(QIcon(RIGHT_ICON));
    }
}

void TestForm::expandTreeClicked(const QModelIndex &)
{
    QTreeView *view = (QTreeView*)sender();
    view->resizeColumnToContents(0);
}

void TestForm::addRow(QStringList filePaths)
{
    QString date, deviceSerial, channel;
    QStandardItem *dateItem = NULL;
    QStandardItem *deviceItem = NULL;
    QStandardItem *channelItem = NULL;

    foreach (QString filePath, filePaths) {
        QString fileName = filePath.mid(filePath.lastIndexOf("/") + 1).replace("\.gif", "");
        QString path = filePath.mid(0, filePath.lastIndexOf("/"));

        channel = path.mid(path.lastIndexOf("/") + 1);

        path = path.mid(0, path.lastIndexOf("/"));

        deviceSerial = path.mid(path.lastIndexOf("/") + 1);

        path = path.mid(0, path.lastIndexOf("/"));

        date = path.mid(path.lastIndexOf("/") + 1);
        //if not find target channel
        if(channelItem == NULL)
        {
            dateItem = findDateItem(date);
            if(dateItem == NULL)
            {
                dateItem = new MyRightTreeItem(date, rDATE);
                if (date == QDate::currentDate().toString("yyyy-MM-dd"))
                    m_rightTreeModel_2->item(0)->appendRow(dateItem);
                else
                    m_rightTreeModel_1->item(0)->appendRow(dateItem);
            }

            deviceItem = findDeviceItem(deviceSerial, dateItem);
            if(deviceItem == NULL)
            {
                deviceItem = new MyRightTreeItem(m_nameMap[deviceSerial], rDEVICE, deviceSerial);
                dateItem->appendRow(deviceItem);
            }

            channelItem = findChannelItem(channel, deviceItem);
            if(channelItem == NULL)
            {
                channelItem = new MyRightTreeItem(channel, rCHANNEL);
                deviceItem->appendRow(channelItem);
            }
        }

        QString label = fileName.split("_").at(1);
        QDateTime dt = QDateTime::fromTime_t(fileName.split("_").at(2).toInt());

        MyRightTreeItem *gifItem = NULL;
        if(label == "1")
            gifItem = new MyRightTreeItem(dt.toString("hh:mm:ss"), rGIF1, filePath);
        else if(label == "2")
            gifItem = new MyRightTreeItem(dt.toString("hh:mm:ss"), rGIF2, filePath);

        if(gifItem != NULL)
            channelItem->appendRow(gifItem);

    }

    //make sure dateItem inside  the right tree
    for(int i = 0; i < m_rightTreeModel_2->item(0)->rowCount(); i++)
    {
        if(m_rightTreeModel_2->item(0)->child(i)->text() != QDate::currentDate().toString("yyyy-MM-dd"))
        {
            QStandardItem *item = m_rightTreeModel_2->item(0)->takeRow(i).at(0);
            m_rightTreeModel_1->item(0)->appendRow(item);
        }
    }

    //make sure today tree has date Item
    if(m_rightTreeModel_2->item(0)->rowCount() == 0)
    {
        QStandardItem *item = new MyRightTreeItem(QDate::currentDate().toString("yyyy-MM-dd"), rDATE);
        m_rightTreeModel_2->item(0)->appendRow(item);
    }
}
