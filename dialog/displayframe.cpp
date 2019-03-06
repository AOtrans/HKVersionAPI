#include "displayframe.h"

DisplayFrame::DisplayFrame(QWidget *parent, int id):
    QFrame(parent)
{
    setStyleSheet("background:rgb(0, 0, 0);");

    //painter = new PainterEvent(this);
    //this->installEventFilter(painter);
    this->m_id = id;

    connect(this, SIGNAL(doUpdate()), this, SLOT(update()));
}

DisplayFrame::~DisplayFrame()
{
    qDebug() << "kill Frame";
//    if(painter)
//        delete painter;
}

//PainterEvent *DisplayFrame::getPainter() const
//{
//    return painter;
//}

bool DisplayFrame::getIsPlaying() const
{
    return isPlaying;
}

void DisplayFrame::setIsPlaying(bool value)
{
    isPlaying = value;
}

int DisplayFrame::getId() const
{
    return m_id;
}

void DisplayFrame::paintEvent(QPaintEvent *event)
{
    matlock.lock();
    QPainter painter(this);

    if( blackbg == false && frameMat.data != NULL)
    {
        for(int i =0 ; i < bboxes.size(); i++)
        {
            BBox box = bboxes.at(i);

            switch (box.type) {
            case NONE:
                cv::rectangle(frameMat, cv::Rect(box.x1, box.y1, box.x2 - box.x1, box.y2 - box.y1 ), cv::Scalar(0, 255, 0), 4);
                break;
            case PHONE:
                cv::rectangle(frameMat, cv::Rect(box.x1, box.y1, box.x2 - box.x1, box.y2 - box.y1 ), cv::Scalar(0, 0, 255), 4);
                break;
            case PHOTO:
                cv::rectangle(frameMat, cv::Rect(box.x1, box.y1, box.x2 - box.x1, box.y2 - box.y1 ), cv::Scalar(0, 0, 255), 4);
                break;
            default:
                break;
            }
        }

        cv:: Mat tmp;
        cv::cvtColor(frameMat, tmp, CV_BGR2RGB);

        float img_aspect = float(tmp.cols) / float(tmp.rows);
        float frame_aspect = float(this->width()) / float(this->height());

        int width = this->width();
        int height = this->height();

        if(img_aspect > frame_aspect)
        {
            width -= 6;
            painter.drawPixmap(3, (height - width/img_aspect)/2, width, width/img_aspect, QPixmap::fromImage(cvMat2Image(tmp)) );
        }
        else
        {
            height -= 6;
            painter.drawPixmap((width - height*img_aspect)/2, 3, height*img_aspect, height, QPixmap::fromImage(cvMat2Image(tmp)) );
        }
    }
    else
    {
        QPixmap bg(this->width(), this->height());
        bg.fill(QColor(0, 0, 0));
        painter.drawPixmap(0, 0, this->width(), this->height(), bg);
    }
    matlock.unlock();

    QFrame::paintEvent(event);
}

QList<BBox> DisplayFrame::getBboxes() const
{
    return bboxes;
}

void DisplayFrame::setBboxes(const QList<BBox> &value)
{
    bboxes = value;
}

void DisplayFrame::reset()
{
    setIsPlaying(false);
    setBlackbg(true);
    bboxes.clear();
    frameMat = cv::Mat();
    update();

}

bool DisplayFrame::getBlackbg() const
{
    return blackbg;
}

void DisplayFrame::setBlackbg(bool value)
{
    blackbg = value;
}

bool DisplayFrame::changeMat(cv::Mat mat)
{
    matlock.lock();
    frameMat = mat;
    emit doUpdate();
    qApp->processEvents();
    matlock.unlock();
    return true;
}
/*-----------------------------------------------------------*/
//bool PainterEvent::getBlackbg() const
//{
//    return blackbg;
//}

//void PainterEvent::setBlackbg(bool value)
//{
//    blackbg = value;
//}

//bool PainterEvent::eventFilter(QObject *obj, QEvent *event)
//{
//    qDebug() << "draw frame" << obj << event->type();
//    if (event->type() == QEvent::Paint) {
//        if( blackbg == false && frameMat.data != NULL)
//        {
//            QFrame *frame = (QFrame*)obj;
//            QPainter painter(frame);
//            painter.drawPixmap(0, 0, frame->width(), frame->height(), QPixmap::fromImage(cvMat2Image(frameMat)) );
//        }
//        else
//        {
//            QFrame *frame = (QFrame*)obj;
//            QPainter painter(frame);

//            QPixmap bg(frame->width(), frame->height());
//            bg.fill(QColor(0, 0, 0));
//            painter.drawPixmap(0, 0, frame->width(), frame->height(), bg);
//        }
//    }

//    // standard event processing
//    return QObject::eventFilter(obj, event);
//}
