/****************************************************************************
**
** @file ISerialCommand.h
** @brief 斯普瑞扫描器接口
** @author xiongpengchao@chinawebox.com
** @version 1.0.0
** @date 2019-10-30
**
****************************************************************************/

#ifndef I_SERIAL_COMMAND_H_
#define I_SERIAL_COMMAND_H_

#include <iostream>

namespace zl
{
namespace zlmodules
{

#define MIN_LEN             7               // 指令最小长度

#define SIM_ICCID_LEN       20              // sim测试结果返回 iccid信息长度
#define SIM_NET_LEN         1               // sim测试结果返回 网络状态信息长度
#define SIM_NETWORK_TYPE_LEN 1              // sim测试结果返回 网络制式信息长度
#define SIM_RSSI_LEN        1               // sim测试结果返回 信号强度信息长度
#define SIM_BER_LEN         1               // sim测试结果返回 误码率信息长度
#define IOT_MODULE_LEN      15              // IOT测试结果返回 IOT版本信息长度
#define IOT_IMEI_LEN        15              // IOT测试结果返回 IOT模块imei信息长度

#define IOT_INFO_LEN        (IOT_MODULE_LEN+IOT_IMEI_LEN)                           // IOT测试结果返回 IOT信息长度
#define SIM_INFO_LEN        (SIM_ICCID_LEN+SIM_NET_LEN+SIM_NETWORK_TYPE_LEN+SIM_RSSI_LEN+SIM_BER_LEN)  // sim测试结果返回 sim信息长度 24字节



#define CMD_SIM             0x01            // sim测试指令号
#define CMD_IOT             0x02            // iot测试指令号
#define CMD_TOTAL           0x03            // iot&sim测试指令号
#define CMD_VERSION         0x04            // 获取版本号指令号

#define RET_ERROR_FLAG      0x81            // 下位机接收指令 校验错误返回 标记
#define RET_BUSY_FLAG       0x00            // 下位机接收指令 当前正在忙无法处理 标记
#define RET_VALID_FLAG      0x01            // 下位机接收指令 当前空闲立刻处理 标记

#define CMD_HEAD_FLAG       0xAA            // 通讯指令固定头 标记
#define MSG_HEAD_FLAG       0xBB            // 下位机主动回复测试结果指令固定头 标记
#define END_FLAG            0x55            // 通讯指令固定尾 标记

#define ESEYE_TYPE          0x00            // 当前sim卡类型为Eseye卡
#define ONE_NICE_TYPE       0x01            // 当前sim卡类型为1Nice卡

#define NET_FAIL            0x00            // sim卡测试结果 网络失败
#define NET_PASS            0x01            // sim卡测试结果 网络成功

typedef enum
{
    Crt_Unknown = -1,       // 默认 未知
    Crt_Success = 0,        // 成功
    Crt_Check_fail,         // 下发指令校验失败
    Crt_Command_error,      // 回复指令号错误
    Crt_Length_error,       // 回复长度错误
    Crt_Bcc_error,          // 回复校验错误
    Crt_State_error,        // 回复状态错误
    Crt_Machine_busy,       // 回复下位机忙
    Crt_Index_error,        // 回复的指令序号错误

} CommandResultType;

typedef enum
{
    Sct_Unknown = -1,       // 默认 未知
    Sct_Eseye,              // Eseye
    Sct_1Nice,              // 1Nice
} SimCardType;

typedef enum
{
    Net_GSM = 0,            // GSM (2G网络)
    Net_UMTS = 2,           // UMTS (3G网络)
    Net_LTE = 3,            // LTE (4G网络)
    Net_Unknown = -1,       // 未知网络制式
} NetworkType;


/// \brief SerialCommand事件回调接口
class ISerialCommandEventNotice
{
public:
    ISerialCommandEventNotice() = default;
    virtual ~ISerialCommandEventNotice() = default;

    /// \brief 指令回应结果回调事件
    virtual void CommandResult(const unsigned char& cmd, CommandResultType type, const unsigned char* data, size_t len) {}

    /// \brief 指令 回应信息 回调事件
    virtual void MsgData(const unsigned char& cmd, const unsigned char* data, size_t len) {}
};

/// \brief SerialCommand串口指令接口
class ISerialCommand
{
public:
    virtual ~ISerialCommand() = default;

    /// \brief 连接到设备
    virtual int32_t Connect(const std::string& port, int32_t baud) = 0;

    /// \brief 断开连接
    virtual void Disconnect() = 0;

    /// \brief 测试sim卡指令
    virtual int32_t SimTestCommand(const std::string& apn_config) = 0;

    /// \brief 测试iot模块指令
    virtual int32_t IotTestCommand() = 0;

    /// \brief 测试iot模块和sim卡指令
    virtual int32_t TotalTestCommand(const std::string& apn_config) = 0;

    /// \brief 获取固件版本号指令
    virtual int32_t GetVersionCommand() = 0;
};

} // namespace zlmodules
} // namespace zl

#endif // I_SERIAL_COMMAND_H_
