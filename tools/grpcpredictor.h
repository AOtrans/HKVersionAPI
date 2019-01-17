#ifndef GRPCPREDICTOR_H
#define GRPCPREDICTOR_H
#include "tools/commonfuncs.h"
#include "structs/commonstructs.h"
#include <QObject>
#include "tfModelClient/tfserverclient.h"

class GrpcPredictor:public QObject
{
    Q_OBJECT
public:
    GrpcPredictor(QObject* parent);

    QVector<int> argmax(QVector<QVector<float> >& inputs);

    QList<BBox> predict(QQueue<cv::Mat> &param, QString param2);

    bool twice_check(QVector<QVector<float> >& inputs);

    bool check_box(QVector<float>& inputs);

    static int index;
private:
    QVector<QVector<float> > pre_boxes;
    TFServerClient *guide;
private slots:
    void cleanThread();
};

#endif // GRPCPREDICTOR_H
