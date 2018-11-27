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
const char* const PY_ROOT_PATH = "/home/zg/yaochang";
const char* const SINGAL_LOCKER_PATH = "/home/zg/camera_monitor/Settings/singal.lock";

const char* const NEW_RECORD = "newrecord";
const char* const WATCHED = "watched";

const char* const XML_PATH = "/home/zg/camera_monitor/Settings/devices.xml";
const char* const PATH_PREFIX = "/home/zg/1T/gifs";       //"/home/zg/yaochang/gifs";
const char* const WARNING_ICON_1 = "/home/zg/camera_monitor/Settings/warning_1.png";
const char* const WARNING_ICON_2 = "/home/zg/camera_monitor/Settings/warning_2.png";

const char* const DEVICE_ICON = "/home/zg/camera_monitor/Settings/device.png";
const char* const CAMERAL_ICON = "/home/zg/camera_monitor/Settings/cameral.png";
const char* const CAMERAL_PLAYING_ICON = "/home/zg/camera_monitor/Settings/cameral_playing.png";

const char* const DATE_ICON = "/home/zg/camera_monitor/Settings/calendar.png";

const char* const RIGHT_ICON = "/home/zg/camera_monitor/Settings/right.png";
const char* const LEFT_ICON = "/home/zg/camera_monitor/Settings/left.png";

const int FRAME_ROW_COUNT = 2;
const int FRAME_COL_COUNT = 2;
const int MAX_DISPLAY_FRAME = FRAME_COL_COUNT * FRAME_ROW_COUNT;

const bool DEBUG_LOG = true;
const int CONNECT_TIMEOUT = 3000;

#endif

