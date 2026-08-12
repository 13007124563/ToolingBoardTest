#ifndef COMMONDEFINE_H
#define COMMONDEFINE_H

#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QDebug>

#ifdef Q_OS_ANDROID

#define WRITE_DIR               "/storage/sdcard0/ZhilaiLocker/"

#define PATH_LOG_FILE           "/storage/sdcard0/ZhilaiLocker/logs/"
#define PATH_WAV_FILE           "/storage/sdcard0/ZhilaiLocker/wav/"
#define CONFIG_PATH             "/storage/sdcard0/ZhilaiLocker/config.json"
#define INPUT_DB_PATH           "/storage/sdcard0/ZhilaiLocker/py.db"
#define DB_PATH                 "/storage/sdcard0/ZhilaiLocker/ZhilaiLocker.db"

#else

#define WRITE_DIR               ""

#define PATH_LOG_FILE           "logs/"
#define PATH_WAV_FILE           "./wav/"
#define CONFIG_PATH             "./config.json"
#define INPUT_DB_PATH           "./py.db"
#define DB_PATH                 "./ToolingBoardTest.db"

#endif


#ifdef QT_NO_DEBUG
    #ifdef NO_INFO_WARNING
        #define SHOW_INFO_WARNING       false
    #else
        #define SHOW_INFO_WARNING       true
    #endif
#else
    #define SHOW_INFO_WARNING           true
#endif // _DEBUG

/*
static void CopyAndroidAssetsFile(QString file_name)
{
    QString write_path = WRITE_DIR;
    if (write_path.isEmpty()) return;

    QByteArray data;
    QFile fileSrc("assets:/WanxinagLocker/" + file_name);
    if (fileSrc.open(QFile::ReadOnly))
    {
        data = fileSrc.readAll();
        fileSrc.close();
    }
    else
    {
        qDebug() << "[ERROR]"<< "assets copy src file open failed " << fileSrc.fileName() << fileSrc.errorString();
        return ;
    }

    QDir dir;
    if (!dir.exists(write_path)) dir.mkpath(write_path);

    QString file_path = write_path + file_name;
    QFile fileDest(file_path);
    if (fileDest.open(QFile::WriteOnly))
    {
        fileDest.write(data);
        fileDest.close();
    }
    else
    {
        qDebug()  << "[ERROR]"<< "assets copy dest file open failed" << fileDest.fileName() << fileDest.errorString();
        return ;
    }

    qDebug()  << "[NEED]"<< "copy android assets file success.name: " << file_name;
}

static void CheckAndCopy(QString file_name)
{
    QString write_path = WRITE_DIR;
    if (write_path.isEmpty()) return;

    QString file_path = write_path + file_name;

    QFile file(file_path);
    if (file.exists()) return;

    CopyAndroidAssetsFile(file_name);
}
*/







#endif // COMMONDEFINE_H
