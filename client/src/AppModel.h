#ifndef CAPPMODEL_H_
#define CAPPMODEL_H_

#include "CommonType.h"
#include <QObject>

#define APPMODEL()	(CAppModel::Instance())

class CAppModel
{
public:
	static CAppModel* Instance();
	
	int32_t LoadAppConfiguration();
	int32_t SaveAppConfiguration();

//    inline QString Version() { return QString("测试专用，不可实际运营使用"); }
//    inline QString CabinetName() { return QString("测试专用，不可实际运营使用"); }
    inline QString Version() { return Version_; }
    inline QString CabinetName() { return config_.cabinet.name; }
    inline QString CabinetTitle() { return config_.cabinet.title; }
    inline QString CabinetId() { return config_.cabinet.cabinet_no; }

    inline void SetCabinetName(QString name) { config_.cabinet.name = name; }
    inline void SetCabinetTitle(QString title) { config_.cabinet.title = title; }
    inline void SetCabinetId(QString id) { config_.cabinet.cabinet_no = id; }

    inline void SetCabinetLanguage(zl::ELanguageType type) { config_.cabinet.language = type; }
    inline zl::ELanguageType CabinetLanguage() { return config_.cabinet.language; }

	inline zl::AppConfig Config() { return config_; }
	inline zl::CabinetInfo CabinetInfo() { return config_.cabinet; }

	inline void SetConfig(const zl::AppConfig& config) { config_ = config; }

	inline void UpdateCabinetInfo(const zl::CabinetInfo& cabinfo) { config_.cabinet = cabinfo; }

protected:
	CAppModel() = default;
	~CAppModel() = default;

private:
	static CAppModel*	Instance_;
    static QString  	Version_;
	zl::AppConfig		config_;
};

#endif // CAPPMODEL_H_
