#ifndef USBPORTTESTER_H
#define USBPORTTESTER_H

#include <QString>

/** USB Host 功能测试结果（仅识别 U 盘）。 */
struct UsbHostTestResult {
    bool ok = false;
    QString summary; // 如 USB OK (sda)
    QString detail;  // 过程日志
};

/**
 * 开发板 USB Host 口功能检测：
 * 1) USB 控制器/根 Hub 是否存在
 * 2) 是否枚举到 USB 大容量存储块设备（/dev/sd*，且 removable=1）
 * 通过条件：仅当识别到 U 盘时成功；其它 USB 外设不算通过。
 */
class UsbPortTester
{
public:
    static UsbHostTestResult testHost();
};

#endif // USBPORTTESTER_H
