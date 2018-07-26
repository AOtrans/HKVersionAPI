#include "testform.h"
#include <QtGui>
#include <QApplication>
TestForm *form;
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

    qDebug()<<"current applicationDirPath: "<<QCoreApplication::applicationDirPath();
    qDebug()<<"current currentPath: "<<QDir::currentPath();

    TestForm f;
    form = &f;

    f.showMaximized();

    return a.exec();
}
