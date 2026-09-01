#ifndef USBPORTTESTER_H
#define USBPORTTESTER_H

#include <QString>

/** USB Host 功能测试结果（插 U 盘/外设后检测枚举与存储）。 */
struct UsbHostTestResult {
    bool ok = false;
    QString summary; // 如 USB OK (1 device, sdX)
    QString detail;  // 过程日志
};

/**
 * 开发板 USB Host 口功能检测：
 * 1) USB 控制器/根 Hub 是否存在
 * 2) 是否枚举到至少一个外设（非根 Hub）
 * 3) 若存在 USB 块设备（/dev/sd*）则记入结果（推荐插 U 盘）
 * 通过条件：控制器正常且至少有 1 个外设。
 */
class UsbPortTester
{
public:
    static UsbHostTestResult testHost();
};

#endif // USBPORTTESTER_H
