#ifndef USBPORTTESTER_H
#define USBPORTTESTER_H

#include <QString>

/** USB Host 功能测试结果（双口均需识别到 U 盘）。 */
struct UsbHostTestResult {
    bool ok = false;
    QString summary; // 如 USB OK (2 ports: sda@usb1/1-1, sdb@usb1/1-2)
    QString detail;  // 过程日志
};

/**
 * 开发板 USB Host 口功能检测：
 * 1) USB 控制器/根 Hub 是否存在
 * 2) 板载 2 个 USB 口均需插入 U 盘（removable=1 的 USB 块设备，且分属不同物理口）
 * 通过条件：至少 2 个不同物理 USB 口各检测到 1 个 U 盘；只插 1 个口算失败。
 */
class UsbPortTester
{
public:
    static const int kRequiredUsbPorts = 2;
    static UsbHostTestResult testHost();
};

#endif // USBPORTTESTER_H
