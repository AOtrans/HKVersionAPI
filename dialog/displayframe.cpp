#include "displayframe.h"

DisplayFrame::DisplayFrame(QWidget *parent, int id):
    QFrame(parent)
{
    setStyleSheet("background:rgb(0, 0, 0);");

    //painter = new PainterEvent(this);
    //this->installEventFilter(painter);
    this->m_id = id;
}

DisplayFrame::~DisplayFrame()
{
//    qDebug() << "kill Frame";
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
    if( blackbg == false && frameMat.data != NULL)
    {
        QPainter painter(this);
        painter.drawPixmap(0, 0, this->width(), this->height(), QPixmap::fromImage(cvMat2Image(frameMat)) );
    }
    else
    {
        QPainter painter(this);

        QPixmap bg(this->width(), this->height());
        bg.fill(QColor(0, 0, 0));
        painter.drawPixmap(0, 0, this->width(), this->height(), bg);
    }

    QFrame::paintEvent(event);
}

bool DisplayFrame::getBlackbg() const
{
    return blackbg;
}

void DisplayFrame::setBlackbg(bool value)
{
    blackbg = value;
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
