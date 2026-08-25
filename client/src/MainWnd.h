#ifndef MAINWND_H
#define MAINWND_H

#include <QWidget>
#include <QLabel>
#include <QLineEdit>

// 包含CSerialCommandWrapper以获取类型定义（Windows和Linux都需要）
#include "CSerialCommandWrapper.h"
#include "SerialManager.h"
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

    QLineEdit* boardTestResultEdit(quint8 cmd) const;
    void clearBoardTestResultField(quint8 cmd);
    void updateBoardTestResultUi(const Protocol::Frame &frame);

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

protected slots:

    void lang_change();

    void event_user_confirm(QString info);

    void onScriptFinished(int exitCode, int exitStatus);
    void onScriptReadyRead();
    void onLogMonitorTimeout();


    void on_btn_nor_version_clicked();

    void on_btn_nor_sim_test_clicked();

    void on_btn_nor_iot_test_clicked();

    void on_btn_nor_all_test_clicked();

    void on_btn_nor_clear_clicked();
    void on_btn_nor_save_clicked();
    void on_btn_nor_record_clicked();

    void on_btn_nor_reconnect_clicked();
    void on_btn_nor_open_port_clicked();
    void on_btn_query_board_version_clicked();

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

    SerialManager m_serial;             // 主界面串口（QSerialPort，独立于 CSerialCommandWrapper）
    Protocol::ProtocolCodec m_codec;    // RS485 帧编解码（测试板协议）
    quint8 m_lastBoardQueryCmd = 0;     // 最近一次发送的治具查询命令

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
};

#endif // MAINWND_H
