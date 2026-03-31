#ifndef LOG_HANDLER_H
#define LOG_HANDLER_H

#include <QtCore/QDateTime>
#include <QtCore/QTextStream>
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QThread>
#include <QMutex>

#include "commondefine.h"


/************************************************************************/
/*
通过qInstallMessageHandler(appLogHandler);
可以将qDebug、qWarning、qCritical、qFatal的信息输出到日志文件里
*/
/************************************************************************/
void appLogHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    Q_UNUSED(context);
    if (msg == "libpng warning: iCCP: known incorrect sRGB profile")
    {
        return;
    }

    static QMutex mutex;
    mutex.lock();

    QString typeStr;
    switch (type)
    {
    case QtDebugMsg:
        typeStr = QString("D");
        break;
    case QtWarningMsg:
        typeStr = QString("W");
        break;
    case QtCriticalMsg:
        typeStr = QString("C");
        break;
    case QtFatalMsg:
        typeStr = QString("F");
        break;
    default:
        typeStr = QString(" ");
        break;
    }

    QDateTime now = QDateTime::currentDateTime();

    //QString context_info = QString("File:(%1) Line:(%2)").arg(QString(context.file)).arg(context.line);
    QString nowStr = now.toString("yyyy-MM-dd hh:mm:ss");
    //QString current_date = QString("(%1)").arg(current_date_time);
    QString message = QString("[%1 %2 %3] %4").arg(nowStr).arg(typeStr).arg((quintptr)QThread::currentThreadId()).arg(msg);

    bool write_msg = true;

    if (SHOW_INFO_WARNING == false)
    {
        if (msg.contains("[INFO]") || msg.contains("[WARNING]"))
            write_msg = false;
    }

    if (write_msg)
    {
        QString filePath;

#ifdef PATH_LOG_FILE
        filePath = PATH_LOG_FILE;
#else
        filePath = "logs/";
#endif

        QDir dir;
        if (!dir.exists(filePath)) dir.mkpath(filePath);

        QString fileUrl = QString(filePath) + nowStr.split(" ")[0] + ".log";

        QFile file(fileUrl);
        bool isOk = file.open(QIODevice::WriteOnly | QIODevice::Append);
        if (isOk)
        {
            QTextStream text_stream(&file);
            text_stream << message << "\r\n";
            file.flush();
            file.close();
        }
    }

    mutex.unlock();
}






#endif	//LOG_HANDLER_H
