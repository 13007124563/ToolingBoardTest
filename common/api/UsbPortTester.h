#ifndef USBPORTTESTER_H
#define USBPORTTESTER_H

#include <QString>

/** USB Host 功能测试结果（双口均需识别到 U 盘）。 */
struct UsbHostTestResult {
    bool ok = false;
    QString summary; // 如 USB OK (2 ports: ...)
    QString detail;  // 过程日志
};

/**
 * 开发板 USB Host 口功能检测：
 * 1) USB 控制器/根 Hub 是否存在
 * 2) 板载 2 个 USB 口均需插入 U 盘
 *
 * U 盘判定（满足其一即可）：
 * - USB Mass Storage 接口（bInterfaceClass=08）
 * - 或 USB 总线上的可移动块设备（/dev/sd* 且 removable=1）
 *
 * 物理口键按板载口计数（如 usb1/1-1.1、usb1/1-1.2）。
 * 外部分线/Hub 更深层级会归并回板载口，避免单口一分二误判为双口通过。
 */
class UsbPortTester
{
public:
    static const int kRequiredUsbPorts = 2;
    static UsbHostTestResult testHost();
};

#endif // USBPORTTESTER_H
