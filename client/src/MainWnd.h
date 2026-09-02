#ifndef MAINWND_H
#define MAINWND_H

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QDateTime>

// 包含CSerialCommandWrapper以获取类型定义（Windows和Linux都需要）
#include "CSerialCommandWrapper.h"
#include "SerialManager.h"
#include "Rs232PortTester.h"
#include "protocol/protocolframe.h"

#include "CommonType.h"

class QPushButton;
class QProcess;

namespace Ui {
class MainWnd;
}

class MainWnd : public QWidget
{
    Q_OBJECT

public:
    explicit MainWnd(QWidget *parent = 0);
    ~MainWnd();

    // 传入串口读取到的数据
    void SerialCommandResult(const unsigned char cmd, zl::zlmodules::CommandResultType type, QByteArray& data);

    // 传入串口读取到的结果信息
    void SerialMsgInfo(const unsigned char cmd, QByteArray& data);

    // 传入配置参数
    void SetCabinetInfo(const QString& version);

    // 传入串口连接信息
    void SetConnectMsg(const QString& info);

    // 同步串口下拉与打开状态（配置变更/重连后调用）
    void SyncSerialPortUi();
protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

    void disableComboBoxWheelSelection();

    void resetCmdResultInfo();

    void resetVersionInfo();

    void resetBoardTestResults();
    void resetExtraTestResults();
    void runExtraTests();
    void runCanPortCn27Test();
    void runRs232Cn35Cn36Test();
    void runRs232Cn37Cn38Test();
    void runUsbPortTest();
    void runTfCardTest();
    void runThCn40Test();
    void runBacklightTest();
    void applyRs232CrossTalkResult(QLineEdit *edit, const QString &logTag,
                                   const Rs232CrossTalkResult &r);

    QLineEdit* boardTestResultEdit(quint8 cmd) const;
    void clearBoardTestResultField(quint8 cmd);
    void updateBoardTestResultUi(const Protocol::Frame &frame);
    void saveSerialTestRecord(quint8 cmd, const QString &summary, const QString &detailLog,
                              zl::EResultType resultType = zl::EResultType_Success);

    void resetSimInfo();

    void resetIotInfo();

    void showCmdExcutedResult(zl::zlmodules::CommandResultType type);

    void showReceivedMsg(QString info);



    void showSimInfo(QByteArray& data);
    void showIotInfo(QByteArray& data);
    void showSimIotInfo(QByteArray& data);
    void showVersionInfo(QByteArray& data);

    void resetRecord();

    void auto_save_record();

    // 更新APN和NET输入框的默认值
    void updateDefaultValues();

    // 设置输入框的启用/禁用状态
    void setInputsEnabled(bool enabled);

    // 设置按钮执行状态的颜色
    void setButtonExecuting(QPushButton* button, bool executing);

    // 串口选择与开关
    void initSerialPortUi();
    void initBoardCommandCombo();
    void refreshSerialPorts();
    void updateOpenPortButton();
    bool openSelectedSerialPort();
    bool ensureSerialPortOpen();
    void closeSerialPort();
    bool sendBuiltFrame(quint8 cmd, const QByteArray &info = QByteArray());
    void sendSelectedBoardQuery();

    // 获取平台相关路径
    QString getLogFilePath() const;
    QString getDevInfoPath() const;
    QString getScriptCommand() const;

#ifdef Q_OS_WIN
    // Windows测试模式：创建模拟测试数据
    void createTestDataForWindows();
#endif

    // IOT脚本执行相关方法
    void executeIotScript(const QString& moduleName, const QString& apn, const QString& net);
    void readScriptResults();
    void displayScriptLog();
    QString readFileContent(const QString& filePath);
    void initModuleTypeComboBox();
    void cleanupOldLogFiles();          // 清理超过指定月数的日志文件
    void startLogMonitoring();
    void stopLogMonitoring();
    void monitorLogFile();
    void parseLogLineAndUpdateUI(const QString& line);
    void addApnToHistory(const QString& apn);
    void addNetToHistory(const QString& net);
    void loadApnNetHistory();
    void saveTestRecordToDatabase();
    void updateTestLogToDatabase(bool showHint = false, bool forceUpdate = false); // 更新测试记录日志（包含拨号和监控）
    void checkTestCompletion(const QString& output);  // 新增：检查测试完成
    void showTestingOverlay(bool show);              // 新增：显示/隐藏测试中遮罩
    void doClearTestResult();                        // 新增：实际执行清空逻辑
    void setLabelFailed(QLabel* label);              // 设置 QLabel 为失败样式
    void setLabelFailed(QLineEdit* edit);            // 重载：设置 QLineEdit 为失败样式

    void proceedOneClickAfterIot();
    void startOneClickBoardQueries();
    void proceedOneClickNextQueryOrFinish();
    void finishOneClickTest();

    enum class DualVoltageTestPhase {
        None,
        WaitLow,
        WaitHigh,
    };
    bool isDualVoltageBoardCmd(quint8 cmd) const;
    bool dualVoltageTestConfig(quint8 boardCmd, quint8 &i2cCmd, QString &logTag) const;
    QString dualVoltageI2cCommandLine(quint8 i2cCmd, quint8 relayArg) const;
    bool runStm32I2cRelayCommand(quint8 i2cCmd, quint8 relayArg, QString *output = nullptr);
    bool startDualVoltageQuery(quint8 boardCmd);
    bool sendDualVoltageSerialQuery();
    void handleDualVoltageSerialResponse(const Protocol::Frame &frame, const QString &parsedText);
    void finalizeDualVoltageTest();
    void failDualVoltageTest(const QString &reason);
    void restoreDualVoltageRelayToLow();
    QString makeSerialExchangeDetailLog(const QString &stepLabel, const QString &parsedContent,
                                        const QString &rxContent = QString()) const;

    enum class StInputIoTestPhase {
        None,
        WaitHighSerial,
        WaitLowSerial,
    };
    bool isStInputIoBoardCmd(quint8 cmd) const;
    QString stInputIoI2cCommandLine(quint8 i2cCmd) const;
    bool runStm32I2cGetInputCommand(quint8 i2cCmd, int &inputValue, QString *output = nullptr,
                                    int simExpectedLevel = -1);
    bool verifyStInputIoI2cInputs(int expectedLevel, QString &summaryOut, QString &detailSectionOut);
    bool startStInputIoQuery();
    bool sendStInputIoSerialQuery(quint8 outputLevel);
    void handleStInputIoSerialResponse(const Protocol::Frame &frame, const QString &parsedText);
    void finalizeStInputIoTest();
    void failStInputIoTest(const QString &reason);

protected slots:

    void lang_change();

    void event_user_confirm(QString info);

    void onScriptFinished(int exitCode, int exitStatus);
    void onScriptReadyRead();
    void onLogMonitorTimeout();


    void on_btn_nor_one_click_test_clicked();

    void on_btn_nor_version_clicked();

    void on_btn_nor_sim_test_clicked();

    void on_btn_nor_iot_test_clicked();

    void on_btn_nor_all_test_clicked();

    void on_btn_nor_clear_clicked();
    void on_btn_nor_save_clicked();
    void on_btn_nor_record_clicked();

    void on_btn_nor_reconnect_clicked();
    void on_btn_query_board_version_clicked();
    void on_btn_nor_extra_test_clicked();

    void onSerialPortOpened(const QString &portName);
    void onSerialPortClosed();
    void onSerialError(const QString &message);
    void onSerialFrameSent(const QByteArray &frame);
    void onSerialFrameReceived(const Protocol::Frame &frame, const QString &parsedText);
    void onSerialPassiveFrameReceived(const Protocol::Frame &frame, const QString &reason);
    void onSerialOperationTimeout();

private:
    Ui::MainWnd *ui;

    zl::RecordInfo record_;

    QString connect_msg_;               // 记录串口连接信息

    SerialManager m_serial;             // 主界面串口（治具协议，独立于 RS232 互发）
    Rs232PortTester m_rs232Serial;      // RS232 互发专用（CN35/36、CN37/38），不复用 m_serial
    Protocol::ProtocolCodec m_codec;    // RS485 帧编解码（测试板协议）
    quint8 m_lastBoardQueryCmd = 0;     // 最近一次发送的治具查询命令
    QString m_lastBoardTxHex;           // 最近一次发送的治具报文（hex）
    QDateTime m_lastBoardTxTime;        // 最近一次发送治具报文时间

    // 跟踪当前执行中的按钮
    QPushButton* m_currentExecutingButton;

    // IOT脚本执行相关成员
    QProcess* m_scriptProcess;          // 脚本执行进程
    QString m_scriptPath;               // 脚本路径
    QTimer* m_logMonitorTimer;          // 日志监控定时器
    qint64 m_lastLogFileSize;           // 上次读取的日志文件大小
    QTimer* m_timeoutTimer;             // 新增：超时定时器
    bool m_testCompleted;               // 新增：测试完成标志
    bool m_isManuallyTerminating;       // 新增：标志是否正在主动终止进程
    QString m_savedSimRecordId;         // 新增：已保存的SIM卡测试记录ID（用于日志更新）
    QString m_savedIotRecordId;         // 新增：已保存的IOT测试记录ID（用于日志更新）
    qint64 m_lastSavedLogSize;          // 最近一次已同步到数据库的日志大小
    qint64 m_lastLogSyncTimestampMs;    // 最近一次日志同步时间戳（ms）
    QWidget* m_testingOverlay;          // 测试执行中遮罩
    QLabel*  m_spinIconLabel;            // 旋转图标
    QLabel*  m_spinTextLabel;            // 旋转文字（双语更新）
    QTimer*  m_spinTimer;               // 旋转定时器
    int      m_spinAngle;               // 当前旋转角度
    bool     m_oneClickTestActive;      // 一键测试流程进行中
    bool     m_oneClickTestAwaitingQuery; // 一键测试等待串口查询回包
    int      m_oneClickQueryIndex;      // 一键测试当前查询指令索引

    DualVoltageTestPhase m_dualVoltagePhase = DualVoltageTestPhase::None;
    quint8 m_dualVoltageBoardCmd = 0;
    quint8 m_dualVoltageI2cCmd = 0;
    QString m_dualVoltageLogTag;
    QString m_dualVoltageTestDetailLog;
    QString m_dualVoltageLowSummary;
    QString m_dualVoltageHighSummary;
    bool m_dualVoltageLowOk = false;
    bool m_dualVoltageHighOk = false;
    bool m_dualVoltageLowReadingValid = false;
    bool m_dualVoltageHighReadingValid = false;

    StInputIoTestPhase m_stInputIoPhase = StInputIoTestPhase::None;
    QString m_stInputIoLogTag;
    QString m_stInputIoTestDetailLog;
    QString m_stInputIoHighSummary;
    QString m_stInputIoLowSummary;
    bool m_stInputIoHighOk = false;
    bool m_stInputIoLowOk = false;
    bool m_stInputIoHighValid = false;
    bool m_stInputIoLowValid = false;
};

#endif // MAINWND_H
