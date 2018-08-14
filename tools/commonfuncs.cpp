#include "commonfuncs.h"
extern TestForm* form;

QString decrypt(QString encryptStr)
{
    QString key = "";
    return encryptStr;
}

QString encrypt(QString srcStr)
{
    QString key = "";
    return srcStr;
}

bool resetDeviceTreeXml(QMap<QString, DeviceData> &deviceList, QString xmlFilePath)
{
    qDebug() << "start resetDeviceTreeXML:" << xmlFilePath;
    QFile file(xmlFilePath);

    if(file.exists())
    {
        if(!file.remove())
        {
            qDebug() << "reset xml failed";
            return false;
        }
    }

    if(!file.open(QIODevice::ReadWrite | QFile::Text))
    {
        qDebug() << "open file failed";
        return false;
    }

    QXmlStreamWriter writer(&file);
    writer.writeStartDocument("1.0");

    writer.writeStartElement("DeviceTree");

    foreach (DeviceData data, deviceList.values()) {
        writer.writeStartElement("Device");

        writer.writeAttribute("name", data.getDeviceName() );
        writer.writeAttribute("serial", data.getSerialNum() );
        writer.writeAttribute("ip", data.getIP() );
        writer.writeAttribute("port", QString::number(data.getPort() ) );
        writer.writeAttribute("user", data.getUsrName() );
        writer.writeAttribute("pwd", encrypt(data.getPasswd() ) );

        foreach (ChannelData data, data.getChannelData()) {
            writer.writeStartElement("channel");

            writer.writeAttribute("name", data.getChannelName() );
            writer.writeAttribute("serial", data.getSerial() );
            writer.writeAttribute("channelnum", QString::number(data.getChannelNum() ) );
            writer.writeAttribute("protoc", QString::number((int)data.getProtocolType() ) );
            writer.writeAttribute("streamtype", QString::number((int)data.getStreamType() ) );

            writer.writeStartElement("saveDir");
            writer.writeAttribute("directory", data.getSaveDir() );
            writer.writeEndElement();

            writer.writeEndElement();
        }

        writer.writeEndElement();
    }

    writer.writeEndElement();
    writer.writeEndDocument();

    file.close();
    return true;
}

bool analysis(QMap<QString, DeviceData> &deviceList, QString xmlFilePath)
{
    qDebug() << "start analysis:" << xmlFilePath;
    QFile file(xmlFilePath);

    if(!file.open(QIODevice::ReadWrite | QFile::Text))
    {
        qDebug() << "open file failed";
        return false;
    }

    QXmlStreamReader reader(&file);

    reader.readNext();

    DeviceData* currentDevice = NULL;
    ChannelData* currentChannel = NULL;

    while(!reader.atEnd())
    {
        if(reader.isStartElement())
        {
            if(reader.name() == "Device")
            {
                if(currentDevice != NULL)
                    delete currentDevice;

                currentDevice = new DeviceData;

                QXmlStreamAttributes &&attributes =  reader.attributes();
                currentDevice->setDeviceName(attributes.value("name").toString() );
                currentDevice->setSerialNum(attributes.value("serial").toString() );
                currentDevice->setMapId(QUuid::createUuid().toString() );
                currentDevice->setIP(attributes.value("ip").toString() );
                currentDevice->setPort(attributes.value("port").toInt() );
                currentDevice->setUsrName(attributes.value("user").toString() );
                currentDevice->setPasswd(decrypt(attributes.value("pwd").toString() ) );

            }
            else if(reader.name() == "channel")
            {
//                if(currentChannel != NULL)
//                    delete currentChannel;

//                currentChannel = new ChannelData;

//                QXmlStreamAttributes &&attributes = reader.attributes();
//                currentChannel->setChannelName(attributes.value("name").toString() );
//                currentChannel->setSerial(currentDevice->getSerialNum() );
//                currentChannel->setChannelNum(attributes.value("channelnum").toInt() );
//                currentChannel->setProtocolType((PROTOCOL)attributes.value("protoc").toInt() );
//                currentChannel->setStreamType((STREAMTYPE)attributes.value("streamtype").toInt() );
            }
            else if(reader.name() == "saveDir")
            {
//                QXmlStreamAttributes &&attributes = reader.attributes();
//                currentChannel->setSaveDir(attributes.value("directory").toString() );

//                currentDevice->insertChannelDataCHild(*currentChannel);
            }

        }
        else if(reader.isEndElement())
        {
            if(reader.name() == "Device")
            {
                deviceList.insert(currentDevice->getMapId(), *currentDevice);
                //currentDevice->debug();
            }
        }

        reader.readNext();
    }

    if(currentChannel != NULL)
        delete currentChannel;
    if(currentDevice != NULL)
        delete currentDevice;

    file.close();
    return true;
}

//int FrameSave2JPG(AVFrame* pFrame, int width, int height, int id, void *dwUser)
//{
//    // 输出文件路径
//    char out_file[100] = { 0 };
//    CameralInfo* info = (CameralInfo*)dwUser;
//    sprintf(out_file, "%s%d/%d.jpg", "./", info->m_id, id);
//    // 分配AVFormatContext对象
//    AVFormatContext* pFormatCtx = avformat_alloc_context();

//    // 设置输出文件格式
//    pFormatCtx->oformat = av_guess_format("mjpeg", NULL, NULL);
//    // 创建并初始化一个和该url相关的AVIOContext
//    if (avio_open(&pFormatCtx->pb, out_file, AVIO_FLAG_READ_WRITE) < 0)
//    {
//        qDebug("Couldn't open output file.");
//        return -1;
//    }

//    // 构建一个新stream
//    AVStream* pAVStream = avformat_new_stream(pFormatCtx, 0);
//    if (pAVStream == NULL)
//    {
//        qDebug("Frame2JPG::avformat_new_stream error.");
//        return -1;
//    }

//    // 设置该stream的信息
//    AVCodecContext* pCodecCtx = pAVStream->codec;

//    pCodecCtx->codec_id = pFormatCtx->oformat->video_codec;
//    pCodecCtx->codec_type = AVMEDIA_TYPE_VIDEO;
//    pCodecCtx->pix_fmt = AV_PIX_FMT_YUVJ420P;
//    pCodecCtx->width = width;
//    pCodecCtx->height = height;
//    pCodecCtx->time_base.num = 1;
//    pCodecCtx->time_base.den = 25;

//    // Begin Output some information
//    // av_dump_format(pFormatCtx, 0, out_file, 1);
//    // End Output some information

//    // 查找解码器
//    AVCodec* pCodec = avcodec_find_encoder(pCodecCtx->codec_id);
//    if (!pCodec)
//    {
//        qDebug("找不到图片编码器.");
//        return -1;
//    }
//    // 设置pCodecCtx的解码器为pCodec
//    if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0)
//    {
//        qDebug("Could not open codec.");
//        return -1;
//    }

//    //Write Header
//    int ret = avformat_write_header(pFormatCtx, NULL);
//    if (ret < 0)
//    {
//        qDebug("Frame2JPG::avformat_write_header error.\n");
//        return -1;
//    }

//    int y_size = pCodecCtx->width * pCodecCtx->height;

//    //Encode
//    // 给AVPacket分配足够大的空间
//    AVPacket pkt;
//    av_new_packet(&pkt, y_size * 3);

//    int got_picture = 0;
//    ret = avcodec_encode_video2(pCodecCtx, &pkt, pFrame, &got_picture);
//    if (ret < 0)
//    {
//        qDebug("Encode Error.\n");
//        return -1;
//    }
//    if (got_picture == 1)
//    {
//        //pkt.stream_index = pAVStream->index;

//        ret = av_write_frame(pFormatCtx, &pkt);
//    }

//    av_free_packet(&pkt);

//    //Write Trailer
//    av_write_trailer(pFormatCtx);

//    if (pAVStream)
//    {
//        avcodec_close(pAVStream->codec);
//    }
//    avio_close(pFormatCtx->pb);
//    avformat_free_context(pFormatCtx);

//    return 0;
//}

cv::Mat buf2cvMat(uchar* pBuf, int width ,int height)
{
    cv::Mat mat(height + height/2, width, CV_8UC1, pBuf);
    cv::Mat dst(height, width, CV_8UC3);
    cv::cvtColor(mat, dst, CV_YUV2BGR_YV12);

    return dst;
}

bool matSave2JPG(cv::Mat &mat, int id, void *dwUser)
{
    ChannelData* data = (ChannelData*)dwUser;
    QString path = data->getSaveDir();

    qDebug() << "save to:" << path;

    return cv::imwrite(path.toStdString(), mat);
}


void showReal(cv::Mat &img, void *dwUser)
{
    ChannelData* data = (ChannelData*)dwUser;
    if(data->getChannelName() == "D1C1")
    {
        cv::imshow("1", img);
    }
    else
    {
        cv::imshow("2", img);
    }
}


//AVFrame* buf2Frame(uchar* pBuf, int width ,int height)
//{
//    AVFrame *pict = av_frame_alloc();
//    pict->width = width;
//    pict->height = height;
//    pict->format = AV_PIX_FMT_YUV420P;

//    int t_ret = av_frame_get_buffer(pict,32);//申请了AVFrame的内存
//    if(t_ret < 0)
//    {
//        qDebug()<<"robin:av_frame_get_buffer,Could not allocate the video frame data";
//        return NULL;
//    }
//    av_frame_make_writable(pict);


//    //av_image_copy_plane(t_pFrame->data[0],t_pFrame->linesize[0],(const uint8_t*)yData,yPerRowBytes,yPerRowBytes,frameHeight);
//    //av_image_copy_plane(t_pFrame->data[1],t_pFrame->linesize[1],(const uint8_t*)uData,uPerRowBytes,yPerRowBytes / 2,frameHeight);
//    //av_image_copy_plane(t_pFrame->data[2],t_pFrame->linesize[2],(const uint8_t*)vData,vPerRowBytes,yPerRowBytes / 2,frameHeight);
//    int size = width * height;

//    //I420/420p:yuv  yv12:yvu (switch u v)
//    pict->data[0] = (uchar*)pBuf;
//    pict->data[2] = pict->data[0] + size;
//    pict->data[1] = pict->data[2] + size / 4;

//    pict->linesize[0] = width;
//    pict->linesize[1] = width / 2;
//    pict->linesize[2] = width / 2;

//    return pict;
//}

//解码回调 视频为YUV数据(YV12)，音频为PCM数据
void CALLBACK DecCBFun(int lPort,char * pBuf,int nSize,FRAME_INFO * pFrameInfo, void* dwUser,int nReserved2)
{
    long lFrameType = pFrameInfo->nType;

    if (lFrameType ==T_AUDIO16)
    {

    }
    else if(lFrameType ==T_YV12)
    {
//        qDebug("Video nStamp:%d\n",pFrameInfo->nStamp);
//        qDebug("frame size:%d width:%d height:%d", nSize, pFrameInfo->nWidth, pFrameInfo->nHeight);

        int width = pFrameInfo->nWidth;
        int height = pFrameInfo->nHeight;

        cv::Mat img = buf2cvMat((uchar*)pBuf, width, height);

        //qDebug() << matSave2JPG(img, pFrameInfo->dwFrameNum, dwUser);
        ChannelData *cdata = (ChannelData*)dwUser;
        cdata->increaseImgNO();

        if(cdata->getImageNO() % IMAGE_STEP == 0)
            cdata->appendImage(img);

        form->showVideo(img, cdata);
//        showReal(img, dwUser);

//        AVFrame *pict = buf2Frame((uchar*)pBuf, width, height);
//        if(pict == NULL)
//        {
//            qDebug() << "alloc frame fail";
//            return;
//        }

//        FrameSave2JPG(pict, width, height, pFrameInfo->dwFrameNum, dwUser);

//        av_frame_free(&pict);
        //        if(lb == NULL)
        //            lb = new QLabel;
        //        lb->setPixmap(QPixmap::fromImage(image));
        //        lb->show();
        //        if (Videofile==NULL)
        //        {
        //            sprintf(filename,"VideoYV12.yuv",iPicNum);
        //            Videofile = fopen(filename,"wb");
        //        }
        //        fwrite(pBuf,nSize,1,Videofile);
    }
    else
    {

    }
}

void CALLBACK  DataCallBack(LONG lRealHandle,DWORD dwDataType,BYTE *pBuffer,DWORD  dwBufSize, void* dwUser)
{
    //where to use the storm data
    DWORD dRet = 0;

    LONG lPort = ((ChannelData*)dwUser)->getDecodePort();
    //qDebug("RealHandle[%d]: Get StreamData! Type[%d], BufSize[%d], pBuffer:%p\n", lRealHandle, dwDataType, dwBufSize, pBuffer);

    switch (dwDataType)
    {
    case NET_DVR_SYSHEAD: //系统头
        if (lPort >= 0)
        {
            break;  //该通道取流之前已经获取到句柄，后续接口不需要再调用
        }

        if (!PlayM4_GetPort(&lPort))  //获取播放库未使用的通道号
        {
            break;
        }

        ((ChannelData*)dwUser)->setDecodePort(lPort);

        if (dwBufSize > 0)
        {
            if (!PlayM4_SetStreamOpenMode(lPort, STREAME_REALTIME))  //设置实时流播放模式
            {
                break;
            }

            if (!PlayM4_OpenStream(lPort, pBuffer, dwBufSize, 1024*1024)) //打开流接口
            {
                break;
            }

            //设置解码回调函数 解码且显示
            if (!PlayM4_SetDecCallBackMend(lPort,DecCBFun,dwUser))
            {
                dRet=PlayM4_GetLastError(lPort);
                break;
            }

            if (!PlayM4_Play(lPort, NULL)) //播放开始
            {
                break;
            }
        }
        break;
    case NET_DVR_STREAMDATA:   //码流数据
        if (dwBufSize > 0 && lPort != -1)
        {
            if (!PlayM4_InputData(lPort, pBuffer, dwBufSize))
            {
                break;
            }
        }
        break;
    default: //其他数据
        if (dwBufSize > 0 && lPort != -1)
        {
            if (!PlayM4_InputData(lPort, pBuffer, dwBufSize))
            {
                break;
            }
        }
        break;
    }

}

void CALLBACK MessCallBack_V30(LONG lCommand, NET_DVR_ALARMER *pAlarmer, char *pAlarmInfo, DWORD dwBufLen, void* pUser)
{
    qDebug("MessCallBack_V30 len:%d, user data:%p", dwBufLen, pUser);

    if(lCommand < EXCEPTION_EXCHANGE)
    {
        qDebug() << "wrong lCommand code";
        return;
    }


    QString alarminformation;

    switch (lCommand)
    {
    case COMM_ALARM:

        switch (((LPNET_DVR_ALARMINFO)pAlarmInfo)->dwAlarmType)
        {
        case 0:
            alarminformation.append("信号量报警");
            break;
        case 1:
            alarminformation.append("硬盘满");
            break;
        case 2:
            alarminformation.append("信号丢失");
            break;
        case 3:
            alarminformation.append("移动侦测");
            break;
        case 4:
            alarminformation.append("硬盘格式化");
            break;

        case 5:
            alarminformation.append("读写硬盘出错");
            break;
        case 6:
            alarminformation.append("遮挡报警");
            break;
        case 7:
            alarminformation.append("制式不匹配");
            break;
        case 8:
            alarminformation.append("非法访问");
            break;
        default:
            alarminformation.append("未知告警");
            break;
        }
        qDebug() << alarminformation;

        break;
        //case COMM_ALARM_RULE:

        //break;

    case COMM_TRADEINFO:
        break;

    case COMM_ALARM_V30:

        switch (((LPNET_DVR_ALARMINFO_V30)pAlarmInfo)->dwAlarmType)
        {
        case 0:
            alarminformation.append("信号量报警");
            break;
        case 1:
            alarminformation.append("硬盘满");
            break;
        case 2:
            alarminformation.append("信号丢失");
            break;
        case 3:
            alarminformation.append("移动侦测");
            break;
        case 4:
            alarminformation.append("硬盘格式化");
            break;

        case 5:
            alarminformation.append("读写硬盘出错");
            break;
        case 6:
            alarminformation.append("遮挡报警");
            break;
        case 7:
            alarminformation.append("制式不匹配");
            break;
        case 8:
            alarminformation.append("非法访问");
            break;
        case 9:
            alarminformation.append("视频信号异常");
            break;
        case 10:
            alarminformation.append("录像异常");
            break;
        default:
            alarminformation.append("未知错误");
            break;
        }

        qDebug() << alarminformation;
        break;
    case COMM_IPCCFG :
        break;

        //case COMM_IPCCFG_V31:
        //break;
    default :
        break;
    }
}

void CALLBACK ExceptionCallBack(DWORD dwType, LONG lUserID, LONG lHandle, void *pUser)
{
    qDebug("ExceptionCallBack lUserID:%d, handle:%d, user data:%p", lUserID, lHandle, pUser);

    QWidget *parent = (QWidget*)pUser;

    switch(dwType)
    {
    case EXCEPTION_AUDIOEXCHANGE:		//Audio exchange exception
        QMessageBox::information(parent, "ERROR", "Audio exchange exception!");
        break;

        //Alarm//
    case EXCEPTION_ALARM:			            //Alarm exception
        QMessageBox::information(parent, "ERROR", "Alarm exception!");
        break;
    case EXCEPTION_ALARMRECONNECT:  //Alarm reconnect
        QMessageBox::information(parent, "ERROR", "Alarm connect exception!");
        break;
    case ALARM_RECONNECTSUCCESS:      //Alarm reconnect success
        QMessageBox::information(parent, "ERROR", "Alarm reconnect success!");
        break;

    case EXCEPTION_SERIAL:			           //Serial exception
        QMessageBox::information(parent, "ERROR", "Serial exception!");
        break;

        //Preview//
    case EXCEPTION_PREVIEW:			     //Preview exception
        QMessageBox::information(parent, "ERROR", "Preview exception!");
        break;
    case EXCEPTION_RECONNECT:			 //preview reconnect
        QMessageBox::information(parent, "ERROR", "Preview reconnect exception!");
        break;
    case PREVIEW_RECONNECTSUCCESS: //Preview reconnect success
        QMessageBox::information(parent, "ERROR", "Preview reconnect success!");
        break;
    default:
        break;
    }
}

bool sdkInit(QWidget* parent)
{
    if (!NET_DVR_Init())
    {
        QMessageBox::warning(parent,"SDK INITIAL ERROR", QString("SDK_LAST_ERROR=%1").arg(NET_DVR_GetLastError()));

        qDebug() << "NET_DVR_Init Fail" << QString::number(NET_DVR_GetLastError());

        return false;
    }
    else
    {
        qDebug() << "-----------------NET_DVR_Init SUCCESS-----------------";
    }

    NET_DVR_SetLogPrint(true);

    NET_DVR_SetConnectTime(3000, 1);
    //注册接收异常、重连等消息的窗口句柄或回调函数。
    NET_DVR_SetExceptionCallBack_V30(0, NULL, ExceptionCallBack, parent);

    qDebug()<< "SDKBuildVersion:" << NET_DVR_GetSDKBuildVersion();
    qDebug()<< "M4PlayVersion:" << PlayM4_GetSdkVersion();
    //注册回调函数，接收设备报警消息等。
    NET_DVR_SetDVRMessageCallBack_V30(MessCallBack_V30,(void*)0);
    //启动监听，接收设备主动上传的报警等信息    m_glistenheandle用于判断关闭时是否需要关闭listen
    int listenhandle = NET_DVR_StartListen_V30(NULL, 7200, MessCallBack_V30, (void*)0 );
    if( listenhandle < 0 )
    {
        qDebug() << "StartListen failed, lastError:" << NET_DVR_GetLastError();
    }

    qDebug() << "set capture jpeg mode" << NET_DVR_SetCapturePictureMode(JPEG_MODE);

    return true;
    //av_register_all();
}

QImage cvMat2Image(cv::Mat src)
{
    QImage dest((const uchar *) src.data, src.cols, src.rows, src.step, QImage::Format_RGB888);
    dest.bits();

    return dest.rgbSwapped();
}


QList<BBox> json2obj(QString json)
{
    QList<BBox> bboxes;
    if(json == "\"\"")
        return bboxes;

    QJsonParseError jsonError;
    QJsonDocument doucment = QJsonDocument::fromJson(json.toLocal8Bit(), &jsonError);  // 转化为 JSON 文档
    if (!doucment.isNull() && (jsonError.error == QJsonParseError::NoError)) {  // 解析未发生错误
        QJsonObject object = doucment.object();  // 转化为对象
        QJsonValue value = object.value("0");  // 获取指定 key 对应的 value
        QJsonArray array = value.toArray();
        for(QJsonArray::Iterator it = array.begin(); it != array.end(); ++it)
        {
            QJsonArray array = (*it).toArray();
            BBox bbox;

            bbox.type = array.at(0).toDouble();
            bbox.y1 = array.at(1).toDouble();
            bbox.x1 = array.at(2).toDouble();

            bbox.y2 = array.at(3).toDouble();
            bbox.x2 = array.at(4).toDouble();

            bbox.predict0 = array.at(5).toDouble();
            bbox.predict1 = array.at(6).toDouble();
            bbox.predict2 = array.at(7).toDouble();

            bboxes.append(bbox);
        }
    }

    return bboxes;
}
