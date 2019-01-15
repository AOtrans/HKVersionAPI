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
#include <QMessageBox>

#define DEBUG 0
#ifdef DEBUG
#define DBG(a, b) \
	(QMessageBox::information(this,tr(a),tr("line=%1").arg(b)))
#else
#define DBG(a, b) 
#endif
const char* const SINGAL_LOCKER_PATH = "/home/sw/Work/QtProject/build-HKVision-Desktop_Qt_5_7_1_GCC_64bit-Debug/Settings/singal.lock";

const char* const NEW_RECORD = "newrecord";
const char* const WATCHED = "watched";

const char* const XML_PATH = "/home/sw/Work/QtProject/HKVision/Settings/devices.xml";
const char* const PATH_PREFIX = "/home/sw/data/gifs";       //"/home/zg/yaochang/gifs";
const char* const WARNING_ICON_1 = "/home/sw/Work/QtProject/build-HKVision-Desktop_Qt_5_7_1_GCC_64bit-Debug/Settings/warning_1.png";
const char* const WARNING_ICON_2 = "/home/sw/Work/QtProject/build-HKVision-Desktop_Qt_5_7_1_GCC_64bit-Debug/Settings/warning_2.png";

const char* const DEVICE_ICON = "/home/sw/Work/QtProject/build-HKVision-Desktop_Qt_5_7_1_GCC_64bit-Debug/Settings/device.png";
const char* const CAMERAL_ICON = "/home/sw/Work/QtProject/build-HKVision-Desktop_Qt_5_7_1_GCC_64bit-Debug/Settings/cameral.png";
const char* const CAMERAL_PLAYING_ICON = "/home/zg/camera_monitor/Settings/Settings/cameral_playing.png";

const char* const DATE_ICON = "/home/sw/Work/QtProject/build-HKVision-Desktop_Qt_5_7_1_GCC_64bit-Debug/Settings/calendar.png";

const char* const RIGHT_ICON = "/home/sw/Work/QtProject/build-HKVision-Desktop_Qt_5_7_1_GCC_64bit-Debug/Settings/right.png";
const char* const LEFT_ICON = "/home/sw/Work/QtProject/build-HKVision-Desktop_Qt_5_7_1_GCC_64bit-Debug/Settings/left.png";

const int FRAME_ROW_COUNT = 2;
const int FRAME_COL_COUNT = 2;
const int MAX_DISPLAY_FRAME = FRAME_COL_COUNT * FRAME_ROW_COUNT;

const bool DEBUG_LOG = true;
const int CONNECT_TIMEOUT = 3000;

const char* const GRPC_SERVER = "192.168.3.235:8500";
const char* const MOBILENET = "mnet";
const char* const MOBILENET_SIG = "classify_image";
const char* const TACTION = "3daction";
const char* const TACTION_SIG = "predict_image";
const char* const YOLOV3 = "yolov3";
const char* const YOLOV3_SIG = "detect_image";

const float TWICECHECK_SCORE_THRESHOLD = 0.2;
const float IOU_THRESHOLD = 0.8;

#endif

