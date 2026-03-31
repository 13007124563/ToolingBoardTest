#ifndef CSIGNALAPPLICATION_H_
#define CSIGNALAPPLICATION_H_

#include <QObject>
#include <QApplication>
#include <QtNetwork/QLocalServer>
#include <QtNetwork/QLocalSocket>


class SingleApplication : public QApplication
{
    Q_OBJECT

public:
    SingleApplication(int &argc,char **argv);
    bool isRunning();
    QWidget *w;

private slots:
    void newLocalConnection();
    void initLocalConnection();
private:
    void newLocalServer();
    void activateWindow();

    bool running;
    QLocalServer *localServer;
    QString serverName;
};

#endif // CSIGNALAPPLICATION_H_
