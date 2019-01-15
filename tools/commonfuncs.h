#ifndef COMMONFUNCS_H
#define COMMONFUNCS_H

#include "HK/DemoPublic.h"

#include<QDebug>
#include<QFile>
#include<QXmlStreamReader>
#include<QXmlStreamWriter>
#include <QMessageBox>
#include<structs/devicedata.h>
#include<QUuid>
#include<QJsonDocument>
#include<QJsonObject>
#include<QJsonArray>
#include "structs/commonstructs.h"

#include "opencv/cv.h"
#include "opencv/highgui.h"

#include"testform.h"
#include "common.h"
class TestForm;

QImage cvMat2Image(cv::Mat&);
QString decrypt(QString encryptStr);
QString encrypt(QString srcStr);
bool resetDeviceTreeXml(QMap<QString, DeviceData> &deviceList, QString xmlFilePath);
bool analysis(QMap<QString, DeviceData> &deviceList, QString xmlFilePath);
//int FrameSave2JPG(AVFrame* pFrame, int width, int height, int id, void *dwUser);
//AVFrame* buf2Frame(uchar* pBuf, int width ,int height);
cv::Mat buf2cvMat(uchar* pBuf, int width ,int height);
bool matSave2JPG(cv::Mat &mat, int id, void *dwUser);
void showReal(cv::Mat &img, void *dwUser);

void CALLBACK DecCBFun(int lPort,char * pBuf,int nSize,FRAME_INFO * pFrameInfo, void* dwUser,int nReserved2);
void CALLBACK DataCallBack(LONG lRealHandle,DWORD dwDataType,BYTE *pBuffer,DWORD  dwBufSize, void* dwUser);
void CALLBACK MessCallBack_V30(LONG lCommand, NET_DVR_ALARMER *pAlarmer, char *pAlarmInfo, DWORD dwBufLen, void* pUser);
void CALLBACK ExceptionCallBack(DWORD dwType, LONG lUserID, LONG lHandle, void *pUser);

bool sdkInit(QWidget *);
#endif // COMMONFUNCS_H
