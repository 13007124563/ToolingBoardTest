#ifndef CSUPERLEADWRAPPER_H_
#define CSUPERLEADWRAPPER_H_

#include <QObject>

#ifdef Q_OS_WIN
// Windows平台：使用串口通信
#include "SerialCommand/ISerialCommand.h"
#include "SerialCommand/CSerialCommandControllor.h"

class CSerialCommandWrapper :
        public QObject,
        public zl::zlmodules::ISerialCommandEventNotice
{
    Q_OBJECT
public:
    CSerialCommandWrapper(QObject* parent = Q_NULLPTR) : QObject(parent) {
        impl_ = new zl::zlmodules::CSerialCommandControllor(this);
    }
    ~CSerialCommandWrapper() {
//        impl_->Disconnect();
        if (impl_ != nullptr)
        {
            delete impl_;
            impl_ = nullptr;
        }
    }

    inline void CommandResult(const unsigned char& cmd, zl::zlmodules::CommandResultType type, const unsigned char* data, size_t len) override
    { emit commandResult(cmd, type, QByteArray((const char*)data, len)); }


    virtual void MsgData(const unsigned char& cmd, const unsigned char* data, size_t len) override
    {
        emit msgData(cmd, QByteArray((const char*)data, len));
    }

public slots:
    inline qint32 connect(const QString& port, qint32 baud)
    {
        if (impl_ == nullptr)
        {
            impl_ = new zl::zlmodules::CSerialCommandControllor(this);
        }

        return impl_->Connect(port.toStdString(), baud);
    }

    inline void disconnect()
    {
//        impl_->Disconnect();
        if (impl_ == nullptr)
        {
            delete impl_;
            impl_= nullptr;
        }
    }


    inline qint32 SimTestCommand(const QString& apn_config) { return impl_->SimTestCommand(apn_config.toStdString()); }

    inline qint32 IotTestCommand() { return impl_->IotTestCommand(); }

    inline qint32 TotalTestCommand(const QString& apn_config) { return impl_->TotalTestCommand(apn_config.toStdString()); }

    inline qint32 GetVersionCommand() { return impl_->GetVersionCommand(); }
signals:

    void commandResult(const unsigned char cmd, zl::zlmodules::CommandResultType type, QByteArray data);

    void msgData(const unsigned char cmd, QByteArray data);
private:
    zl::zlmodules::ISerialCommand* impl_;

};

#else
// Linux平台：不使用串口，提供空实现
// 为了兼容性，定义所有需要的常量和枚举类型

// 协议常量定义
#define SIM_ICCID_LEN       20
#define SIM_NET_LEN         1
#define SIM_NETWORK_TYPE_LEN 1
#define SIM_RSSI_LEN        1
#define SIM_BER_LEN         1
#define IOT_MODULE_LEN      15
#define IOT_IMEI_LEN        15
#define IOT_INFO_LEN        (IOT_MODULE_LEN+IOT_IMEI_LEN)
#define SIM_INFO_LEN        (SIM_ICCID_LEN+SIM_NET_LEN+SIM_NETWORK_TYPE_LEN+SIM_RSSI_LEN+SIM_BER_LEN)

// 命令常量定义
#define CMD_SIM             0x01
#define CMD_IOT             0x02
#define CMD_TOTAL           0x03
#define CMD_VERSION         0x04

// 网络状态定义
#define NET_FAIL            0x00
#define NET_PASS            0x01

namespace zl {
namespace zlmodules {
    // 命令结果类型枚举
    enum CommandResultType {
        Crt_Unknown = -1,
        Crt_Success = 0,
        Crt_Check_fail,
        Crt_Command_error,
        Crt_Length_error,
        Crt_Bcc_error,
        Crt_State_error,
        Crt_Machine_busy,
        Crt_Index_error
    };
}
}

class CSerialCommandWrapper : public QObject
{
    Q_OBJECT
public:
    CSerialCommandWrapper(QObject* parent = Q_NULLPTR) : QObject(parent) {}
    ~CSerialCommandWrapper() {}

public slots:
    inline qint32 connect(const QString& /*port*/, qint32 /*baud*/) { return -1; }
    inline void disconnect() {}
    inline qint32 SimTestCommand(const QString& /*apn_config*/) { return -1; }
    inline qint32 IotTestCommand() { return -1; }
    inline qint32 TotalTestCommand(const QString& /*apn_config*/) { return -1; }
    inline qint32 GetVersionCommand() { return -1; }

signals:
    // 注意：这里使用 CommandResultType 类型以保持与MainApp的兼容性
    void commandResult(const unsigned char cmd, zl::zlmodules::CommandResultType type, QByteArray data);
    void msgData(const unsigned char cmd, QByteArray data);
};
#endif // Q_OS_WIN

#endif // CSUPERLEADWRAPPER_H_
