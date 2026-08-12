#include <QApplication>

#include "LogHandler.h"
#include "MyProxyStyle.h"

#include "SingleApplication.h"
#include "MainApp.h"

int main(int argc, char *argv[])
{
    qInstallMessageHandler(appLogHandler);

    SingleApplication a(argc, argv);

    QDir::setCurrent(QApplication::applicationDirPath());

    QApplication::setQuitOnLastWindowClosed(true);

    if (a.isRunning())
    {
        return 0;
    }

    qApp->setStyle(new MyProxyStyle);

    MainApp* mainApp = MainApp::Instance();
    mainApp->Run();


    return a.exec();
}
