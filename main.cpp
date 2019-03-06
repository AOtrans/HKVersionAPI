#include <QtGui>
#include <QApplication>
#include <common.h>
#include <QDesktopWidget>
#include <QMessageBox>
#include <QLockFile>

#include "testform.h"
#include <QDir>
#include <QTextStream>
#include "iostream"
#include <QDateTime>
#include "QDebug"
using namespace std;
void setDebugOutput(const QString &rawTargetFilePath_, const bool &argDateFlag_)
{
    static QString rawTargetFilePath;
    static bool argDateFlag;
    rawTargetFilePath = rawTargetFilePath_;
    argDateFlag = argDateFlag_;
    class HelperClass
    {
        public:
            static void messageHandler(QtMsgType type, const QMessageLogContext &, const QString &message_)
            {
                QString message;
                switch ( type )
                {
                    case QtDebugMsg:
                        { message = message_; break; }
                    case QtWarningMsg:
                        { message.append("[Warning]: "); message.append(message_); break; }
                    case QtCriticalMsg:
                        { message.append("[Critical]: "); message.append(message_); break; }
                    case QtFatalMsg:
                        { message.append("[Fatal]: "); message.append(message_); break; }
                    default: { break; }
                }
                QString currentTargetFilePath;
                currentTargetFilePath = rawTargetFilePath;
                if ( !QFileInfo::exists( currentTargetFilePath ) )
                {
                    QDir().mkpath( QFileInfo( currentTargetFilePath ).path() );
                }
                QFile file( currentTargetFilePath );
                file.open( QIODevice::WriteOnly | QIODevice::Append );
                QTextStream textStream( &file );

                textStream << QDateTime::currentDateTime().toString( "yyyy-MM-dd hh:mm:ss:zzz" ) << ": " << message << endl;
            }
    };
    qInstallMessageHandler( HelperClass::messageHandler );
}




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
    setDebugOutput(a.applicationDirPath()+"/logs.log", false);
    qDebug()<<"current applicationDirPath: "<<QCoreApplication::applicationDirPath();
    qDebug()<<"current Path: "<<QDir::currentPath();
    config = new Config(a.applicationDirPath()+"/Settings/config.ini");

    if(!config->ready)
    {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setText("missing ini config file in:   " + a.applicationDirPath() + "/Settings/config.ini");
        msgBox.exec();
        qWarning() << "missing ini config file in:   " + a.applicationDirPath() + "/Settings/config.ini";
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
        qDebug() << "The application is already running.\nAllowed to run only one instance of the application.";
        return 1;
    }

    int w = a.desktop()->width();
    int h = a.desktop()->height();

    TestForm f(argc, argv, w, h);
    form = &f;

    f.showMaximized();

    return a.exec();
}
