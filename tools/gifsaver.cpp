#include "gifsaver.h"
#include "gifmaker.h"
#include <QDebug>
GIFSaver::GIFSaver(const QMap<QString, QVector<cv::Mat> > &saveImgs, QObject*parent):
    QThread(parent)
{
    this->saveImgs = saveImgs;
}

GIFSaver::~GIFSaver()
{

}

void GIFSaver::run()
{
    foreach (QString key, saveImgs.keys()) {

        QVector<cv::Mat>& value = saveImgs[key];

        qDebug()<< key;
        GifWriter writer;

        GifBegin(&writer, key.toStdString().c_str(), 200, 200, 0.2);

        for(int i=0; i<value.size(); i++)
        {
            cv::Mat m = value[i].clone();

            cv::resize(m,m,cv::Size(200,200));
            cv::cvtColor(m,m,CV_BGR2RGBA);
            GifWriteFrame(&writer, m.data, 200, 200, 0.2);
        }

        GifEnd(&writer);
    }
}
