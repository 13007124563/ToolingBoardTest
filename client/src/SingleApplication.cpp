#include "SingleApplication.h"

#include <QFileInfo>
#include <QTimer>
#include <QWidget>

#define TIME_OUT 500

SingleApplication::SingleApplication(int &argc,char **argv)
    : QApplication(argc,argv)
    ,w(NULL)
    ,running(false)
    ,localServer(NULL)
{
    serverName = QFileInfo(QCoreApplication::applicationFilePath()).fileName();
    initLocalConnection();
    if(argc > 1)
    {
        running = false;
        QTimer::singleShot(1000,this,SLOT(initLocalConnection()));
    }
}


bool SingleApplication::isRunning()
{
    return running;
}

void SingleApplication::newLocalConnection()
{
    QLocalSocket *socket = localServer->nextPendingConnection();
    if(socket)
    {
        socket->waitForReadyRead(2*TIME_OUT);
        delete socket;
        activateWindow();
    }
}

void SingleApplication::initLocalConnection()
{
    running = false;
    QLocalSocket socket;
    socket.connectToServer(serverName);
    if(socket.waitForConnected(TIME_OUT))
    {
        running = true;
        return;
    }
    newLocalServer();
}

void SingleApplication::newLocalServer()
{
    localServer = new QLocalServer(this);
    connect(localServer,SIGNAL(newConnection()),this,SLOT(newLocalConnection()));
    if(!localServer->listen(serverName))
    {
        if(localServer->serverError() == QAbstractSocket::AddressInUseError)
        {
            QLocalServer::removeServer(serverName);
            localServer->listen(serverName);
        }
    }
}

void SingleApplication::activateWindow()
{
    if(w)
    {
        w->show();
        w->raise();
        w->activateWindow();
    }
}
