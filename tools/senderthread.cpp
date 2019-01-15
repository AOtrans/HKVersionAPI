#include "senderthread.h"
#include <QDebug>
#include <QDateTime>
#include "common.h"
#include <QDir>

SenderThread::SenderThread(ChannelData *cdata, QObject *parent)
    :QThread(parent)
{
    this->cdata = cdata;
    predictor = new GrpcPredictor(this);
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
            qDebug() << "currentThread:" << QThread::currentThread();
            qDebug() << "---------------------------------------maxQueue begin predict";
            QQueue<cv::Mat> param = cdata->getImageQueue();

            qDebug() << "--------------------------------------- have 16 images and send" ;


            QString save_path = QString(PATH_PREFIX) + "/" + QDate::currentDate().toString("yyyy-MM-dd") + "/" + cdata->getSaveDir();
            QDir dir(save_path);

            if(!dir.exists())
                dir.mkpath(save_path);

            QString param2 = save_path + "," + QString::number(QDateTime::currentDateTime().toTime_t());

            QList<BBox> &&bboxes = predictor->predict(param, param2);
            cdata->getFrame()->setBboxes(bboxes);

            QStringList filePaths;
            for(int i =0; i < bboxes.size(); i++)
            {
                if(bboxes.at(i).savePath != "None")
                    filePaths << bboxes.at(i).savePath;
            }

            emit addRow(filePaths);
        }
        else
        {
            msleep(100);
        }
    }
}
