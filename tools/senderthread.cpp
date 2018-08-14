#include "senderthread.h"
#include "pyloader.h"
#include <QDebug>
extern PYLoader py_loader;
SenderThread::SenderThread(ChannelData *cdata)
{
    this->cdata = cdata;
}

SenderThread::~SenderThread()
{
    requestInterruption();
    quit();
    wait();
}

void SenderThread::run()
{
    while(!isInterruptionRequested())
    {
        if(cdata->checkQueueMax())
        {
#ifdef THREAD_ABLE
            PyEnsureGIL gil;
#endif
            qDebug() << "currentThread:" << QThread::currentThread();
            qDebug() << "---------------------------------------maxQueue begin making packge";
            PyObject *param = cdata->makeImagePackge();
            qDebug() << "--------------------------------------- have 16 images and send" ;
            PyObject *ret = py_loader.callPyMethod(param);

            if(ret)
            {
                QString json = QString(PyString_AsString(ret));
                qDebug() << json;
                QList<BBox> &&bboxes = json2obj(json);
                cdata->getFrame()->setBboxes(bboxes);
            }

        }
        else
        {
            msleep(100);
        }
    }
}
