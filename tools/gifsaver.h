#ifndef GIFSAVER_H
#define GIFSAVER_H
#include <QThread>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/ml/ml.hpp>
#include <QMap>
#include <QVector>

class GIFSaver : public QThread
{
    Q_OBJECT
public:
    GIFSaver(const QMap<QString, QVector<cv::Mat>> &saveImgs, QObject*parent=0);
    virtual void run() override;
private:
    QMap<QString, QVector<cv::Mat>> saveImgs;
};

#endif // GIFSAVER_H
