#ifndef COMMONSTRUCTS_H
#define COMMONSTRUCTS_H
#include <QString>

typedef struct{
    int type;
    int x1, y1, x2, y2;
    double predict0, predict1, predict2;
    QString savePath = "None";
}BBox;

#endif // COMMONSTRUCTS_H
