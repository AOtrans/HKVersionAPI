#include "testform.h"
#include <QtGui>
#include <QApplication>
#include "tools/pyloader.h"
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

    qDebug()<<"current applicationDirPath: "<<QCoreApplication::applicationDirPath();
    qDebug()<<"current currentPath: "<<QDir::currentPath();

    if(!py_loader.initPY(argc, argv))
    {
        //QMessageBox::warning(this, "error", "load PY func failed");
        qDebug() << "load PY func failed";
    }

    TestForm f;
    form = &f;

    f.showMaximized();

    return a.exec();
}
