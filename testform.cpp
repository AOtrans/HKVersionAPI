#include "testform.h"
#include "ui_testform.h"
#include <QDebug>
#include <QImage>
#include <stdio.h>
#include <unistd.h>
#include <QMenu>
#include <QUuid>

TestForm::TestForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TestForm)
{
    ui->setupUi(this);

    if(sdkInit(this))
    {
        if(!analysis(m_deviceList, xmlPath))
            qDebug() << "analysis failed";

        loginAllDevice();
        initTree();

        m_filter = new PainterEvent(this);
        ui->frame_1->installEventFilter(m_filter);
    }
}

TestForm::~TestForm()
{
    delete ui;

    //NET_DVR_LOGOUT(id);

    NET_DVR_Cleanup();
}

void TestForm::showVideo(cv::Mat img, ChannelData *cdata)
{
    qDebug() << "show";
    m_filter->changeMat(img);
    ui->frame_1->update();
}
//todo login check
void TestForm::initTree()
{
    if(m_treeModel == NULL)
        m_treeModel = new QStandardItemModel(this);

    MyTreeItem *rootItem = new MyTreeItem("Devices", "", ROOT, NULL);


    foreach(QString mapId, m_deviceList.keys())
    {
        DeviceData *ddata = &m_deviceList[mapId];
        MyTreeItem *deviceItem = new MyTreeItem(ddata->getDeviceName(), ddata->getMapId(), DEVICE, ddata);

        QList<ChannelData> &clist = ddata->getChannelData();
        for(int i = 0; i<clist.size(); i++)
        {
            ChannelData *cdata = &clist[i];
            MyTreeItem *channelItem = new MyTreeItem(cdata->getChannelName(), "", CHANNEL, cdata);
            deviceItem->appendRow(channelItem);
        }

        rootItem->appendRow(deviceItem);
    }

    m_treeModel->appendRow(rootItem);
    m_treeModel->setHeaderData(0, Qt::Horizontal, "DevicesTree");

    QHeaderView *view = new QHeaderView(Qt::Horizontal, this);
    view->setSectionResizeMode(QHeaderView::Stretch);

    ui->leftTreeView->setHeader(view);

    ui->leftTreeView->setModel(m_treeModel);


    ui->leftTreeView->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->leftTreeView->expandAll();
    connect(ui->leftTreeView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onTreeClicked(QPoint)));
    connect(ui->leftTreeView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(onTreeDoubleClicked(QModelIndex)));
}

void TestForm::showRootMenu(QPoint &point)
{
    QPoint showP = this->mapToGlobal(point);
    showP.setY(showP.y() + 30);

    QMenu *menu = new QMenu(this);
    QAction *acAdd = new QAction("add Device", menu);

    connect(acAdd, SIGNAL(triggered(bool)), this, SLOT(showAddDialog(bool)));

    menu->addAction(acAdd);
    menu->exec(showP);
}

void TestForm::showDeviceMenu(QPoint &point)
{
    QPoint showP = this->mapToGlobal(point);
    showP.setY(showP.y() + 30);

    QMenu *menu = new QMenu(this);
    QAction *acDelete = new QAction("delete Device", menu);
    connect(acDelete, SIGNAL(triggered(bool)), this, SLOT(deleteDevice(bool)));

    QAction *acAlt = new QAction("alt Device", menu);
    connect(acAlt, SIGNAL(triggered(bool)), this, SLOT(showAltDialog(bool)));

    menu->addAction(acAlt);
    menu->addAction(acDelete);
    menu->exec(showP);
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
                //Camera
                qDebug() << "(this is Cameral)" << "getIPPARACFG erro:" << NET_DVR_GetLastError();
                for (int i=devicecfg.byStartChan;i<=devicecfg.byChanNum ;i++)
                {
                    ChannelData *newChannel = new ChannelData;
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
            else //DVR
            {
                qDebug() << "(this is DVR)";

                //扫描添加模拟通道
                for (int i=0;i< MAX_ANALOG_CHANNUM;i++)
                {
                    if (1== ipcfg.byAnalogChanEnable[i])
                    {
                        ChannelData *newChannel = new ChannelData;
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

                        //todo添加进设备节点
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
                        ChannelData *newChannel = new ChannelData;
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

                        //todo添加进设备节点
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

void TestForm::loginAllDevice()
{
    foreach(QString mapId, m_deviceList.keys())
    {
        DeviceData &ddata = m_deviceList[mapId];
        if(!testLogin(mapId))
            QMessageBox::warning(this, "error", "Login device:"+ddata.getDeviceName()+" failed");
    }
}

void TestForm::on_pbRealPaly_clicked()
{
    qDebug() << "------------------RealPlay--------------------";

    if(currentDevice == NULL)
    {
        qDebug() << "please login first";
        return;
    }

    int userID = currentDevice->getUsrID();
    ChannelData currentChannel = currentDevice->m_qlistchanneldata.at(0);

    int channelNum = currentChannel.getChannelNum();
    //computed by stream type and protocl type
    int linkMode = currentChannel.getLinkMode();


    //-----------------------------------------

    //设置预览打开的相关参数
    NET_DVR_CLIENTINFO *clientinfo = new NET_DVR_CLIENTINFO;

    clientinfo->hPlayWnd = NULL;
    clientinfo->lChannel = channelNum;
    clientinfo->lLinkMode = linkMode;

    char tmp[128] = {0};
    sprintf(tmp, "%s", currentDevice->getMultiCast().toLatin1().data());
    clientinfo->sMultiCastIP = tmp;

    //取流
    int realHandle = NET_DVR_RealPlay_V30(userID, clientinfo, DataCallBack,NULL,0);
    qDebug("realPlay---Protocal:%d", clientinfo->lLinkMode);

    if (realHandle < 0)
    {
        QMessageBox::information(this, tr("NET_DVR_RealPlay error"), tr("SDK_LASTERROR=%1").arg(NET_DVR_GetLastError()));
    }
    else
    {
        if (currentChannel.getChannelNum()>32)
        {
            NET_DVR_IPPARACFG ipcfg;
            DWORD Bytesreturned;
            if (!NET_DVR_GetDVRConfig(userID, NET_DVR_GET_IPPARACFG,0,
                                      &ipcfg, sizeof(NET_DVR_IPPARACFG),&Bytesreturned))
            {
                QMessageBox::information(this, tr("NET_DVR_GetDVRConfig"), \
                                         tr("SDK_LAST_ERROR=%1").arg(NET_DVR_GetLastError()));

                return ;
            }
            if (ipcfg.struIPChanInfo[currentChannel.getChannelNum()-32-1].byEnable == 0)
            {
                QMessageBox::information(this,tr("NET_DVR_RealPlay error"), \
                                         tr("该通道不在线，预览失败"));

                return ;
            }
        }

        //设置设备是否在预览状态标签值
        currentDevice->setRealPlayLabel(1);
    }
}



//unknow problem ,cause too slow
void TestForm::on_pbPalyBack_clicked()
{
    int userID = currentDevice->getUsrID();
    ChannelData currentChannel = currentDevice->m_qlistchanneldata.at(0);

    int channelNum = currentChannel.getChannelNum();

    NET_DVR_TIME remoteplaystarttime;
    NET_DVR_TIME remoteplaystoptime;

    //get the begin and end date time
    remoteplaystarttime.dwYear = 2018;
    remoteplaystarttime.dwMonth = 7;
    remoteplaystarttime.dwDay = 12;
    remoteplaystarttime.dwHour = 0;
    remoteplaystarttime.dwMinute = 0;
    remoteplaystarttime.dwSecond = 0;

    remoteplaystoptime.dwYear = 2018;
    remoteplaystoptime.dwMonth = 7;
    remoteplaystoptime.dwDay = 12;
    remoteplaystoptime.dwHour = 23;
    remoteplaystoptime.dwMinute = 59;
    remoteplaystoptime.dwSecond = 59;

    //----------------------------------------------------------
    int playBackHandle = NET_DVR_PlayBackByTime(userID, channelNum, &remoteplaystarttime, &remoteplaystoptime, NULL);




    NET_DVR_SetPlayDataCallBack_V40(playBackHandle, DataCallBack, NULL);

    if (!NET_DVR_PlayBackControl(playBackHandle, NET_DVR_PLAYSTART, 0, NULL))
    {
        qDebug() << "NET_DVR_PlayBackControl failed" << tr("SDK_Last_Error =%1").arg(NET_DVR_GetLastError());
        return;
    }
}

//check file exists from start to end
void TestForm::on_pbCheckPlayBack_clicked()
{

    ChannelData currentChannel = currentDevice->m_qlistchanneldata.at(1);

    int channelNum = currentChannel.getChannelNum();

    uint filetype = 0xff;


    uint fileattr = 0xff;


    NET_DVR_TIME remotestarttime;
    NET_DVR_TIME remotestoptime;

    //get the begin and end date time
    remotestarttime.dwYear = 2018;
    remotestarttime.dwMonth = 7;
    remotestarttime.dwDay = 1;
    remotestarttime.dwHour = 0;
    remotestarttime.dwMinute = 0;
    remotestarttime.dwSecond = 0;

    remotestoptime.dwYear = 2018;
    remotestoptime.dwMonth = 7;
    remotestoptime.dwDay = 12;
    remotestoptime.dwHour = 23;
    remotestoptime.dwMinute = 59;
    remotestoptime.dwSecond = 59;

    //---------------------------------------------------------------
    bool bGetFile = true;
    int ret =0;
    int hFile =0;

    //call the hcnetsdk interface
    NET_DVR_FILECOND *m_pbpDvrFile = new NET_DVR_FILECOND;

    //give the variant prepare value
    m_pbpDvrFile->struStartTime = remotestarttime;
    m_pbpDvrFile->struStopTime = remotestoptime;
    m_pbpDvrFile->lChannel = channelNum;

    m_pbpDvrFile->dwIsLocked = fileattr;
    m_pbpDvrFile->dwFileType = filetype;

    //Search fot file
    hFile = NET_DVR_FindFile_V30(currentDevice->getUsrID(), m_pbpDvrFile);
    if (hFile < 0)
    {
        QMessageBox::information(this,tr(" NET_DVR_FindFile_V30 "),
            tr("SDK_LASTERROR=%1").arg(NET_DVR_GetLastError()));


        return;
    }
    else
    {

        NET_DVR_FINDDATA_V30 *fileData = new NET_DVR_FINDDATA_V30;
        while (bGetFile)
        {
            ret = NET_DVR_FindNextFile_V30(hFile, fileData);
            switch (ret)
            {
                case NET_DVR_FILE_SUCCESS:
                    qDebug() << "find file" << fileData->sFileName;
                    break;

                case NET_DVR_FILE_NOFIND:
                    qDebug() << "no file find";
                    NET_DVR_FindClose_V30(hFile);
                    bGetFile = FALSE;
                    break;

                case NET_DVR_ISFINDING:
                    QMessageBox::information(this,tr(" NET_DVR_FindFile_V30"),
                        tr("Is finding now,wait a moment!"));

                    usleep(200*1000);

                    break;

                case NET_DVR_NOMOREFILE:
                    qDebug() << "no more file";
                    NET_DVR_FindClose_V30(hFile);
                    bGetFile = FALSE;
                    break;

                case NET_DVR_FILE_EXCEPTION:
                    qDebug() << "File Exception!";
                    NET_DVR_FindClose_V30(hFile);
                    bGetFile = FALSE;
                    break;

                default:
                    qDebug() << "Error occured";
                    NET_DVR_FindClose_V30(hFile);
                    bGetFile = FALSE;
                    break;
            }
        }
        return ;
    }
}

//test if can do capture(about 300ms, too slow)
void TestForm::on_pbCapture_clicked()
{
    int userID = currentDevice->getUsrID();
//    ChannelData currentChannel = currentDevice->m_qlistchanneldata.at(0);

//    int channelNum = currentChannel.getChannelNum();

//    NET_DVR_JPEGPARA para;
//    para.wPicQuality = 0;
//    para.wPicSize = 5;

//    qDebug() << NET_DVR_CaptureJPEGPicture(userID, channelNum, &para, "./1.jpg");

    char in[200]="<?xml version=\"1.0\" encoding=\"utf-8\"?> \
            <JpegCaptureAbility version=\"2.0\"> \
                    <channelNO>1</channelNO>\
            </JpegCaptureAbility> ";
    char out[2000];
    qDebug()<<in;
    qDebug() << NET_DVR_GetDeviceAbility(userID,DEVICE_JPEG_CAP_ABILITY,in,200,out,2000);
    qDebug()<<out;

    qDebug() << NET_DVR_GetLastError();
}

void TestForm::onTreeClicked(QPoint point)
{
    currentTreeItem = (MyTreeItem*)m_treeModel->itemFromIndex(ui->leftTreeView->indexAt(point));

    if(currentTreeItem == NULL)
        return;

    int type = currentTreeItem->itemType();

    switch (type) {
    case ROOT:
        showRootMenu(point);
        break;
    case DEVICE:
        showDeviceMenu(point);
        break;
    case CHANNEL:

        break;
    default:
        break;
    }

}

void TestForm::showAddDialog(bool)
{
    DeviceDialog *dialog = new DeviceDialog(currentTreeItem->getMapId(), m_deviceList.value(currentTreeItem->getMapId()), ADD, this);
    connect(dialog, SIGNAL(addDevice(DeviceData*)), this, SLOT(addDevice(DeviceData*)));
    dialog->exec();
}

void TestForm::showAltDialog(bool)
{
    DeviceDialog *dialog = new DeviceDialog(currentTreeItem->getMapId(), m_deviceList.value(currentTreeItem->getMapId()), ALT, this);
    connect(dialog, SIGNAL(altDevice(DeviceData*)), this, SLOT(altDevice(DeviceData*)));
    dialog->exec();
}

void TestForm::deleteDevice(bool)
{
    if(QMessageBox::information(this, "infomation", "confirm to remove?", QMessageBox::Cancel, QMessageBox::Ok) == QMessageBox::Ok)
    {
        m_deviceList.remove(currentTreeItem->getMapId());
        m_treeModel->removeRow(currentTreeItem->row(), currentTreeItem->parent()->index());
        resetDeviceTreeXml(m_deviceList, xmlPath);
    }
}

void TestForm::altDevice(DeviceData *newdd)
{
    m_deviceList[newdd->getMapId()] = *newdd;
    DeviceData *ddata = &m_deviceList[newdd->getMapId()];

    if(!testLogin(ddata->getMapId()))
        QMessageBox::warning(this, "error", "Login device:"+ddata->getDeviceName()+" failed");

    m_treeModel->removeRows(0, currentTreeItem->rowCount(), currentTreeItem->index());
    currentTreeItem->setText(ddata->getDeviceName());

    QList<ChannelData> &clist = m_deviceList[ddata->getMapId()].getChannelData();
    for(int i = 0; i<clist.size(); i++)
    {
        ChannelData *cdata = &clist[i];
        MyTreeItem *channelItem = new MyTreeItem(cdata->getChannelName(), cdata->getSerial(), CHANNEL, cdata);
        currentTreeItem->appendRow(channelItem);
    }

    resetDeviceTreeXml(m_deviceList, xmlPath);
    delete newdd;
}

void TestForm::addDevice(DeviceData *newdd)
{
    newdd->setMapId(QUuid::createUuid().toString());
    m_deviceList.insert(newdd->getMapId(), *newdd);

    DeviceData *ddata = &m_deviceList[newdd->getMapId()];

    if(!testLogin(ddata->getMapId()))
        QMessageBox::warning(this, "error", "Login device:"+ddata->getDeviceName()+" failed");

    MyTreeItem *deviceItem = new MyTreeItem(ddata->getDeviceName(), ddata->getMapId(), DEVICE, ddata);

    QList<ChannelData> &clist = m_deviceList[ddata->getMapId()].getChannelData();
    for(int i = 0; i<clist.size(); i++)
    {
        ChannelData *cdata = &clist[i];
        MyTreeItem *channelItem = new MyTreeItem(cdata->getChannelName(), cdata->getSerial(), CHANNEL, cdata);
        deviceItem->appendRow(channelItem);
    }

    currentTreeItem->appendRow(deviceItem);

    resetDeviceTreeXml(m_deviceList, xmlPath);
    delete newdd;
}

void TestForm::onTreeDoubleClicked(QModelIndex index)
{
    MyTreeItem *item = (MyTreeItem*)m_treeModel->itemFromIndex(index);
    if(item->itemType() == CHANNEL)
    {
        ChannelData *cdata = (ChannelData*)item->getBindData();
        if(cdata->isPlaying == true)
        {
            stopRealPlay(cdata);
        }
        else
        {
            foreach(QString mapId, m_deviceList.keys())
            {
                DeviceData *ddata = &m_deviceList[mapId];

                QList<ChannelData> &clist = ddata->getChannelData();
                for(int i = 0; i<clist.size(); i++)
                {
                    ChannelData *cdata = &clist[i];
                    if(cdata->isPlaying == true)
                    {
                        stopRealPlay(cdata);
                    }
                }
            }

            startRealPlay(cdata);
        }
    }
}

void TestForm::startRealPlay(ChannelData *cdata)
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
                QMessageBox::information(this, tr("NET_DVR_GetDVRConfig"), \
                                         tr("SDK_LAST_ERROR=%1").arg(NET_DVR_GetLastError()));

                return ;
            }
            if (ipcfg.struIPChanInfo[cdata->getChannelNum()-32-1].byEnable == 0)
            {
                QMessageBox::information(this,tr("NET_DVR_RealPlay error"), \
                                         tr("该通道不在线，预览失败"));

                return ;
            }
        }

        cdata->setRealhandle(realHandle);
        //设置设备是否在预览状态标签值
        cdata->isPlaying = true;
        m_filter->setBlackbg(false);
    }
}

void TestForm::stopRealPlay(ChannelData *cdata)
{
    NET_DVR_StopRealPlay(cdata->getRealhandle());

    PlayM4_FreePort(cdata->getDecodePort());
    cdata->setDecodePort(-1);
    cdata->setRealhandle(-1);

    cdata->isPlaying = false;

    m_filter->setBlackbg(true);
    ui->frame_1->update();
}

bool PainterEvent::getBlackbg() const
{
    return blackbg;
}

void PainterEvent::setBlackbg(bool value)
{
    blackbg = value;
}

bool PainterEvent::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::Paint) {
        if( blackbg == false && frameMat.data != NULL)
        {
            QFrame *frame = (QFrame*)obj;
            QPainter painter(frame);

            painter.drawPixmap(0, 0, frame->width(), frame->height(), QPixmap::fromImage(cvMat2Image(frameMat)) );
        }
        else
        {
            QFrame *frame = (QFrame*)obj;
            QPainter painter(frame);

            QPixmap bg(frame->width(), frame->height());
            bg.fill(QColor(0, 0, 0));
            painter.drawPixmap(0, 0, frame->width(), frame->height(), bg);
        }
    }

    // standard event processing
    return QObject::eventFilter(obj, event);
}
