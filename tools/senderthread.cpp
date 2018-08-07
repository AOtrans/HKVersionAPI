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
        PyObject *param = cdata->makeImagePackge();
        if(param)
        {
            qDebug() << "--------------------------------------- have 16 images and send" ;
            qDebug() << py_loader.callPyMethod(param);
        }
        else
        {
            msleep(100);
        }
    }
}
