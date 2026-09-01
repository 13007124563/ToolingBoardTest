#ifndef TFCARDTESTER_H
#define TFCARDTESTER_H

#include <QString>

/** TF/SD 卡功能测试结果（必须识别到 SD 类型 mmc 块设备才算通过）。 */
struct TfCardTestResult {
    bool ok = false;
    QString summary; // 如 TF OK (mmcblk1)
    QString detail;  // 过程日志
};

/**
 * 开发板 TF 卡座功能检测：
 * 扫描 /sys/block/mmcblk*，要求 device/type == SD（排除 eMMC 的 MMC）。
 * 通过条件：至少存在 1 个 SD 类型块设备。
 */
class TfCardTester
{
public:
    static TfCardTestResult testCard();
};

#endif // TFCARDTESTER_H
