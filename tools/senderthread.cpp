#include "senderthread.h"
#include "pyloader.h"
#include <QDebug>
#include <QDateTime>
#include "common.h"
#include <QDir>

extern PYLoader py_loader;

SenderThread::SenderThread(ChannelData *cdata, QObject *parent)
    :QThread(parent)
{
    this->cdata = cdata;
    connect(this, SIGNAL(addRow(QStringList)), parent, SLOT(addRow(QStringList)) );
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
        //if has 16 imgs
        if(cdata->checkQueueMax())
        {
#ifdef THREAD_ABLE
            //make sure get GIL before use python
            PyEnsureGIL gil;
#endif
            qDebug() << "currentThread:" << QThread::currentThread();
            qDebug() << "---------------------------------------maxQueue begin making packge";
            PyObject *param = cdata->makeImagePackge();

            if(!param)
            {
                qDebug() << "------------------------------------------fail to alloc pylist";
                continue;
            }

            qDebug() << "--------------------------------------- have 16 images and send" ;


            QString save_path = QString(PATH_PREFIX) + "/" + QDate::currentDate().toString("yyyy-MM-dd") + "/" + cdata->getSaveDir();
            QDir dir(save_path);

            if(!dir.exists())
                dir.mkpath(save_path);

            QString cameralinfo = save_path + "," + QString::number(QDateTime::currentDateTime().toTime_t());

            PyObject *param2 = Py_BuildValue("s", cameralinfo.toStdString().c_str());

            //call py method
            PyObject *ret = py_loader.callPyMethod(param, param2);

            if(ret)
            {
                QString json = QString(PyString_AsString(ret));
                qDebug() << json;
                //cvt json to object
                QList<BBox> &&bboxes = json2obj(json);
                cdata->getFrame()->setBboxes(bboxes);

                QStringList filePaths;
                for(int i =0; i < bboxes.size(); i++)
                {
                    if(bboxes.at(i).savePath != "None")
                        filePaths << bboxes.at(i).savePath;
                }

                emit addRow(filePaths);
                Py_DECREF(ret);
            }

            if(param)
                Py_DECREF(param);
            if(param2)
                Py_DECREF(param2);

        }
        else
        {
            msleep(100);
        }
    }
}
