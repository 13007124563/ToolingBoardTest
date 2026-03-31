#ifndef TEST_RECORD_MANAGER_H
#define TEST_RECORD_MANAGER_H

#include <QtCore/QObject>

#include <QVector>
#include "CommonType.h"

namespace zl {


class TestRecordManager : public QObject
{
public:
    static TestRecordManager* getInstance();
    static void Release();

    // 创建默认测试记录
    void GetEmptyRecord(RecordInfo& record);

    // 保存测试记录
    int32_t SaveTestRecord(RecordInfo& record);

    // 更新测试记录的日志（用于追加拨号和监控日志）
    int32_t UpdateTestLog(const QString& record_id, const QString& test_log);

    // 删除选中的记录
    int32_t RemoveTestRecords(const QStringList& record_id);

    // 在数据库中查找是否已经存过
    int32_t SearchRecord(const QString& record_id);

    // 在数据库中删除指定记录
    int32_t RemoveRecord(const QString& record_id);

    // 按照条件 查询测试记录信息
    int32_t GetAllRecord(
        RecordVec& vec,
        const QString& begin_time,
        const QString& close_time,
        const ETestType test_type = ETestType_Unknow,
        const EResultType result_type = EResultType_Unknow,
        const ESimNetStatus net_status = ESimNetStatus_Unknow,
        const QString iot_module_id = "",
        const QString imei = "",
        const QString iccid = "",
        const QString version = "",
        const QString network_type = "");  // 新增：网络类型筛选

    int32_t GetComboCompleterInfo(QStringList& module_id_list, QStringList& imei_list, QStringList& iccid_list, QStringList& ver_list, const ETestType test_type = ETestType_Unknow);

    QString GeneratedRecordID();

private:   

    TestRecordManager();
    TestRecordManager(const TestRecordManager &);
    TestRecordManager& operator = (const TestRecordManager &);

    ~TestRecordManager();
    
private:
    static TestRecordManager* m_instance;

};

} // namespace end

#endif // TEST_RECORD_MANAGER_H
