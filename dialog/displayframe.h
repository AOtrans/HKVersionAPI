#ifndef DISPLAYFRAME_H
#define DISPLAYFRAME_H
#include <QFrame>
#include <QEvent>
#include <QPainter>
#include <QMutex>
#include <QLabel>
#include "tools/commonfuncs.h"
#include "structs/commonstructs.h"

//class PainterEvent;

enum state{NONE, PHONE, PHOTO};

class DisplayFrame : public QFrame
{
    Q_OBJECT
public:
    DisplayFrame(QWidget *parent, int id);
    virtual ~DisplayFrame() override;
//    PainterEvent *getPainter() const;

    bool getIsPlaying() const;
    void setIsPlaying(bool value);

    int getId() const;

    bool getBlackbg() const;
    void setBlackbg(bool value);

    bool changeMat(cv::Mat mat)
    {
        frameMat = mat;
        emit doUpdate();
        return true;
    }

    QList<BBox> getBboxes() const;
    void setBboxes(const QList<BBox> &value);

    void reset();
signals:
    void doUpdate();
private:
    virtual void paintEvent(QPaintEvent *) override;
    //    PainterEvent *painter;
    bool isPlaying = false;
    int m_id;


    QList<BBox> bboxes;
    cv::Mat frameMat;
    bool blackbg = true;
};


//class PainterEvent : public QObject
//{
//    Q_OBJECT
//public:
//    PainterEvent(QObject* parent):QObject(parent){}

//    bool changeMat(cv::Mat mat)
//    {
//        frameMat = mat;

//        return true;
//    }

//    bool getBlackbg() const;
//    void setBlackbg(bool value);

//private:
//    cv::Mat frameMat;
//    bool blackbg = true;

//protected:
//    virtual bool eventFilter(QObject *obj, QEvent *event) override;
//};

#endif // DISPLAYFRAME_H
