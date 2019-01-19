#include "testform.h"
#include <QtGui>
#include <QApplication>
#include <common.h>
#include <QDesktopWidget>
#include <QMessageBox>
#include <QLockFile>
#include "tfModelClient/tfserverclient.h"

TestForm *form;
Config* config;
/*******************************************************************
      Function:   main
   Description:   主函数入口
     Parameter:   (IN)   int argc
                  (IN)   char *argv[]
**********************************************************************/
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    config = new Config(a.applicationDirPath()+"/Settings/config.ini");

    //make sure singal instance in runtime
    QLockFile lockFile(config->SINGAL_LOCKER_PATH);

    if (!lockFile.tryLock(100))
    {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setText("The application is already running.\nAllowed to run only one instance of the application.");
        msgBox.exec();
        return 1;
    }

    int w = a.desktop()->width();
    int h = a.desktop()->height();
    qDebug()<<"current applicationDirPath: "<<QCoreApplication::applicationDirPath();
    qDebug()<<"current Path: "<<QDir::currentPath();

    TestForm f(argc, argv, w, h);
    form = &f;

    f.showMaximized();

    return a.exec();
}
