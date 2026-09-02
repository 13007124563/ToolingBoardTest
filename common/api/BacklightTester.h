#ifndef BACKLIGHTTESTER_H
#define BACKLIGHTTESTER_H

#include <QString>

/** 背光渐变测试结果。 */
struct BacklightTestResult {
    bool ok = false;
    QString summary;
    QString detail;
};

/**
 * 背光调节测试：向 lvds_backlight/brightness 写入 10→100 渐变（3 秒）。
 * 路径：/sys/class/backlight/lvds_backlight/brightness
 * 无命令行文本返回值；通过写入后读回 brightness 节点自动判定。
 */
class BacklightTester
{
public:
    static const char *kBrightnessPath;
    static const int kMinBrightness = 10;
    static const int kMaxBrightness = 100;
    static const int kGradientDurationMs = 3000;

    /** 3 秒内从 10 渐变到 100；端点写入读回一致则 ok=true。 */
    static BacklightTestResult runGradient();
};

#endif // BACKLIGHTTESTER_H
