#include "testform.h"
#include "tools/pyloader.h"
#include <QtGui>
#include <QApplication>
#include <QDesktopWidget>
TestForm *form;
PYLoader py_loader;
/*******************************************************************
      Function:   main
   Description:   主函数入口
     Parameter:   (IN)   int argc
                  (IN)   char *argv[]
        Return:   0--成功，-1--失败。
**********************************************************************/
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    int w = a.desktop()->width();
    int h = a.desktop()->height();
    qDebug()<<"current applicationDirPath: "<<QCoreApplication::applicationDirPath();
    qDebug()<<"current currentPath: "<<QDir::currentPath();

    TestForm f(argc, argv, w, h);
    form = &f;

    f.showMaximized();

    return a.exec();
}
