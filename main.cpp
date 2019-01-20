#include "testform.h"
#include <QtGui>
#include <QApplication>
#include <common.h>
#include <QDesktopWidget>
#include <QMessageBox>
#include <QLockFile>

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
    qDebug()<<"current applicationDirPath: "<<QCoreApplication::applicationDirPath();
    qDebug()<<"current Path: "<<QDir::currentPath();
    config = new Config(a.applicationDirPath()+"/Settings/config.ini");

    if(!config->ready)
    {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setText("missing ini config file in:   " + a.applicationDirPath() + "/Settings/config.ini");
        msgBox.exec();
        return 1;
    }

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

    TestForm f(argc, argv, w, h);
    form = &f;

    f.showMaximized();

    return a.exec();
}
