#include "AppModel.h"
#include "JsonConfigurator.h"

#include "CommonType.h"

#include <QObject>
#include <QDate>
#include <QFileInfo>
#include <QCoreApplication>
#include <QDebug>

// 版本号日期部分根据编译日期自动生成：YYMMDD
static QString buildVersionString()
{
    QDate d = QDate::fromString(__DATE__, "MMM dd yyyy");
    if (!d.isValid())
        d = QDate::fromString(QString(__DATE__).simplified(), "MMM d yyyy");
    QString datePart = d.toString("yyMMdd");
    QString version = QString("1.0.1.%1").arg(datePart);
    qDebug() << "[INFO] App version built from compile date:" << version << "(__DATE__ =" << __DATE__ << ")";
    return version;
}

// 优先使用可执行文件时间生成日期版本，避免增量编译时 __DATE__ 不更新。
static QString buildVersionFromExecutableTimestamp()
{
    const QString exePath = QCoreApplication::applicationFilePath();
    if (exePath.isEmpty()) {
        return QString();
    }

    QFileInfo exeInfo(exePath);
    if (!exeInfo.exists()) {
        return QString();
    }

    const QDate buildDate = exeInfo.lastModified().date();
    if (!buildDate.isValid()) {
        return QString();
    }

    const QString datePart = buildDate.toString("yyMMdd");
    return QString("1.0.1.%1").arg(datePart);
}

QString CAppModel::Version_ = buildVersionString();
CAppModel* CAppModel::Instance_ = new CAppModel;
CAppModel* CAppModel::Instance()
{
	return Instance_;
}

int32_t CAppModel::LoadAppConfiguration()
{
	zl::AppConfig config;

    const QString runtimeVersion = buildVersionFromExecutableTimestamp();
    if (!runtimeVersion.isEmpty()) {
        Version_ = runtimeVersion;
        qDebug() << "[INFO] App version refreshed from executable timestamp:" << Version_;
    }

    config.cabinet.name = "ToolingBoardTest";
    // Force set the title to ensure it matches the requirement: EN="IOT Test Tool", CN="IOT模块测试工具"
    config.cabinet.title = QObject::tr("IOT Test Tool");
    config.cabinet.cabinet_no = "0";

	int32_t ret = zl::CJsonConfigurator::Load(config);
    
    // Ensure title persists even if config file has different value
    config.cabinet.title = QObject::tr("IOT Test Tool");

	if (ret == zl::EResult_Success)
	{
		config_ = config;
	}

	return ret;
}

int32_t CAppModel::SaveAppConfiguration()
{
	auto ret = zl::CJsonConfigurator::Save(config_);
	return ret;
}
