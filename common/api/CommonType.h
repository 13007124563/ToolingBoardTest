#ifndef COMMONTYPE_H_
#define COMMONTYPE_H_

#include <QString>
#include <QStringList>

namespace zl {

/* language */
enum ELanguageType {
    ELanguageType_Cn               = 0,
    ELanguageType_En               = 1
};

// 通讯结果 1 成功 2 失败 3 超时
enum EMsgSendResultType {
    EMsgSendResultType_Success  = 0,
    EMsgSendResultType_Fail     = 1,
    EMsgSendResultType_TimeOut  = 2
};

/* erro code */
enum EResult {
    EResult_Success,
    EResult_Failed,

    EResult_OpenFileFailed,
    EResult_WriteFileFailed,
    EResult_ParseJsonFailed,

    EResult_OrderNotFound,

    EResult_CabinetNotInitialized,
    EResult_InvalidLockerNo,
    EResult_InvalidCard,

    EResult_Unknown					= 0xFF
};

enum EWF {
        EWF_Home = 0,                   // 展示 首页

        EWF_Unknow
};

enum EBP {
        EBP_Select_page = 0,            // 展示 后台选择界面
        EBP_Record_page,                // 展示 记录查询界面
        EBP_Advance_page,               // 展示 高级设计界面

        EBP_Unknow
};

enum ECardStatus {
    ECardStatus_Normal				= 0x01,
    ECardStatus_Locked				= 0x02,

    ECardStatus_Error				= 0xFF
};

enum EMgrCardType {
    EMgrCardType_Error				= -1,

    EMgrCardType_Normal				= 0,
    EMgrCardType_Advance			= 1,
    EMgrCardType_Super				= 2,
    EMgrCardType_Root				= 3
};

/* test type */
enum ETestType {
    ETestType_Unknow             = -1,

    ETestType_Sim                = 0,
    ETestType_Iot                = 1,
    ETestType_SimIot             = 2,
    ETestType_RomVer             = 3,
    ETestType_Serial             = 4,
    ETestType_Extra              = 5   // 新增测试（RS232/USB/TF/温湿度/背光等）
};

/* result type */ // corresponding with CommandResultType
enum EResultType {
    EResultType_Unknow             = -1,

    EResultType_Success = 0,        // 成功
    EResultType_Check_fail,         // 下发指令校验失败
    EResultType_Command_error,      // 回复指令号错误
    EResultType_Length_error,       // 回复长度错误
    EResultType_Bcc_error,          // 回复校验错误

    EResultType_State_error,        // 回复状态错误
    EResultType_Machine_busy,       // 回复下位机忙
    EResultType_Index_error,        // 回复的指令序号错误
};


// 网络制式类型，对应协议字段，便于switch分支和UI显示
enum ENetType {
    ENetType_Unknown = -1,      // 未知
    ENetType_None    = 0,       // 无服务
    ENetType_2G      = 2,       // 2G
    ENetType_4G      = 3,       // 4G
    ENetType_CAT1    = 7,       // CAT1
    ENetType_CAT4    = 8,       // CAT4
    ENetType_CATM1   = 9,       // CAT-M1
    ENetType_NoService = 0xFF   // 无服务
};

// corresponding with SimCardType
enum ESelSimCardType
{
    ESelSimCardType_Unknown = -1,       // 默认 未知
    ESelSimCardType_Eseye,              // Eseye
    ESelSimCardType_1Nice,              // 1Nice
};

/* sim net status */
enum ESimNetStatus {
    ESimNetStatus_Unknow             = -1,

    ESimNetStatus_Fail = 0,         // 联网失败
    ESimNetStatus_Success,          // 联网成功
};

/* Configuration */

// cabinet info
struct CabinetInfo
{
    QString                         title;
    QString                         name;
    QString                         cabinet_no;
    ELanguageType                   language;

    CabinetInfo()
    {
        title = "";
        name = "";
        cabinet_no = "";
        // language由配置文件设置，不在构造函数中初始化
    }
};

// serial port info
struct SerialInfo
{
    QString                         port;               // 串口号
    QString                         baud_rate;          // 波特率

    SerialInfo()
    {
        port = "";
        baud_rate = "";
    }
};

// history record
typedef struct {
    int32_t                         log_file_month;
    int32_t                         order_record_month;
    int32_t                         operate_record_month;
}HistoryRecordConfig;

// db config
typedef struct {
    int32_t							port;
    int32_t							type;
    QString 						addr;
    QString 						user;
    QString 						pwd;
    QString 						name;
}DatabaseConfig;

// record info
struct RecordInfo
{
    QString                         record_id;          // 流水号：年月日时分秒毫秒（20250126165800001）

    ESelSimCardType                 sim_card_type;
    ETestType                       test_type;
    int32_t                         ver_test;
    int32_t                         sim_test;
    int32_t                         iot_test;
    int32_t                         simiot_test;
    int32_t                         serial_test;
    int32_t                         extra_test;         // 新增测试标记
    QString                         test_time;          // 测试时间
    EResultType                     result_type;
    QString                         cmd_ret_info;       // 指令回应信息
    QString                         result_info;        // 测试结果信息
    QString                         test_log;           // 完整的测试日志（从/var/log/iot_start.log读取）

    QString                         module_type;        // 模块类型（ME3630/EC200U/EG21/BG95）
    QString                         iot_module_id;      // IOT版本
    QString                         iot_imei;           // IOT IMEI
    QString                         iccid;              // SIM卡ICCID
    ESimNetStatus                   net_status;         // 网络状态
    QString                         network_type;       // 网络制式（GSM/LTE/UMTS等）
    QString                         signal_strength;    // 信号强度（-85dBm等）
    QString                         version;            // 镜像版本

    RecordInfo()
    {
        record_id = "";

        sim_card_type = ESelSimCardType_Unknown;

        ver_test = 0;
        sim_test = 0;
        iot_test = 0;
        simiot_test = 0;
        serial_test = 0;
        extra_test = 0;

        test_time = "";
        result_type = EResultType_Unknow;
        cmd_ret_info = "";
        result_info = "";
        test_log = "";

        module_type = "";
        iot_module_id = "";
        iot_imei = "";
        iccid = "";
        net_status = ESimNetStatus_Unknow;
        network_type = "";
        signal_strength = "";
        version = "";
    }
};
typedef QVector<RecordInfo> RecordVec;

typedef struct {
    CabinetInfo						cabinet;
    SerialInfo                      serial_config;
}AppConfig;


// manager card
typedef struct ManagerCard {
    QString 						cardno;
    int32_t							status;
    int32_t                         type;
}ManagerCard;
typedef QVector<ManagerCard> ManagerCardVec;


// order
typedef struct Order {
    QString                         order_id;
    int32_t							status;

    QString                         send_time;          // 投递时间
    QString                         receive_time;       // 取件时间

    QString                         sender_account;     // 投递账号 // 当前存件人user_id //***终端软件中user_id实际采用login_name字段
    QString                         receiver_account;   // 接收账号 // 当前取件人登录名

    int32_t                         box_no;             // 投递箱号
    QString                         bussiness_id;       // 服务器业务编号
    int32_t                         confirmed;          // 是否上报成功

}Order;
typedef QVector<Order> OrderVec;


}


//  非零开头正整数
#define REG_EXP_BOX_NO      "^[1-9]*[1-9][0-9]*$"

// 10位或11位数字
#define REG_EXP_ACCOUNT     "^[0-9]{9,10}[0-9]$"

// 6位到10位数字 用于后台账号
#define REG_EXP_MANAGER     "^[0-9]{5,9}[0-9]$"

// 1位或2位数字 用于设置保存日志记录的月份
#define REG_EXP_RECORD      "^[1-9]{0,1}[0-9]$"

// 1位或4位数字 用于设置以秒位单位超时时长
#define REG_EXP_SECONDS     "^[1-9][0-9]{0,3}$"

// 1位或4位数字 用于设置人脸识别区域
#define REG_EXP_FACE_POS    "^[0-9][0-9]{0,3}$"

// 6位数字
#define REG_EXP_PWD         "^[0-9]{5}[0-9]$"

// 纯英文字母
#define REG_EXP_SHORT_NAME  "^[A-Za-z]+$"

// 取件码 英文字母或数字
//#define REG_EXP_PIN_CODE    "^[A-Za-z0-9]{8}$"
#define REG_EXP_PIN_CODE    "^[0-9]{8}$"

// 手机号码 符合中国手机号段
//#define REG_EXP_PHONE       "^(13[0-9]|14[5-9]|15[0-3,5-9]|16[6]|17[2-8]|18[0-9]|19[1|3|8|9])[0-9]{8}$"
#define REG_EXP_PHONE       "^[1]([3-9])[0-9]{9}$"

//
#define DEL_STR_BLANK       QRegExp("\\s")

// 日志文件保存最小月数
#define MINI_MONTHS_LOG_FILE            1

// 后台操作记录保存最小月数
#define MINI_MONTHS_OPER_RECORD         3

// 订单保存最小月数
#define MINI_MONTHS_ORDER               3

// 等待扫描时间 单位：秒
#define SCAN_TIME   30

// 等待请求服务器时间 单位：秒
#define REQ_TIME    30

#endif // COMMONTYPE_H_
