/************************************************************************
 *        Copyright 2009-2011 Hikvision Digital Technology Co., Ltd.
 *        FileName                      :   common.h
 *        Description                   :   helper 
 *        Modification History          :   none
 *        Version                       :   V1.0
 *        Time                          :   2009-11,12
 *        Author                        :   wanggongpu
 *        Descrp                        :   none
*************************************************************************/
#ifndef COMMON_H
#define COMMON_H
#include<QString>
#include<QSettings>
#include<QFile>
#include<QDebug>
class Config
{
public:
    Config(QString iniFile){
        if(!QFile(iniFile).exists())
        {
            qWarning() << "missing ini config file";
            ready = false;
            return;
        }

        qDebug() << "load ini CONFIG from" << iniFile;
        qDebug() << "*********************************************************";
        QSettings set(iniFile, QSettings::IniFormat);
        SINGAL_LOCKER_PATH = set.value("CONFIG/SINGAL_LOCKER_PATH").toString();
        qDebug() << "load SINGAL_LOCKER_PATH:" << SINGAL_LOCKER_PATH;
        NEW_RECORD = set.value("CONFIG/NEW_RECORD").toString();
        qDebug() << "load NEW_RECORD:" << NEW_RECORD;
        WATCHED = set.value("CONFIG/WATCHED").toString();
        qDebug() << "load WATCHED:" << WATCHED;
        XML_PATH = set.value("CONFIG/XML_PATH").toString();
        qDebug() << "load XML_PATH:" << XML_PATH;
        PATH_PREFIX = set.value("CONFIG/PATH_PREFIX").toString();
        qDebug() << "load PATH_PREFIX:" << PATH_PREFIX;
        WARNING_ICON_1 = set.value("CONFIG/WARNING_ICON_1").toString();
        qDebug() << "load WARNING_ICON_1:" << WARNING_ICON_1;
        WARNING_ICON_2 = set.value("CONFIG/WARNING_ICON_2").toString();
        qDebug() << "load WARNING_ICON_2:" << WARNING_ICON_2;
        DEVICE_ICON = set.value("CONFIG/DEVICE_ICON").toString();
        qDebug() << "load DEVICE_ICON:" << DEVICE_ICON;
        CAMERAL_ICON = set.value("CONFIG/CAMERAL_ICON").toString();
        qDebug() << "load CAMERAL_ICON:" << CAMERAL_ICON;
        CAMERAL_PLAYING_ICON = set.value("CONFIG/CAMERAL_PLAYING_ICON").toString();
        qDebug() << "load CAMERAL_PLAYING_ICON:" << CAMERAL_PLAYING_ICON;
        DATE_ICON = set.value("CONFIG/DATE_ICON").toString();
        qDebug() << "load DATE_ICON:" << DATE_ICON;
        RIGHT_ICON = set.value("CONFIG/RIGHT_ICON").toString();
        qDebug() << "load RIGHT_ICON:" << RIGHT_ICON;
        LEFT_ICON = set.value("CONFIG/LEFT_ICON").toString();
        qDebug() << "load LEFT_ICON:" << LEFT_ICON;
        FRAME_ROW_COUNT = set.value("CONFIG/FRAME_ROW_COUNT").toInt();
        qDebug() << "load FRAME_ROW_COUNT:" << FRAME_ROW_COUNT;
        FRAME_COL_COUNT = set.value("CONFIG/FRAME_COL_COUNT").toInt();
        qDebug() << "load FRAME_COL_COUNT:" << FRAME_COL_COUNT;
        MAX_DISPLAY_FRAME = FRAME_COL_COUNT * FRAME_ROW_COUNT;
        DEBUG_LOG = set.value("CONFIG/DEBUG_LOG").toBool();
        qDebug() << "load DEBUG_LOG:" << DEBUG_LOG;
        CONNECT_TIMEOUT = set.value("CONFIG/CONNECT_TIMEOUT").toInt();
        qDebug() << "load CONNECT_TIMEOUT:" << CONNECT_TIMEOUT;

        GRPC_SERVER = set.value("CONFIG/GRPC_SERVER").toString();
        qDebug() << "load GRPC_SERVER:" << GRPC_SERVER;
        MOBILENET = set.value("CONFIG/MOBILENET").toString();
        qDebug() << "load MOBILENET:" << MOBILENET;
        MOBILENET_SIG = set.value("CONFIG/MOBILENET_SIG").toString();
        qDebug() << "load MOBILENET_SIG:" << MOBILENET_SIG;
        TACTION = set.value("CONFIG/TACTION").toString();
        qDebug() << "load TACTION:" << TACTION;
        TACTION_SIG = set.value("CONFIG/TACTION_SIG").toString();
        qDebug() << "load TACTION_SIG:" << TACTION_SIG;
        YOLOV3 = set.value("CONFIG/YOLOV3").toString();
        qDebug() << "load YOLOV3:" << YOLOV3;
        YOLOV3_SIG = set.value("CONFIG/YOLOV3_SIG").toString();
        qDebug() << "load YOLOV3_SIG:" << YOLOV3_SIG;

        TWICECHECK_SCORE_THRESHOLD = set.value("CONFIG/TWICECHECK_SCORE_THRESHOLD").toFloat();
        qDebug() << "load TWICECHECK_SCORE_THRESHOLD:" << TWICECHECK_SCORE_THRESHOLD;
        IOU_THRESHOLD = set.value("CONFIG/IOU_THRESHOLD").toFloat();
        qDebug() << "load IOU_THRESHOLD:" << IOU_THRESHOLD;

        NUM_SERVICES = set.value("CONFIG/NUM_SERVICES").toInt();
        qDebug() << "load NUM_SERVICES:" << NUM_SERVICES;

        TEST_IMG_PATH = set.value("CONFIG/TEST_IMG_PATH").toString();
        qDebug() << "load TEST_IMG_PATH:" << TEST_IMG_PATH;
        qDebug() << "*********************************************************";

        ready = true;
    }
    QString SINGAL_LOCKER_PATH;

    QString NEW_RECORD;
    QString WATCHED;

    QString XML_PATH;
    QString PATH_PREFIX;
    QString WARNING_ICON_1;
    QString WARNING_ICON_2;

    QString DEVICE_ICON;
    QString CAMERAL_ICON;
    QString CAMERAL_PLAYING_ICON;

    QString DATE_ICON;

    QString RIGHT_ICON;
    QString LEFT_ICON;

    int FRAME_ROW_COUNT;
    int FRAME_COL_COUNT;
    int MAX_DISPLAY_FRAME;

    bool DEBUG_LOG;
    int CONNECT_TIMEOUT;

    QString GRPC_SERVER;
    QString MOBILENET;
    QString MOBILENET_SIG;
    QString TACTION;
    QString TACTION_SIG;
    QString YOLOV3;
    QString YOLOV3_SIG;

    float TWICECHECK_SCORE_THRESHOLD;
    float IOU_THRESHOLD;

    int NUM_SERVICES;

    QString TEST_IMG_PATH;


    bool ready;
};
#endif

