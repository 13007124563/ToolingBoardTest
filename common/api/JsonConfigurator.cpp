#include "JsonConfigurator.h"
#include <QMutex>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>

using namespace zl;

QMutex _GMutex;

int32_t CJsonConfigurator::Load(
	zl::AppConfig& config, 
    const QString & filepath)
{
	QMutexLocker locker(&_GMutex);
    QFile file(filepath);
	if (!file.open(QIODevice::ReadOnly))
	{
		return EResult_OpenFileFailed;
	}

	QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
	file.close();

	if (!doc.isObject())
	{
		return EResult_ParseJsonFailed;
	}

	// get config_
	QJsonObject obj = doc.object();

	QJsonObject cabobj = obj["cabinet"].toObject();
    QJsonObject	serialobj = obj["serial_info"].toObject();
	
    config.cabinet.name = cabobj["name"].toString();
    config.cabinet.title = cabobj["title"].toString();
    config.cabinet.cabinet_no = cabobj["cabinet_no"].toString();
    QString lang_value = cabobj["language"].toString();

    // 严格按照配置文件设置，不做默认假设
    if (lang_value.compare("Cn") == 0)
        config.cabinet.language = ELanguageType_Cn;
    else if (lang_value.compare("En") == 0)
        config.cabinet.language = ELanguageType_En;

    config.serial_config.port = serialobj["serial_port_name"].toString();
    config.serial_config.baud_rate = serialobj["baudrate"].toString();

	return EResult_Success;
}

int32_t CJsonConfigurator::Save(
	const zl::AppConfig& config,
    const QString & filepath)
{
	QMutexLocker locker(&_GMutex);
    QFile file(filepath);
	if (!file.open(QIODevice::WriteOnly))
	{
		return EResult_OpenFileFailed;
	}

    QJsonObject cabobj, serialobj, obj;

    cabobj["name"] = config.cabinet.name;
    cabobj["title"] = config.cabinet.title;
    cabobj["cabinet_no"] = config.cabinet.cabinet_no;

    QString lang_value = "Cn";

    if (config.cabinet.language == ELanguageType_Cn)
        lang_value = "Cn";
    else if (config.cabinet.language == ELanguageType_En)
        lang_value = "En";

    cabobj["language"] = lang_value;

    serialobj["serial_port_name"] = config.serial_config.port;
    serialobj["baudrate"] = config.serial_config.baud_rate;

	obj["cabinet"] = cabobj;	
    obj["serial_info"] = serialobj;

	if (file.write(QJsonDocument(obj).toJson()) < 0)
	{
		file.close();
		return EResult_WriteFileFailed;
	}
	file.close();

	return EResult_Success;
}
