#include "grpcpredictor.h"
#include <QUuid>

int GrpcPredictor::index = 0;

GrpcPredictor::GrpcPredictor(QObject *parent):
    QObject(parent)
{
    index++;
}

QVector<int> GrpcPredictor::argmax(QVector<QVector<float> > &inputs)
{
    QVector<int> outputs;

    for(int i=0; i<inputs.size(); i++)
    {
        int index = 0;
        QVector<float> &tmp = inputs[i];

        for(int j=1; j<tmp.size(); j++)
        {
            if(tmp[j]>tmp[index])
                index = j;
        }

        outputs.append(index);
    }

    return outputs;
}

QList<BBox> GrpcPredictor::predict(QQueue<cv::Mat> &param, QString param2)
{
    if(param.size()!=16)
    {
        qDebug() << "wrong input size";
        return QList<BBox>();
    }

    grpc::ChannelArguments args;
    args.SetUserAgentPrefix(std::to_string(index));
    TFServerClient guide(grpc::CreateCustomChannel(GRPC_SERVER, grpc::InsecureChannelCredentials(), args));


    QStringList frameInfo = param2.split(",");
    QString savePath = frameInfo[0];
    QString timeT = frameInfo[1];

    cv::Mat detectImage = param[0];
    int width = detectImage.cols;
    int height = detectImage.rows;
    QVector<QVector<float> > boxes;
    QVector<QVector<float> > tdaction_classes;


    QVector<QVector<cv::Mat> > batch_inputs;

    QVector<cv::Mat> inputs;
    inputs.append(detectImage);
    batch_inputs.append(inputs);

    bool flag = guide.callPredict(YOLOV3, YOLOV3_SIG, batch_inputs, boxes);
    batch_inputs.clear();
    inputs.clear();

    if(!flag)
        return QList<BBox>();

    foreach (QVector<float> box, boxes) {
        int top = box[0];
        int left = box[1];
        int bottom = box[2];
        int right = box[3];

        int center_x = (right+left)/2;
        int center_y = (bottom+top)/2;
        int w = right-left;
        int h = bottom-top;

        int csize = MAX(w, h);
        csize = MIN(MIN(csize, width-5), height-5);

        top = int(center_y - (csize/2));
        left = center_x - (csize/2);
        bottom = center_y + (csize/2);
        right = center_x + (csize/2);

        if(top < 0)
        {
            bottom = bottom - top;
            top = 0;
        }
        if(left < 0)
        {
            right = right - left;
            left = 0;
        }
        if(bottom > height)
        {
            top = top + height - bottom;
            bottom = height;
        }
        if(right > width)
        {
            left = left + width - right;
            right = width;
        }

        inputs.clear();
        for(int i=0; i<param.size(); i++)
        {
            qDebug()<< param[i].cols << param[i].rows<< top << left << right-left << bottom-top;
            cv::Rect roi(left, top, right-left, bottom-top);
            inputs.append(param[i](roi));
        }
        batch_inputs.append(inputs);
    }

    QList<BBox> ret_bboxes;

    if(boxes.size() != 0)
    {
        bool flag = guide.callPredict(TACTION, TACTION_SIG, batch_inputs, tdaction_classes);

        if(!flag)
            return QList<BBox>();

        QVector<int> result = argmax(tdaction_classes);

        QStringList fileSavePath;
        for(int i=0; i<result.size(); i++)
        {
            if(result[i]!=0)
            {
                QVector<QVector<float> > mnet_classes;
                QVector<QVector<cv::Mat> > tmp_inputs;
                tmp_inputs.append(batch_inputs.at(i));
                bool flag = guide.callPredict(MOBILENET, MOBILENET_SIG, tmp_inputs, mnet_classes);

                if(!flag)
                    return QList<BBox>();

                if(twice_check(mnet_classes))
                {
                    QString tmpPath = QString("%1/newrecord_%2_%3_%4.gif").arg(savePath).arg(result[i]).arg(timeT).arg(QUuid::createUuid().toString());
                    if(!check_box(boxes[i]))
                    {
                        fileSavePath.append(tmpPath);
                    }
                    else
                    {
                        fileSavePath.append("None");
                    }
                }
                else
                {
                    fileSavePath.append("None");
                    result[i]=0;
                }
            }
            else
            {
                fileSavePath.append("None");
            }
        }

        for(int i=0; i<boxes.size(); i++)
        {
            BBox box;

            box.type = result[i];
            box.y1 = boxes[i].at(0);
            box.x1 = boxes[i].at(1);
            box.y2 = boxes[i].at(2);
            box.x2 = boxes[i].at(3);

            ret_bboxes.append(box);
        }
    }

    pre_boxes = boxes;

    return ret_bboxes;
}

bool GrpcPredictor::twice_check(QVector<QVector<float> > &inputs)
{
    QVector<int> result = argmax(inputs);
    float sum=0.0;
    for(int i=0; i<result.size(); i++)
    {
        sum+=result[i];
    }

    sum/=result.size();

    if(sum>TWICECHECK_SCORE_THRESHOLD)
    {
        qDebug() << "pass twice check";
        return true;
    }
    else
    {
        qDebug() << "********************twice check delete one************************";
        return false;
    }
}

bool match(QVector<float> box1, QVector<float> box2)
{
    float cx1 = box1[0];
    float cy1 = box1[1];
    float cx2 = box1[2];
    float cy2 = box1[3];

    float gx1 = box2[0];
    float gy1 = box2[1];
    float gx2 = box2[2];
    float gy2 = box2[3];

    float carea = (cx2 - cx1) * (cy2 - cy1);
    float garea = (gx2 - gx1) * (gy2 - gy1);

    float x1 = MAX(cx1, gx1);
    float y1 = MAX(cy1, gy1);
    float x2 = MIN(cx2, gx2);
    float y2 = MIN(cy2, gy2);
    float w = MAX(0, x2 - x1);
    float h = MAX(0, y2 - y1);
    float area = w * h;

    float iou = area / (carea + garea - area);

    return iou>IOU_THRESHOLD;
}

bool GrpcPredictor::check_box(QVector<float> &inputs)
{
    foreach(QVector<float> pre_box, pre_boxes)
    {
        if(match(inputs, pre_box))
        {
            qDebug() << "************delete one**************";
            return true;
        }
    }

    return false;
}
