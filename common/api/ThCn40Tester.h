#ifndef THCN40TESTER_H
#define THCN40TESTER_H

#include <QString>
#include <QtGlobal>

/** CN40 温湿度传感器读取结果。 */
struct ThCn40TestResult {
    bool ok = false;
    double temperatureC = 0.0;
    double humidityPct = 0.0;
    QString summary; // 如 T:25.3°C H:60.5%
    QString detail;  // 过程日志
};

/**
 * CN40 温湿度检测：执行 /etc/zl_test/stm32_i2c_test 0x4 读取传感器。
 * 温湿度传感器接 CN40（见测试文档 10.3）。
 */
class ThCn40Tester
{
public:
    static const quint8 kI2cCmd = 0x04;

    static ThCn40TestResult readSensor();
};

#endif // THCN40TESTER_H
