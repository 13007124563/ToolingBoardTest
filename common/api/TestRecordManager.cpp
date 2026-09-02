#include "TestRecordManager.h"
#include "DBConnector.h"
#include <QDebug>

#include <QDateTime>

#define CURRENT_DATESTR QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")

#define CHECK_NEW_CODE_TIMES            5

using namespace zl;
TestRecordManager* zl::TestRecordManager::m_instance = nullptr;


TestRecordManager* zl::TestRecordManager::getInstance()
{
	if (!m_instance)
	{
        m_instance = new TestRecordManager();
	}
	return m_instance;
}

void zl::TestRecordManager::Release()
{
    qDebug() << "[WARNING]" << "release test record  manager...";
    delete m_instance;
    m_instance = nullptr;
}

TestRecordManager::TestRecordManager()
{

}

TestRecordManager::~TestRecordManager()
{

}

QString zl::TestRecordManager::GeneratedRecordID()
{
    // 生成流水号：时间戳 + 同毫秒序号，确保连续写入也唯一。
    static QString s_lastBase;
    static int s_sameMsSeq = 0;

    QDateTime now = QDateTime::currentDateTime();
    QString base = now.toString("yyyyMMddHHmmsszzz");
    if (base == s_lastBase) {
        ++s_sameMsSeq;
    } else {
        s_lastBase = base;
        s_sameMsSeq = 0;
    }

    QString recordId = QString("%1%2").arg(base).arg(s_sameMsSeq, 2, 10, QChar('0'));
    
    qDebug() << "[INFO] Generated Record ID:" << recordId;
    return recordId;
}

void zl::TestRecordManager::GetEmptyRecord(RecordInfo& record)
{
    qDebug() << "[INFO]" << "empty record";

    record.record_id = GeneratedRecordID();

    record.sim_card_type = ESelSimCardType_Unknown;
    record.ver_test = 0;
    record.sim_test = 0;
    record.iot_test = 0;
    record.simiot_test = 0;
    record.serial_test = 0;
    record.extra_test = 0;
    record.test_time = "";
    record.result_type = EResultType_Unknow;
    record.cmd_ret_info = "";
    record.result_info = "";
    record.test_log = "";

    record.module_type = "";
    record.iot_module_id = "";
    record.iot_imei = "";
    record.iccid = "";
    record.net_status = ESimNetStatus_Unknow;
    record.network_type = "";
    record.signal_strength = "";
    record.version = "";
}

int32_t zl::TestRecordManager::SaveTestRecord(RecordInfo& record)
{
    qDebug() << "[INFO]" << "save record: " << record.record_id;

    QSqlQuery query = zl::CDBConnector::Instance()->CreateQuery();
    query.prepare("INSERT INTO T_RECORD (RECORD_ID, SIM_CARD_TYPE, VER_TEST, SIM_TEST, IOT_TEST, SIMIOT_TEST, SERIAL_TEST, EXTRA_TEST, TEST_TIME, \
                    RESULT_TYPE, CMD_RET_INFO, RESULT_INFO, TEST_LOG, \
                    MODULE_TYPE, IOT_MODULE_ID, IOT_IMEI, ICCID, NET_STATUS, NETWORK_TYPE, SIGNAL_STRENGTH, VERSION) \
        VALUES(:record_id, :sim_card_type, :ver_test, :sim_test, :iot_test, :simiot_test, :serial_test, :extra_test, :test_time, \
                    :result_type, :cmd_ret_info, :result_info, :test_log, \
                    :module_type, :iot_module_id, :iot_imei, :iccid, :net_status, :network_type, :signal_strength, :version)");

    query.bindValue(":record_id", record.record_id);
    query.bindValue(":sim_card_type", record.sim_card_type);
    query.bindValue(":ver_test", record.ver_test);
    query.bindValue(":sim_test", record.sim_test);
    query.bindValue(":iot_test", record.iot_test);
    query.bindValue(":simiot_test", record.simiot_test);
    query.bindValue(":serial_test", record.serial_test);
    query.bindValue(":extra_test", record.extra_test);
    query.bindValue(":test_time", CURRENT_DATESTR);

    query.bindValue(":result_type", record.result_type);
    query.bindValue(":cmd_ret_info", record.cmd_ret_info);
    query.bindValue(":result_info", record.result_info);
    query.bindValue(":test_log", record.test_log);

    query.bindValue(":module_type", record.module_type);
    query.bindValue(":iot_module_id", record.iot_module_id);
    query.bindValue(":iot_imei", record.iot_imei);
    query.bindValue(":iccid", record.iccid);
    query.bindValue(":net_status", record.net_status);
    query.bindValue(":network_type", record.network_type);
    query.bindValue(":signal_strength", record.signal_strength);
    query.bindValue(":version", record.version);


    int32_t ret = zl::CDBConnector::Instance()->ExecQuery(query);
    if (ret != zl::CDBConnector::EDBError_Success)
    {
        qDebug() << "[ERROR]" << "save record failed: " << ret;
        return EResult_Failed;
    }

    record.test_time = CURRENT_DATESTR;

    qDebug() << "[INFO]" << "save record success: " << record.record_id;
    return EResult_Success;
}

int32_t zl::TestRecordManager::UpdateTestLog(const QString& record_id, const QString& test_log)
{
    qDebug() << "[INFO]" << "update test log for record: " << record_id << ", log size:" << test_log.size();

    if (record_id.isEmpty()) {
        qDebug() << "[ERROR]" << "update test log failed: record_id is empty";
        return EResult_Failed;
    }

    QSqlQuery query = zl::CDBConnector::Instance()->CreateQuery();
    query.prepare("UPDATE T_RECORD SET TEST_LOG = :test_log WHERE RECORD_ID = :record_id");
    
    query.bindValue(":test_log", test_log);
    query.bindValue(":record_id", record_id);

    int32_t ret = zl::CDBConnector::Instance()->ExecQuery(query);
    if (ret != zl::CDBConnector::EDBError_Success)
    {
        qDebug() << "[ERROR]" << "update test log failed: " << ret;
        return EResult_Failed;
    }

    qDebug() << "[INFO]" << "update test log success: " << record_id;
    return EResult_Success;
}

int32_t zl::TestRecordManager::RemoveTestRecords(const QStringList& record_id)
{
    qDebug() << "[INFO]" << "remove records, record id list size: " << record_id.size();

    if (record_id.isEmpty())
    {
        qDebug() << "[ERROR]" << "remove records error, record id list is empty";
        return EResult_Failed;
    }

    int list_size = record_id.size();

    QString tail;

    for (int i = 0; i < list_size; i++)
    {
        QString data = record_id.at(i);

        if (!data.isEmpty())
            tail += "?,";
        else
            qDebug() << "[ERROR]" << "remove records, one record id is empty";
    }

    tail.chop(1);        // 去掉多余的逗号

    QSqlQuery query = zl::CDBConnector::Instance()->CreateQuery();

    QString str = QString("DELETE FROM T_RECORD WHERE RECORD_ID IN(%1)").arg(tail);

    query.prepare(str);

    for (int i = 0; i < list_size; i++)
    {
        QString data = record_id.at(i);

        if (!data.isEmpty())
            query.bindValue(i, data);
    }

    int32_t ret = zl::CDBConnector::Instance()->ExecQuery(query);
    if (ret != zl::CDBConnector::EDBError_Success)
    {
        qDebug() << "[ERROR]" << "remove records failed: " << ret;
        return EResult_Failed;
    }

    qDebug() << "[INFO]" << "remove records success";
    return EResult_Success;
}

int32_t zl::TestRecordManager::SearchRecord(const QString& record_id)
{
    qDebug() << "[INFO]" << "search record info by record id: " << record_id;

    QSqlQuery query = zl::CDBConnector::Instance()->CreateQuery();
    query.prepare("SELECT * FROM T_RECORD WHERE RECORD_ID=:record_id");
    query.bindValue(":record_id", record_id);

    int32_t ret = zl::CDBConnector::Instance()->ExecQuery(query);
    if (ret != zl::CDBConnector::EDBError_Success || !query.next())
    {
        qDebug() << "[INFO]" << "record not found or query failed: " << ret;
        return EResult_Failed;
    }

    qDebug() << "[INFO]" << "search record info success: " << record_id;
    return EResult_Success;
}

int32_t zl::TestRecordManager::RemoveRecord(const QString& record_id)
{
    qDebug() << "[INFO]" << "remove record info by record id: " << record_id;

    QSqlQuery query = zl::CDBConnector::Instance()->CreateQuery();
    query.prepare("DELETE FROM T_RECORD WHERE RECORD_ID=:record_id");
    query.bindValue(":record_id", record_id);

    int32_t ret = zl::CDBConnector::Instance()->ExecQuery(query);
    if (ret != zl::CDBConnector::EDBError_Success)
    {
        qDebug() << "[INFO]" << "remove record failed: " << ret;
        return EResult_Failed;
    }

    qDebug() << "[INFO]" << "remove record info success: " << record_id;
    return EResult_Success;
}

int32_t zl::TestRecordManager::GetAllRecord(RecordVec& vec,
        const QString& begin_time,
        const QString& close_time,
        const ETestType test_type,
        const EResultType result_type,
        const ESimNetStatus net_status,
        const QString iot_module_id,
        const QString imei,
        const QString iccid,
        const QString version,
        const QString network_type)
{
    qDebug() << "[INFO]" << "query record";
    QSqlQuery query = zl::CDBConnector::Instance()->CreateQuery();
    QString str = "SELECT * FROM T_RECORD WHERE TEST_TIME>=:begin_time AND TEST_TIME<:close_time";

    if (test_type > ETestType_Unknow)
    {
        switch (test_type)
        {
        case ETestType_Sim:
        {
            str += " AND SIM_TEST=1";
        }
            break;
        case ETestType_Iot:
        {
            str += " AND IOT_TEST=1";
        }
            break;
        case ETestType_SimIot:
        {
            str += " AND SIMIOT_TEST=1";
        }
            break;
        case ETestType_RomVer:
        {
            str += " AND VER_TEST=1";
        }
            break;
        case ETestType_Serial:
        {
            str += " AND SERIAL_TEST=1";
        }
            break;
        case ETestType_Extra:
        {
            str += " AND EXTRA_TEST=1";
        }
            break;
        default:
            break;
        }
    }

    if (result_type > EResultType_Unknow) str += " AND RESULT_TYPE=:result_type";

    if (net_status > ESimNetStatus_Unknow) str += " AND NET_STATUS=:net_status";

    if (!iot_module_id.isEmpty()) str += " AND IOT_MODULE_ID=:iot_module_id";

    if (!imei.isEmpty()) str += " AND IOT_IMEI=:iot_imei";

    if (!iccid.isEmpty()) str += " AND ICCID=:iccid";

    if (!version.isEmpty()) str += " AND VERSION=:version";

    if (!network_type.isEmpty()) str += " AND NETWORK_TYPE LIKE :network_type";

    str += " ORDER BY TEST_TIME DESC";

    query.prepare(str);
    query.bindValue(":begin_time", begin_time);
    query.bindValue(":close_time", close_time);

    if (result_type > EResultType_Unknow) query.bindValue(":result_type", result_type);

    if (net_status > ESimNetStatus_Unknow) query.bindValue(":net_status", net_status);

    if (!iot_module_id.isEmpty()) query.bindValue(":iot_module_id", iot_module_id);

    if (!imei.isEmpty()) query.bindValue(":iot_imei", imei);

    if (!iccid.isEmpty()) query.bindValue(":iccid", iccid);

    if (!version.isEmpty()) query.bindValue(":version", version);

    if (!network_type.isEmpty()) query.bindValue(":network_type", network_type + "%");

    int32_t ret = zl::CDBConnector::Instance()->ExecQuery(query);
    if (ret != zl::CDBConnector::EDBError_Success)
    {
        qDebug() << "[ERROR]" << "get all record, exec query failed: " << ret;
        return EResult_Failed;
    }

    while (query.next())
    {
        RecordInfo record;

        record.record_id = query.value("RECORD_ID").toString();

        record.sim_card_type = ESelSimCardType(query.value("SIM_CARD_TYPE").toInt());
        record.ver_test = query.value("VER_TEST").toInt();
        record.sim_test = query.value("SIM_TEST").toInt();
        record.iot_test = query.value("IOT_TEST").toInt();
        record.simiot_test = query.value("SIMIOT_TEST").toInt();
        record.serial_test = query.value("SERIAL_TEST").toInt();
        {
            const QVariant extraVal = query.value("EXTRA_TEST");
            record.extra_test = extraVal.isValid() && !extraVal.isNull() ? extraVal.toInt() : 0;
        }
        record.test_time = query.value("TEST_TIME").toString();
        record.result_type = EResultType(query.value("RESULT_TYPE").toInt());
        record.cmd_ret_info = query.value("CMD_RET_INFO").toString();
        record.result_info = query.value("RESULT_INFO").toString();

        record.iot_module_id = query.value("IOT_MODULE_ID").toString();
        record.iot_imei = query.value("IOT_IMEI").toString();
        record.iccid = query.value("ICCID").toString();
        record.net_status = ESimNetStatus(query.value("NET_STATUS").toInt());
        record.version = query.value("VERSION").toString();
        
        // 读取测试日志和其他扩展字段
        record.test_log = query.value("TEST_LOG").toString();
        record.module_type = query.value("MODULE_TYPE").toString();
        record.network_type = query.value("NETWORK_TYPE").toString();
        record.signal_strength = query.value("SIGNAL_STRENGTH").toString();

        vec.push_back(record);
    }

    qDebug() << "[INFO]" << "get all records success: " << vec.size();
    return EResult_Success;
}

int32_t zl::TestRecordManager::GetComboCompleterInfo(QStringList& module_id_list, QStringList& imei_list, QStringList& iccid_list, QStringList& ver_list, const ETestType test_type)
{
    qDebug() << "[INFO]" << "get combo completer info: ";

    QSqlQuery query = zl::CDBConnector::Instance()->CreateQuery();

    QString str = "SELECT IOT_MODULE_ID, IOT_IMEI, ICCID, VERSION FROM T_RECORD";

    if (test_type > ETestType_Unknow)
    {
        auto pfconvert = [](ETestType v) -> QString {
            switch (v)
            {
            case zl::ETestType_Sim:         return tr("SIM_TEST");
            case zl::ETestType_Iot:         return tr("IOT_TEST");
            case zl::ETestType_SimIot:      return tr("SIMIOT_TEST");
            case zl::ETestType_RomVer:      return tr("VER_TEST");
            case zl::ETestType_Serial:      return tr("SERIAL_TEST");
            case zl::ETestType_Extra:       return tr("EXTRA_TEST");

            default: return "";
            }
        };

        QString select_type = pfconvert(test_type);

        if (select_type.isEmpty())
        {
            qDebug() << "[ERROR]" << "get combo completer info error, selected test type: " << test_type;
            return EResult_Failed;
        }

        str += QString(" WHERE %1=1").arg(select_type);
    }

    query.prepare(str);

    int32_t ret = zl::CDBConnector::Instance()->ExecQuery(query);
    if (ret != zl::CDBConnector::EDBError_Success)
    {
        qDebug() << "[INFO]" << "get combo completer info, exec query failed: " << ret;
        return EResult_Failed;
    }

    while (query.next())
    {
        QString iot_module_id = query.value("IOT_MODULE_ID").toString();
        QString iot_imei = query.value("IOT_IMEI").toString();
        QString sim_iccid = query.value("ICCID").toString();
        QString version = query.value("VERSION").toString();

        if (!module_id_list.contains(iot_module_id) && !iot_module_id.isEmpty()) module_id_list.append(iot_module_id);

        if (!imei_list.contains(iot_imei) && !iot_imei.isEmpty()) imei_list.append(iot_imei);

        if (!iccid_list.contains(sim_iccid) && !sim_iccid.isEmpty()) iccid_list.append(sim_iccid);

        if (!ver_list.contains(version) && !version.isEmpty()) ver_list.append(version);
    }

    qDebug() << "[INFO]" << "get combo completer info success: ";
    return EResult_Success;
}
