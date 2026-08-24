#include "MainWnd.h"
#include "ui_MainWnd.h"

#include "MainApp.h"
#include "MsgWnd.h"

#include "GlobalSignal.h"

#include <QDebug>
#include <QTimer>
#include <QApplication>

#include <QLineEdit>
#include <QRegularExpression>
#include <QRegularExpressionValidator>

#include "TestRecordManager.h"
#include "AppModel.h"
#include "commondefine.h"
#include "protocol/protocolframe.h"

#include <QProcess>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QCoreApplication>
#include <QTimer>
#include <QDateTime>
#include <QPainter>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QScrollArea>
#include <QSizePolicy>

// 测试执行中半透明遮罩小工具类
class TestProgressWidget : public QWidget {
public:
    explicit TestProgressWidget(QWidget* parent) : QWidget(parent) {
        setObjectName("testProgressBg");
    }
protected:
    void paintEvent(QPaintEvent*) override {
        QPainter p(this);
        p.fillRect(rect(), QColor(0, 0, 0, 100));
    }
    void mousePressEvent(QMouseEvent*) override {} // 屏蔽鼠标点击
};

MainWnd::MainWnd(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWnd)
  , connect_msg_("")
  , m_currentExecutingButton(nullptr)
  , m_scriptProcess(nullptr)
  , m_logMonitorTimer(nullptr)
  , m_lastLogFileSize(0)
  , m_timeoutTimer(nullptr)
  , m_testCompleted(false)
  , m_isManuallyTerminating(false)
  , m_savedSimRecordId("")
  , m_savedIotRecordId("")
  , m_lastSavedLogSize(0)
  , m_lastLogSyncTimestampMs(0)
  , m_testingOverlay(nullptr)
  , m_spinIconLabel(nullptr)
  , m_spinTextLabel(nullptr)
  , m_spinTimer(nullptr)
  , m_spinAngle(0)
{

    ui->setupUi(this);

    // 右侧布局：蓝色滚动区拉伸占满剩余空间，绿色过程区保持固定高度
    if (ui->verticalLayout_Right) {
        ui->verticalLayout_Right->setContentsMargins(20, 10, 20, 20);
        ui->verticalLayout_Right->setStretch(0, 1);  // scrollArea_Info
        ui->verticalLayout_Right->setStretch(1, 0);  // wnd_process_fixed
    }

    // 滚动区表格式布局：左 Label 固定宽度，右 QLineEdit 拉伸
    if (ui->gridLayout_Info) {
        ui->gridLayout_Info->setVerticalSpacing(8);
        ui->gridLayout_Info->setHorizontalSpacing(12);
        ui->gridLayout_Info->setContentsMargins(0, 0, 8, 0);
        ui->gridLayout_Info->setColumnStretch(0, 0);
        ui->gridLayout_Info->setColumnStretch(1, 1);
    }
    if (ui->scrollAreaWidgetContents) {
        for (QLabel *lb : ui->scrollAreaWidgetContents->findChildren<QLabel *>()) {
            lb->setContentsMargins(0, 0, 0, 0);
            lb->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
            lb->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
            lb->setMinimumWidth(140);
        }
        for (QLineEdit *edit : ui->scrollAreaWidgetContents->findChildren<QLineEdit *>()) {
            edit->setContentsMargins(0, 0, 0, 0);
            edit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        }
    }

    // QScrollArea 默认 minimumSizeHint 会跟随内容高度，
    // 垂直策略设为 Ignored 后，由 stretch 决定可视高度，内容超出即可滚动。
    if (ui->scrollArea_Info) {
        ui->scrollArea_Info->setMinimumHeight(0);
        ui->scrollArea_Info->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Ignored);
        ui->scrollArea_Info->setWidgetResizable(true);
    }
    if (ui->scrollArea_Left) {
        ui->scrollArea_Left->setMinimumHeight(0);
        ui->scrollArea_Left->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        ui->scrollArea_Left->setWidgetResizable(true);
    }

    // 初始化脚本路径（与可执行文件同级目录）
    m_scriptPath = QCoreApplication::applicationDirPath() + "/iot_start.sh";

    // 初始化模块类型下拉框
    initModuleTypeComboBox();

    // 初始化串口/波特率选择
    initSerialPortUi();

    // 加载APN和NET历史记录
    loadApnNetHistory();

    // 初始化日志监控定时器
    m_logMonitorTimer = new QTimer(this);
    connect(m_logMonitorTimer, &QTimer::timeout, this, &MainWnd::onLogMonitorTimeout);

    // 清理超过1个月的旧日志文件
    cleanupOldLogFiles();

    // 设置APN输入验证器（字母、数字、下划线、点、短横线，长度1-64）
    QRegularExpression apnRegExp("^[A-Za-z0-9_.-]{1,64}$");
    QRegularExpressionValidator* apnValidator = new QRegularExpressionValidator(apnRegExp, this);
    bool isCnInit = (APPMODEL()->CabinetLanguage() == zl::ELanguageType_Cn);
    QString placeholderInit = isCnInit ? "请输入或选择" : "Enter or select";

    if (ui->le_apn->lineEdit()) {
        ui->le_apn->lineEdit()->setValidator(apnValidator);
        ui->le_apn->lineEdit()->setMaxLength(64);
        ui->le_apn->lineEdit()->setPlaceholderText(placeholderInit);
    }

    // 设置NET输入验证器（合法域名或IP地址，长度7-253）
    // 支持域名：www.baidu.com、aws.amazon.com
    // 支持IP：8.8.8.8、114.114.114.114
    QRegularExpression netRegExp("^([A-Za-z0-9]([A-Za-z0-9-]{0,61}[A-Za-z0-9])?\\.)+[A-Za-z]{2,}$|^((25[0-5]|(2[0-4]|1\\d|[1-9]|)\\d)\\.?\\b){4}$");
    QRegularExpressionValidator* netValidator = new QRegularExpressionValidator(netRegExp, this);
    if (ui->le_net->lineEdit()) {
        ui->le_net->lineEdit()->setValidator(netValidator);
        ui->le_net->lineEdit()->setMaxLength(253);
        ui->le_net->lineEdit()->setPlaceholderText(placeholderInit);
    }

    // 可编辑下拉框需在 lineEdit 就绪后强制写入默认显示文本
    {
        const int cmnetIndex = ui->le_apn->findText(QStringLiteral("cmnet"));
        if (cmnetIndex >= 0) {
            ui->le_apn->setCurrentIndex(cmnetIndex);
            ui->le_apn->setEditText(QStringLiteral("cmnet"));
        }
        if (ui->le_net->count() > 0) {
            ui->le_net->setCurrentIndex(0);
            ui->le_net->setEditText(ui->le_net->itemText(0));
        }
    }

    // ui->btn_nor_clear->hide();
    // ui->btn_nor_save->hide();

    resetVersionInfo();
    resetSimInfo();
    resetIotInfo();
    resetCmdResultInfo();

    // 初始化默认值
    updateDefaultValues();

    QObject::connect(GlobalSignal::getInstance(), &GlobalSignal::switchLanguage, this, &MainWnd::lang_change);

    QObject::connect(GlobalSignal::getInstance(), &GlobalSignal::user_confirm, this, &MainWnd::event_user_confirm);
}

MainWnd::~MainWnd()
{
    QObject::disconnect(GlobalSignal::getInstance(), &GlobalSignal::switchLanguage, this, &MainWnd::lang_change);

    QObject::disconnect(GlobalSignal::getInstance(), &GlobalSignal::user_confirm, this, &MainWnd::event_user_confirm);

    // 停止日志监控
    stopLogMonitoring();

    // 清理脚本进程
    if (m_scriptProcess) {
        if (m_scriptProcess->state() == QProcess::Running) {
            m_scriptProcess->kill();
            m_scriptProcess->waitForFinished();
        }
        delete m_scriptProcess;
        m_scriptProcess = nullptr;
    }

    // 清理定时器
    if (m_logMonitorTimer) {
        delete m_logMonitorTimer;
        m_logMonitorTimer = nullptr;
    }

    if (m_timeoutTimer) {
        delete m_timeoutTimer;
        m_timeoutTimer = nullptr;
    }

    delete ui;
}

void MainWnd::lang_change()
{
    qDebug() << "[DEBUG] lang_change() called";

    if (ui) ui->retranslateUi(this);

    SetCabinetInfo(APPMODEL()->Version());

    SetConnectMsg(connect_msg_);
    updateOpenPortButton();

    // 更新 APN/NET 占位符文本（双语言）
    bool isCn = (APPMODEL()->CabinetLanguage() == zl::ELanguageType_Cn);
    QString editPlaceholder = isCn ? "请输入或选择" : "Enter or select";
    if (ui->le_apn->lineEdit()) ui->le_apn->lineEdit()->setPlaceholderText(editPlaceholder);
    if (ui->le_net->lineEdit()) ui->le_net->lineEdit()->setPlaceholderText(editPlaceholder);
    // 更新测试执行中遮罩文字
    if (m_spinTextLabel)
        m_spinTextLabel->setText(isCn ? "测试执行中~" : "Testing~");

    // 延迟更新默认值，确保语言状态已经更新
    QTimer::singleShot(50, this, [this](){
        qDebug() << "[DEBUG] Delayed updateDefaultValues() called";
        updateDefaultValues();
    });
}

void MainWnd::event_user_confirm(QString info)
{
    if (info.isEmpty()) return;

    // 处理清空测试结果确认
    if (info == "CLEAR_TEST_RESULT") {
        doClearTestResult();
        return;
    }
}

void MainWnd::SetCabinetInfo(const QString& /*version*/)
{
    // Version label moved to Banner
    // QString version_ = QString("Ver:%1").arg(version);
    // ui->lb_version->setText(version_);
}

void MainWnd::SetConnectMsg(const QString& info)
{
    connect_msg_ = info;
    // ui->lb_connect_msg->setText(info);
    updateOpenPortButton();
}

void MainWnd::SyncSerialPortUi()
{
    refreshSerialPorts();
    updateOpenPortButton();
}

void MainWnd::resetCmdResultInfo()
{
    // ui->lb_test_cmd_excute_result->clear();
    ui->lb_test_cmd_excute_return_msg->clear();
}

void MainWnd::resetVersionInfo()
{
    ui->lb_test_version->clear();
    ui->lb_test_version->setStyleSheet("");
}

void MainWnd::resetSimInfo()
{
    ui->lb_test_sim_network->clear();
    ui->lb_test_iccid->clear();
    
    // 清空新增的网络信息显示控件
    ui->lb_test_network_type->clear();
    ui->lb_test_rssi->clear();
    
    ui->lb_test_sim_network->setStyleSheet("");
    ui->lb_test_iccid->setStyleSheet("");
    ui->lb_test_network_type->setStyleSheet("");
    ui->lb_test_rssi->setStyleSheet("");
}

void MainWnd::resetIotInfo()
{
    ui->lb_test_iot_module_ver->clear();
    ui->lb_test_iot_imei->clear();
    ui->lb_test_iot_module_ver->setStyleSheet("");
    ui->lb_test_iot_imei->setStyleSheet("");
}

void MainWnd::SerialCommandResult(const unsigned char cmd, zl::zlmodules::CommandResultType type, QByteArray& data)
{
    showCmdExcutedResult(type);

    showReceivedMsg(data.toHex(' '));

    record_.result_type = EResultType(int32_t(type));
    record_.cmd_ret_info = data;

    QString info(""), info1("");

    if (record_.result_type == EResultType_Success)
    {
        if (cmd == CMD_SIM || cmd == CMD_TOTAL)
        {
            info = tr("Sim card test needs few minutes");
            info1 = tr("please wait");
        }
        else if (cmd == CMD_IOT)
        {
            info = tr("IOT test needs few seconds");
            info1 = tr("please wait");
        }
    }
    else if (record_.result_type == EResultType_Machine_busy)
    {
        info = tr("Tooling board is busy");
        info1 = tr("please wait");
    }

    if (!info.isEmpty() && !info1.isEmpty())
    {
        MsgWnd::ShowNormalInfo(info, info1);
    }
    else if (!info.isEmpty())
    {
        MsgWnd::ShowNormalInfo(info);
    }
}

void MainWnd::SerialMsgInfo(const unsigned char cmd, QByteArray& data)
{
    if (cmd == CMD_SIM)
    {
        record_.sim_test = 1;
        showSimInfo(data);
    }
    else if (cmd == CMD_IOT)
    {
        record_.iot_test = 1;
        showIotInfo(data);
    }
    else if (cmd == CMD_TOTAL)
    {
        record_.simiot_test = 1;
        showSimIotInfo(data);
    }
    else if (cmd == CMD_VERSION)
    {
        showVersionInfo(data);
    }
    else
    {
        qDebug() << "[ERROR]" << "receive serial msg error, cmd error: " << QByteArray(cmd, 1).toHex() << " msg data: " << data.toHex();
        return;
    }

    showReceivedMsg(data.toHex(' '));
    record_.result_info = data;

    if (cmd == CMD_IOT)
        auto_save_record();

    // 测试结果已收到，恢复按钮状态
    if (m_currentExecutingButton != nullptr)
    {
        qDebug() << "[DEBUG] Test completed, restoring button:" << m_currentExecutingButton->objectName();
        setInputsEnabled(true);
        setButtonExecuting(m_currentExecutingButton, false);
        m_currentExecutingButton = nullptr;
    }
}

void MainWnd::showCmdExcutedResult(zl::zlmodules::CommandResultType type)
{
    auto pfconvert = [](zl::zlmodules::CommandResultType v)-> QString {
        switch (v)
        {
        case zl::zlmodules::Crt_Unknown: return tr("Unknown");
        case zl::zlmodules::Crt_Success: return tr("Correct");
        case zl::zlmodules::Crt_Check_fail: return tr("Send cmd BCC error");
        case zl::zlmodules::Crt_Command_error: return tr("Ret cmd id error");
        case zl::zlmodules::Crt_Length_error: return tr("Ret len error");
        case zl::zlmodules::Crt_Bcc_error: return tr("Ret BCC error");
        case zl::zlmodules::Crt_State_error: return tr("Ret state error");
        case zl::zlmodules::Crt_Machine_busy: return tr("Ret busy");
        case zl::zlmodules::Crt_Index_error: return tr("Ret cmd index error");
        default: return tr("Unknown");
        }
    };

    // ui->lb_test_cmd_excute_result->setText(pfconvert(type));
}

void MainWnd::showReceivedMsg(QString info)
{
    ui->lb_test_cmd_excute_return_msg->setPlainText(info);
}

void MainWnd::showSimInfo(QByteArray& data)
{

    // 新固件协议去掉了BER（误码率），长度变为23字节
    if (data.size() != SIM_INFO_LEN && data.size() != SIM_INFO_LEN - 1) {
        qDebug() << "[ERROR]" << "receive sim info error, data size: " << data.size() << " msg data: " << data.toHex();
        return;
    }

    bool all_zero(true);

    for (int i = 0; i < SIM_ICCID_LEN; i++)
    {
        if (data.at(i) != 0x00)
        {
            all_zero = false;
            break;
        }
    }

    QString iccid(""), network(""), network_type(""), rssi_info("");
    ESimNetStatus sim_net_state = ESimNetStatus_Unknow;

    char net_state = data.at(SIM_ICCID_LEN);                    // 第21字节：联网状态
    char net_type = data.at(SIM_ICCID_LEN + 1);                 // 第22字节：网络制式
    qint8 rssi = static_cast<qint8>(data.at(SIM_ICCID_LEN + 2));  // 第23字节：信号强度（dBm，带符号）

    // 解析ICCID
    if (all_zero)
        iccid = data.left(SIM_ICCID_LEN).toHex(' ');
    else
        iccid = data.left(SIM_ICCID_LEN);

    // 解析联网状态
    if (net_state == NET_FAIL)
    {
        network = tr("Failed");
        sim_net_state = ESimNetStatus_Fail;
    }
    else if (net_state == NET_PASS)
    {
        network = tr("Pass");
        sim_net_state = ESimNetStatus_Success;
    }
    else
    {
        network = tr("Unknown");
        qDebug() << "[ERROR]" << "receive sim info error, network status: " << QByteArray(net_state, 1).toHex() << " msg data: " << data.toHex();
    }

    // 解析网络制式
    switch (net_type)
    {
    case 0:
        network_type = tr("GSM (2G)");
        break;
    case 2:
        network_type = tr("UMTS (3G)");
        break;
    case 3:
        network_type = tr("LTE (4G)");
        break;
    case 7:
        network_type = tr("CAT1");
        break;
    case 8:
        network_type = tr("CAT4");
        break;
    case 9:
        network_type = tr("CAT-M1");
        break;
    case (char)0xFF:
        network_type = tr("无服务");
        break;
    default:
        network_type = tr("Unknown");
        qDebug() << "[ERROR]" << "receive sim info error, network type: " << QByteArray(&net_type, 1).toHex();
        break;
    }

    // 直接显示信号强度（dBm）
    rssi_info = QString::number(rssi) + " dBm";


    // 更新界面显示
    ui->lb_test_sim_network->setText(network);
    ui->lb_test_iccid->setText(iccid);
    
    // 更新新增的显    [2025-08-11 10:54:06 D 19820] [SEND][SIM] "AA 01 1E 41 50 4E 3A 63 6D 6E 65 74 2D 4E 45 54 3A 77 77 77 2E 71 71 2E 63 6F 6D 01 FE 55"    [2025-08-11 10:54:06 D 19820] [SEND][SIM] "AA 01 1E 41 50 4E 3A 63 6D 6E 65 74 2D 4E 45 54 3A 77 77 77 2E 71 71 2E 63 6F 6D 01 FE 55"
    ui->lb_test_network_type->setText(network_type);
    ui->lb_test_rssi->setText(rssi_info);

    // 更新记录
    record_.net_status = sim_net_state;
    record_.iccid = iccid;

    QString info1, info2, info3;

    if (all_zero)
    {
        info1 = tr("Please Input ICCID");
        info2 = tr("Query iccid failed!");
        info3 = "";
    }
    else
    {
        info1 = tr("Please Check ICCID");
        info2 = QString(tr("Iccid is: %1")).arg(iccid);
        info3 = iccid;
    }

    MsgWnd::ConfirmInfo(info1, info2, info3);
}

void MainWnd::showIotInfo(QByteArray& data)
{
    const int32_t msg_len = IOT_INFO_LEN;

    if (data.size() != msg_len || (IOT_MODULE_LEN + IOT_IMEI_LEN) != IOT_INFO_LEN)
    {
        qDebug() << "[ERROR]" << "receive iot info error, data size: " << data.size() << " msg data: " << data.toHex();
        return;
    }

    bool zero_module(true), zero_imei(true);

    for (int i = 0; i < IOT_MODULE_LEN; i++)
    {
        if (data.at(i) != 0x00)
        {
            zero_module = false;
            break;
        }
    }

    for (int i = IOT_MODULE_LEN; i < IOT_INFO_LEN; i++)
    {
        if (data.at(i) != 0x00)
        {
            zero_imei = false;
            break;
        }
    }

    QString module_info(""), imei_info("");

    if (zero_module)
        module_info = data.left(IOT_MODULE_LEN).toHex(' ');
    else
        module_info = data.left(IOT_MODULE_LEN);

    if (zero_imei)
        imei_info = data.right(IOT_IMEI_LEN).toHex(' ');
    else
        imei_info = data.right(IOT_IMEI_LEN);

    ui->lb_test_iot_module_ver->setText(module_info);
    ui->lb_test_iot_imei->setText(imei_info);

    record_.iot_module_id = module_info;
    record_.iot_imei = imei_info;
}

void MainWnd::showSimIotInfo(QByteArray& data)
{
    // 新协议去掉BER，SIM部分为23字节
    const int32_t msg_len_new = IOT_INFO_LEN + SIM_INFO_LEN - 1;
    if (data.size() != IOT_INFO_LEN + SIM_INFO_LEN && data.size() != msg_len_new) {
        qDebug() << "[ERROR]" << "receive iot&sim info error, data size: " << data.size() << " msg data: " << data.toHex();
        return;
    }
    QByteArray iot_data = data.left(IOT_INFO_LEN);
    QByteArray sim_data = data.right(data.size() - IOT_INFO_LEN);
    showIotInfo(iot_data);
    showSimInfo(sim_data);
}

void MainWnd::showVersionInfo(QByteArray& data)
{
    if (data.size()  < 1)
    {
        qDebug() << "[ERROR]" << "receive version error, data size: " << data.size();
        return;
    }

    ui->lb_test_version->setText(data);

    record_.version = data;
}

void MainWnd::resetRecord()
{
    TestRecordManager* instance = TestRecordManager::getInstance();

    if (instance == Q_NULLPTR)
    {
        qDebug() << "[ERROR]" << "reset record failed, test record mgr is null";
        MsgWnd::ShowNormalInfo(tr("Something error"));
        return;
    }

    instance->GetEmptyRecord(record_);

    record_.version = ui->lb_test_version->text();
}

void MainWnd::on_btn_nor_version_clicked()
{
    resetVersionInfo();
    resetCmdResultInfo();

    // 禁用输入框和改变按钮颜色
    setInputsEnabled(false);
    setButtonExecuting(ui->btn_nor_version, true);

    // 记录当前执行的按钮
    m_currentExecutingButton = ui->btn_nor_version;

#ifdef Q_OS_WIN
    // Windows测试模式：显示模拟数据
    ui->lb_test_version->setText("1.0.0-test");
    MsgWnd::ShowNormalInfo(tr("Windows test mode: Version query simulated"));
    setInputsEnabled(true);
    setButtonExecuting(ui->btn_nor_version, false);
    m_currentExecutingButton = nullptr;
#else
    ui->lb_test_cmd_excute_return_msg->appendPlainText("$ cat /etc/sw-version");

    QString output;
    QFile swFile("/etc/sw-version");
    if (swFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        output = QString::fromUtf8(swFile.readAll());
        swFile.close();
    } else {
        qDebug() << "[WARN] Failed to open /etc/sw-version directly, fallback to /bin/cat";
        QProcess process;
        process.start("/bin/cat", QStringList() << "/etc/sw-version");
        if (process.waitForFinished(3000) && process.exitCode() == 0) {
            output = QString::fromUtf8(process.readAllStandardOutput());
        } else {
            QString err = QString::fromUtf8(process.readAllStandardError()).trimmed();
            ui->lb_test_version->setText("Error");
            ui->lb_test_cmd_excute_return_msg->appendPlainText(
                tr("Failed to query image version: %1").arg(err.isEmpty() ? tr("unknown error") : err));
            qDebug() << "[ERROR] Failed to read /etc/sw-version, stderr:" << err;
            MsgWnd::ShowNormalInfo(tr("Failed to query image version"));

            setInputsEnabled(true);
            setButtonExecuting(ui->btn_nor_version, false);
            m_currentExecutingButton = nullptr;
            return;
        }
    }

    QString trimmedOutput = output.trimmed();
    if (!trimmedOutput.isEmpty()) {
        ui->lb_test_cmd_excute_return_msg->appendPlainText(trimmedOutput);
    } else {
        ui->lb_test_cmd_excute_return_msg->appendPlainText(tr("No output from /etc/sw-version"));
    }

    QString version = "unknown";
    
    // 添加调试信息
    qDebug() << "[DEBUG] Raw output:" << output;
    
    // 直接查找以 V 开头的版本号（如 V1.0.1.260210）
    QRegularExpression re("(V[0-9]+\\.[0-9]+\\.[0-9]+\\.[0-9]+)", QRegularExpression::CaseInsensitiveOption);
    QRegularExpressionMatch match = re.match(output);
    if (match.hasMatch()) {
        version = match.captured(1);
        qDebug() << "[INFO] Version matched:" << version;
    } else {
        // 备用：查找 V 后跟任意非空白字符
        QRegularExpression re2("(V[^\\s\\n\\r]+)", QRegularExpression::CaseInsensitiveOption);
        QRegularExpressionMatch match2 = re2.match(output);
        if (match2.hasMatch()) {
            version = match2.captured(1);
            qDebug() << "[INFO] Version matched (alternative):" << version;
        } else {
            qDebug() << "[WARN] No version starting with 'V' found in output";
        }
    }

    // 直接设置版本号到输入框（清除样式确保可见）
    ui->lb_test_version->setStyleSheet("");
    ui->lb_test_version->setText(version);
    ui->lb_test_version->update();  // 强制刷新控件
    qDebug() << "[INFO] Final version displayed in widget:" << version;

    setInputsEnabled(true);
    setButtonExecuting(ui->btn_nor_version, false);
    m_currentExecutingButton = nullptr;
#endif
}

void MainWnd::on_btn_nor_sim_test_clicked()
{
    // Function removed per UI requirements
}

void MainWnd::on_btn_nor_iot_test_clicked()
{
    // Function removed per UI requirements
}

void MainWnd::on_btn_nor_all_test_clicked()
{
    // 获取模块类型
    QString moduleName = ui->cb_module_type->currentData().toString();
    QString displayName = ui->cb_module_type->currentText();
    
    qDebug() << "[DEBUG] Module selection - Display:" << displayName << ", Data:" << moduleName;

    // 双语提示辅助：根据当前语言返回中/英文字符串
    auto bilingual = [](const QString& cn, const QString& en) -> QString {
        return (APPMODEL()->CabinetLanguage() == zl::ELanguageType_Cn) ? cn : en;
    };

    // 检查模块类型是否已选择
    if (moduleName.isEmpty()) {
        MsgWnd::ShowNormalInfo(bilingual("请先选择模块类型", "Please select module type first"));
        return;
    }

    // 获取APN和NET输入
    QString apn = ui->le_apn->currentText().trimmed();
    QString net = ui->le_net->currentText().trimmed();

    // 验证APN输入
    if (apn.isEmpty()) {
        MsgWnd::ShowNormalInfo(bilingual("请先输入 APN", "Please input APN first"));
        return;
    }

    // 验证APN格式（字母数字下划线点短横线，1-64位）
    QRegularExpression apnRegex("^[A-Za-z0-9_.-]{1,64}$");
    if (!apnRegex.match(apn).hasMatch()) {
        MsgWnd::ShowNormalInfo(bilingual(
            "APN 格式无效，仅支持字母/数字/下划线/点/短横线（1-64位）",
            "Invalid APN format. Use only letters, numbers, underscore, dot, hyphen (1-64 chars)"));
        return;
    }

    // 验证NET输入
    if (net.isEmpty()) {
        MsgWnd::ShowNormalInfo(bilingual("请先输入 NET 测试地址", "Please input NET host first"));
        return;
    }

    // 验证NET格式（域名或IP地址）
    QRegularExpression domainRegex("^([A-Za-z0-9]([A-Za-z0-9-]{0,61}[A-Za-z0-9])?\\.)+[A-Za-z]{2,}$");
    QRegularExpression ipRegex("^((25[0-5]|(2[0-4]|1\\d|[1-9]|)\\d)\\.?\\b){4}$");
    if (!domainRegex.match(net).hasMatch() && !ipRegex.match(net).hasMatch()) {
        MsgWnd::ShowNormalInfo(bilingual(
            "NET 格式无效，请输入域名（如 www.baidu.com）或 IP 地址（如 8.8.8.8）",
            "Invalid NET format. Use domain name (e.g. www.baidu.com) or IP (e.g. 8.8.8.8)"));
        return;
    }

    // ============ 重要：终止旧测试脚本（如果还在运行） ============
    // 场景：上一次测试已完成（已自动保存），但脚本还在监控阶段运行
    // 用户立即点击新测试时，需要先终止旧脚本
    if (m_scriptProcess && m_scriptProcess->state() == QProcess::Running) {
        if (m_testCompleted) {
            qDebug() << "[INFO] Previous test already completed and saved, terminating monitoring script...";
        } else {
            qDebug() << "[WARN] Previous test not completed, terminating incomplete test script...";
        }

        // 停止前先同步一次当前日志，确保“查看报告”能看到截至当前时刻的内容
        monitorLogFile();
        if (m_testCompleted) {
            updateTestLogToDatabase(false, true);
        }
        
        // 终止旧脚本
        m_isManuallyTerminating = true;
        m_scriptProcess->kill();
        m_scriptProcess->waitForFinished(2000);

        // 再补读一次终止瞬间的新日志，并落库（日志截至到这里）
        monitorLogFile();
        if (m_testCompleted) {
            updateTestLogToDatabase(false, true);
        } else {
            readScriptResults();
        }

        m_scriptProcess->deleteLater();
        m_scriptProcess = nullptr;
        m_isManuallyTerminating = false;
        qDebug() << "[INFO] Previous script terminated, ready for new test";
    }
    
    resetSimInfo();
    resetIotInfo();
    resetCmdResultInfo();

    // 初始化测试记录
    TestRecordManager::getInstance()->GetEmptyRecord(record_);
    record_.module_type = moduleName;
    record_.simiot_test = 1;  // 标记为一键测试
    record_.result_type = zl::EResultType_Success;  // 默认成功，如果失败会更新

    // 禁用输入框和改变按钮颜色
    setInputsEnabled(false);
    setButtonExecuting(ui->btn_nor_all_test, true);

    // 记录当前执行的按钮
    m_currentExecutingButton = ui->btn_nor_all_test;
    m_testCompleted = false;

    // 2分钟后自动恢复（仅在测试未完成时生效）
    // 注意：如果用户选择了错误的模块类型，会导致乱码和无限等待，超时保护机制能避免界面卡死
    if (!m_timeoutTimer) {
        m_timeoutTimer = new QTimer(this);
        m_timeoutTimer->setSingleShot(true);
        connect(m_timeoutTimer, &QTimer::timeout, [this]() {
            if (m_currentExecutingButton == ui->btn_nor_all_test && !m_testCompleted) {
                qDebug() << "[TIMEOUT] Test timeout (possibly wrong module type selected), restoring UI...";
                setInputsEnabled(true);
                setButtonExecuting(ui->btn_nor_all_test, false);
                m_currentExecutingButton = nullptr;
                showTestingOverlay(false);
                
                ui->lb_test_cmd_excute_return_msg->appendPlainText("\n========================================");
                ui->lb_test_cmd_excute_return_msg->appendPlainText(tr("⚠️ Test timeout!"));
                ui->lb_test_cmd_excute_return_msg->appendPlainText(tr("🔍 Possible reasons:"));
                ui->lb_test_cmd_excute_return_msg->appendPlainText(tr("  • Wrong module type selected (e.g., selected EG21 but actual device is EC200U)"));
                ui->lb_test_cmd_excute_return_msg->appendPlainText(tr("  • Module not responding"));
                ui->lb_test_cmd_excute_return_msg->appendPlainText(tr("  • Hardware connection issue"));
                ui->lb_test_cmd_excute_return_msg->appendPlainText(tr("💡 Suggestion: Please verify the module type matches the actual device."));
                ui->lb_test_cmd_excute_return_msg->appendPlainText("========================================");
                
                MsgWnd::ShowNormalInfo(tr("Test timeout! Please check if the module type is correct."));
                
                // 终止脚本进程
                if (m_scriptProcess && m_scriptProcess->state() == QProcess::Running) {
                    m_isManuallyTerminating = true;
                    m_scriptProcess->kill();
                    m_scriptProcess->waitForFinished(2000);
                    m_scriptProcess->deleteLater();
                    m_scriptProcess = nullptr;
                    m_isManuallyTerminating = false;
                    qDebug() << "[INFO] Script process terminated due to timeout";
                }
            }
        });
    }
    m_timeoutTimer->start(300000);  // 5分钟超时，避免错误模块类型导致长时间卡死

    resetRecord();
    
    // 清空之前保存的记录ID，准备新测试
    m_savedSimRecordId.clear();
    m_savedIotRecordId.clear();
    m_lastSavedLogSize = 0;
    m_lastLogSyncTimestampMs = 0;

    // 调用IOT脚本执行测试
    executeIotScript(moduleName, apn, net);
}

void MainWnd::on_btn_nor_clear_clicked()
{
    // 弹出确认对话框再执行清空
    MsgWnd::ShowClearConfirm();
}

void MainWnd::on_btn_nor_save_clicked()
{
    // Function removed per UI requirements
}

void MainWnd::on_btn_nor_reconnect_clicked()
{
    // Function removed per UI requirements
}

void MainWnd::initSerialPortUi()
{
    connect(&m_serial, &SerialManager::portOpened, this, &MainWnd::onSerialPortOpened);
    connect(&m_serial, &SerialManager::portClosed, this, &MainWnd::onSerialPortClosed);
    connect(&m_serial, &SerialManager::errorOccurred, this, &MainWnd::onSerialError);
    connect(&m_serial, &SerialManager::frameSent, this, &MainWnd::onSerialFrameSent);
    connect(&m_serial, &SerialManager::frameReceived, this, &MainWnd::onSerialFrameReceived);
    connect(&m_serial, &SerialManager::passiveFrameReceived, this, &MainWnd::onSerialPassiveFrameReceived);
    connect(&m_serial, &SerialManager::operationTimeout, this, &MainWnd::onSerialOperationTimeout);

    m_serial.setTimeoutMs(Protocol::kDefaultTimeoutMs);

    ui->cb_baudrate->clear();
    const QStringList baudRates = QStringList()
        << "9600" << "115200";
    for (const QString& baud : baudRates)
        ui->cb_baudrate->addItem(baud, baud);

    refreshSerialPorts();
    updateOpenPortButton();
}

void MainWnd::refreshSerialPorts()
{
    const QString currentPort = ui->cb_serial_port->currentData().toString();
    ui->cb_serial_port->clear();

    QStringList ports = m_serial.availablePorts();
    // 枚举为空时保留 COM1~COM30，便于无设备时仍可选择（与后台高级页一致）
    if (ports.isEmpty()) {
        for (int i = 1; i <= 30; ++i)
            ports << QString("COM%1").arg(i);
    }

    for (const QString& portName : ports)
        ui->cb_serial_port->addItem(portName, portName);

    const auto serialConfig = APPMODEL()->Config().serial_config;
    QString preferPort = currentPort.isEmpty() ? serialConfig.port : currentPort;
    if (!preferPort.isEmpty()) {
        const int idx = ui->cb_serial_port->findData(preferPort);
        if (idx >= 0)
            ui->cb_serial_port->setCurrentIndex(idx);
    }

    if (!serialConfig.baud_rate.isEmpty()) {
        const int baudIdx = ui->cb_baudrate->findData(serialConfig.baud_rate);
        if (baudIdx >= 0)
            ui->cb_baudrate->setCurrentIndex(baudIdx);
        else {
            const int defaultIdx = ui->cb_baudrate->findData(QStringLiteral("9600"));
            if (defaultIdx >= 0)
                ui->cb_baudrate->setCurrentIndex(defaultIdx);
        }
    } else {
        const int defaultIdx = ui->cb_baudrate->findData(QStringLiteral("9600"));
        if (defaultIdx >= 0)
            ui->cb_baudrate->setCurrentIndex(defaultIdx);
    }
}

void MainWnd::updateOpenPortButton()
{
    const bool opened = m_serial.isOpen();
    ui->btn_nor_open_port->setText(opened ? tr("Close Serial Port") : tr("Open Serial Port"));
    ui->cb_serial_port->setEnabled(!opened);
    ui->cb_baudrate->setEnabled(!opened);
}

bool MainWnd::openSelectedSerialPort()
{
    const QString port = ui->cb_serial_port->currentData().toString().trimmed();
    const QString baudText = ui->cb_baudrate->currentData().toString().trimmed();
    bool ok = false;
    const int baud = baudText.toInt(&ok);

    if (port.isEmpty()) {
        const QString msg = tr("No serial port selected");
        SetConnectMsg(msg);
        ui->lb_test_cmd_excute_return_msg->appendPlainText(msg);
        return false;
    }
    if (!ok || baud <= 0) {
        const QString msg = tr("Invalid baudrate: %1").arg(baudText);
        SetConnectMsg(msg);
        ui->lb_test_cmd_excute_return_msg->appendPlainText(msg);
        return false;
    }

    if (!m_serial.openPort(port, baud)) {
        // 具体错误由 onSerialError 展示
        updateOpenPortButton();
        return false;
    }

    // 写回配置，与后台高级页保持一致
    auto config = APPMODEL()->Config();
    config.serial_config.port = port;
    config.serial_config.baud_rate = baudText;
    APPMODEL()->SetConfig(config);
    APPMODEL()->SaveAppConfiguration();

    return true;
}

void MainWnd::closeSerialPort()
{
    m_serial.closePort();
}

void MainWnd::on_btn_nor_open_port_clicked()
{
    if (m_serial.isOpen()) {
        closeSerialPort();
        return;
    }
    openSelectedSerialPort();
}

void MainWnd::onSerialPortOpened(const QString &portName)
{
    const QString baudText = ui->cb_baudrate->currentData().toString();
    const QString msg = tr("Serial %1, baudrate %2, connect success").arg(portName, baudText);
    SetConnectMsg(msg);
    ui->lb_test_cmd_excute_return_msg->appendPlainText(msg);
    updateOpenPortButton();
}

void MainWnd::onSerialPortClosed()
{
    const QString msg = tr("Serial port closed");
    SetConnectMsg(msg);
    ui->lb_test_cmd_excute_return_msg->appendPlainText(msg);
    updateOpenPortButton();
}

void MainWnd::onSerialError(const QString &message)
{
    SetConnectMsg(message);
    ui->lb_test_cmd_excute_return_msg->appendPlainText(tr("[ERROR] %1").arg(message));
    updateOpenPortButton();
}

void MainWnd::onSerialFrameSent(const QByteArray &frame)
{
    ui->lb_test_cmd_excute_return_msg->appendPlainText(
        tr("[TX] %1").arg(Protocol::ProtocolCodec::frameToHex(frame)));
}

void MainWnd::onSerialFrameReceived(const Protocol::Frame &frame, const QString &parsedText)
{
    ui->lb_test_cmd_excute_return_msg->appendPlainText(
        tr("[RX match] seq=%1 cmd=0x%2 %3")
            .arg(frame.seq)
            .arg(frame.cmd, 2, 16, QChar('0'))
            .arg(Protocol::ProtocolCodec::frameToHex(frame.raw)));
    ui->lb_test_cmd_excute_return_msg->appendPlainText(parsedText);
}

void MainWnd::onSerialPassiveFrameReceived(const Protocol::Frame &frame, const QString &reason)
{
    ui->lb_test_cmd_excute_return_msg->appendPlainText(
        tr("[RX passive] seq=%1 cmd=0x%2 resp=0x%3 %4 (%5)")
            .arg(frame.seq)
            .arg(frame.cmd, 2, 16, QChar('0'))
            .arg(frame.resp, 2, 16, QChar('0'))
            .arg(Protocol::ProtocolCodec::frameToHex(frame.raw))
            .arg(reason));
}

void MainWnd::onSerialOperationTimeout()
{
    ui->lb_test_cmd_excute_return_msg->appendPlainText(
        tr("[TIMEOUT] No matching response within %1 ms").arg(m_serial.timeoutMs()));
}

void MainWnd::auto_save_record()
{
    if (record_.sim_test == 0 && record_.iot_test == 0 && record_.simiot_test == 0)
    {
        qDebug() << "[ERROR]" << "auto save record fail, test type error";
        return;
    }

    if (record_.result_type == zl::EResultType_Unknow || record_.cmd_ret_info.isEmpty() || record_.result_info.isEmpty())
    {
        qDebug() << "[ERROR]" << "auto save record fail, result type: " << record_.result_type << " ,cmd ret info: " << record_.cmd_ret_info << " ,result info: " << record_.result_info;
        return;
    }

    auto ret = TestRecordManager::getInstance()->SearchRecord(record_.record_id);
    if (ret == zl::EResult_Success)
    {
        qDebug() << "[ERROR]" << "test record saved";

        TestRecordManager::getInstance()->RemoveRecord(record_.record_id);
    }

    ret = TestRecordManager::getInstance()->SaveTestRecord(record_);

    if (ret != zl::EResult_Success)
    {
        qDebug() << "[ERROR]" << "auto save record fail, ret: " << ret;
        return;
    }
}

void MainWnd::updateDefaultValues()
{
    // 语言切换时不自动填充APN/NET，由用户手动选择或输入
    Q_UNUSED(this);
}

void MainWnd::setInputsEnabled(bool enabled)
{
    // 模块类型固定 EG21，始终不可改
    ui->cb_module_type->setEnabled(false);
    ui->le_apn->setEnabled(enabled);
    ui->le_net->setEnabled(enabled);

    // 串口打开时不允许改端口/波特率（使用独立 SerialManager）
    const bool serialEditable = enabled && !m_serial.isOpen();
    ui->cb_serial_port->setEnabled(serialEditable);
    ui->cb_baudrate->setEnabled(serialEditable);
    ui->btn_nor_open_port->setEnabled(enabled);

    // 强制刷新界面
    ui->cb_module_type->update();
    ui->le_apn->update();
    ui->le_net->update();
    ui->cb_serial_port->update();
    ui->cb_baudrate->update();

    qDebug() << "[DEBUG] Input fields enabled:" << enabled;
}

void MainWnd::setButtonExecuting(QPushButton* button, bool executing)
{
    if (executing) {
        // 仅禁用按钮防止重复点击，不改变颜色
        button->setEnabled(false);
        qDebug() << "[DEBUG] Button set to executing (disabled):" << button->objectName();
    } else {
        button->setEnabled(true);
        qDebug() << "[DEBUG] Button restored to normal:" << button->objectName();
    }

    // 强制刷新按钮显示
    button->update();
    QApplication::processEvents();
}

void MainWnd::on_btn_nor_record_clicked()
{
    // Go to Record Page in Backend
    GlobalSignal::getInstance()->backend_page_change(zl::EBP_Record_page);
}

// ===================== IOT脚本执行相关实现 =====================

void MainWnd::initModuleTypeComboBox()
{
    // 模块类型固定为 EG21，不可修改
    ui->cb_module_type->clear();
    ui->cb_module_type->addItem("EG21", "EG21");
    ui->cb_module_type->setCurrentIndex(0);
    ui->cb_module_type->setEditable(false);
    ui->cb_module_type->setEnabled(false);
}

void MainWnd::executeIotScript(const QString& moduleName, const QString& apn, const QString& net)
{
    qDebug() << "[INFO] Executing IOT script: module=" << moduleName << ", apn=" << apn << ", net=" << net;

    // 检查脚本文件是否存在（仅Linux需要检查）
#ifndef Q_OS_WIN
    if (!QFile::exists(m_scriptPath)) {
        qDebug() << "[ERROR] Script file not found:" << m_scriptPath;
        MsgWnd::ShowNormalInfo(tr("Script file not found: %1").arg(m_scriptPath));
        setInputsEnabled(true);
        setButtonExecuting(m_currentExecutingButton, false);
        m_currentExecutingButton = nullptr;
        return;
    }
#else
    qDebug() << "[INFO] Windows test mode - will simulate script execution";
#endif

    // 检查是否已有进程在运行（按钮点击时已经处理过，理论上不会到这里）
    if (m_scriptProcess && m_scriptProcess->state() == QProcess::Running) {
        qDebug() << "[WARNING] Unexpected: script still running in executeIotScript (should be handled in button handler)";
        
        // 设置主动终止标志，避免在onScriptFinished中显示错误提示
        m_isManuallyTerminating = true;
        
        // 终止旧进程
        m_scriptProcess->kill();
        m_scriptProcess->waitForFinished(2000);
        
        // 清理旧进程对象
        m_scriptProcess->deleteLater();
        m_scriptProcess = nullptr;
        
        // 重置标志
        m_isManuallyTerminating = false;
    }

    // 清空输出框并初始化日志文件大小
    ui->lb_test_cmd_excute_return_msg->clear();
    m_lastLogFileSize = 0;
    ui->lb_test_cmd_excute_return_msg->appendPlainText(tr("Starting IOT module test..."));
    ui->lb_test_cmd_excute_return_msg->appendPlainText(tr("Module: %1").arg(moduleName));
    ui->lb_test_cmd_excute_return_msg->appendPlainText(QString("[DEBUG] Script will receive: -m %1").arg(moduleName));
    ui->lb_test_cmd_excute_return_msg->appendPlainText(tr("APN: %1").arg(apn));
    ui->lb_test_cmd_excute_return_msg->appendPlainText(tr("Network Test Host: %1").arg(net));
    ui->lb_test_cmd_excute_return_msg->appendPlainText("----------------------------------------");

    // 保存到历史记录（已禁用）
    // addApnToHistory(apn);
    // addNetToHistory(net);

    // 创建进程对象
    if (!m_scriptProcess) {
        m_scriptProcess = new QProcess(this);
        connect(m_scriptProcess, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), 
                this, [this](int exitCode, QProcess::ExitStatus exitStatus) {
                    onScriptFinished(exitCode, static_cast<int>(exitStatus));
                });
        connect(m_scriptProcess, &QProcess::readyReadStandardOutput, 
                this, &MainWnd::onScriptReadyRead);
        connect(m_scriptProcess, &QProcess::readyReadStandardError, 
                this, &MainWnd::onScriptReadyRead);
    }

    // 构造命令参数
    QStringList arguments;
    
#ifdef Q_OS_WIN
    // Windows上使用cmd模拟（用于UI测试）
    ui->lb_test_cmd_excute_return_msg->appendPlainText("[Windows Test Mode] Simulating script execution...");
    arguments << "/c" << "echo [Test Mode] Module:" << moduleName << "APN:" << apn << "NET:" << net 
              << "&&" << "timeout" << "/t" << "2" << "/nobreak" << ">nul";
    m_scriptProcess->setProgram("cmd");
#else
    // Linux上实际执行脚本
    arguments << "-m" << moduleName;
    arguments << "-a" << apn;
    arguments << "-ping" << net;
    m_scriptProcess->setProgram(m_scriptPath);
#endif

    qDebug() << "[INFO] Command:" << m_scriptProcess->program() << arguments.join(" ");

    // 启动进程
    m_scriptProcess->setArguments(arguments);
    m_scriptProcess->start();

    if (!m_scriptProcess->waitForStarted(3000)) {
        qDebug() << "[ERROR] Failed to start script:" << m_scriptProcess->errorString();
        ui->lb_test_cmd_excute_return_msg->appendPlainText(tr("Failed to start script: %1").arg(m_scriptProcess->errorString()));
        MsgWnd::ShowNormalInfo(tr("Failed to start IOT test script"));
        
        setInputsEnabled(true);
        setButtonExecuting(m_currentExecutingButton, false);
        m_currentExecutingButton = nullptr;
        return;
    }

    ui->lb_test_cmd_excute_return_msg->appendPlainText(tr("Script started, waiting for results..."));
    qDebug() << "[INFO] Script started successfully, PID:" << m_scriptProcess->processId();

    // 显示测试中遮罩
    showTestingOverlay(true);

#ifdef Q_OS_WIN
    // Windows测试模式：创建模拟测试数据
    createTestDataForWindows();
#endif

    // 启动日志文件监控
    startLogMonitoring();
}

void MainWnd::onScriptReadyRead()
{
    if (!m_scriptProcess) return;

    // 读取标准输出和错误输出
    QByteArray stdOutput = m_scriptProcess->readAllStandardOutput();
    QByteArray stdError = m_scriptProcess->readAllStandardError();

    if (!stdOutput.isEmpty()) {
        QString output = QString::fromUtf8(stdOutput);
        ui->lb_test_cmd_excute_return_msg->appendPlainText(output);
        qDebug() << "[SCRIPT OUTPUT]" << output;
        
        // 检查测试完成标志
        checkTestCompletion(output);
    }

    if (!stdError.isEmpty()) {
        QString error = QString::fromUtf8(stdError);
        ui->lb_test_cmd_excute_return_msg->appendPlainText(error);
        qDebug() << "[SCRIPT ERROR]" << error;
        
        // 也检查错误输出中的测试完成标志
        checkTestCompletion(error);
    }
}

void MainWnd::onScriptFinished(int exitCode, int exitStatus)
{
    qDebug() << "[INFO] Script finished, exitCode:" << exitCode << ", status:" << exitStatus;

    // 隐藏测试中遮罩
    showTestingOverlay(false);

    // 停止日志监控
    stopLogMonitoring();

    // 最后再读取一次日志，确保没有遗漏
    monitorLogFile();

    if (exitStatus == QProcess::CrashExit) {
        // 如果是主动终止的（为了开始新测试），不显示错误提示
        if (!m_isManuallyTerminating) {
            ui->lb_test_cmd_excute_return_msg->appendPlainText("----------------------------------------");
            ui->lb_test_cmd_excute_return_msg->appendPlainText(tr("Script crashed or was terminated"));
            MsgWnd::ShowNormalInfo(tr("Test script failed"));
        } else {
            qDebug() << "[INFO] Script was manually terminated for new test, skipping error message";
        }
        // 如果测试阶段未完成就崩溃，尝试保存已有的测试结果
        if (!m_testCompleted) {
            qDebug() << "[WARN] Test was not completed before script exit, saving partial results";
            readScriptResults();
        } else {
            // 测试已完成并保存，更新日志以包含拨号和监控信息
            qDebug() << "[INFO] Test already completed and saved, updating log with dial/monitor info";
            updateTestLogToDatabase(true, true);
        }
    } else {
        ui->lb_test_cmd_excute_return_msg->appendPlainText("----------------------------------------");
        ui->lb_test_cmd_excute_return_msg->appendPlainText(tr("Script execution completed, exit code: %1").arg(exitCode));
        
        // 显示日志文件内容
        displayScriptLog();
        
        // 如果测试阶段已完成，更新日志（包含拨号和监控部分）
        if (!m_testCompleted) {
            qDebug() << "[WARN] Test was not completed before script exit, saving partial results";
            readScriptResults();
        } else {
            // 测试已完成并保存，更新日志以包含拨号和监控信息
            qDebug() << "[INFO] Test already completed and saved, updating log with dial/monitor info";
            updateTestLogToDatabase(true, true);
        }
    }

    // 恢复按钮状态
    if (m_currentExecutingButton != nullptr) {
        setInputsEnabled(true);
        setButtonExecuting(m_currentExecutingButton, false);
        m_currentExecutingButton = nullptr;
    }

    // 确保进程状态被清理，允许下次执行
    if (m_scriptProcess) {
        m_scriptProcess->deleteLater();
        m_scriptProcess = nullptr;
        qDebug() << "[INFO] Script process cleaned up, ready for next execution";
    }
}

void MainWnd::displayScriptLog()
{
    QString logFile = getLogFilePath();
    QString logContent = readFileContent(logFile);
    
    if (!logContent.isEmpty()) {
        ui->lb_test_cmd_excute_return_msg->appendPlainText("\n========== IOT Test Log ==========");
        ui->lb_test_cmd_excute_return_msg->appendPlainText(logContent);
        ui->lb_test_cmd_excute_return_msg->appendPlainText("========== End of Log ==========");
    } else {
        qDebug() << "[WARN] Log file is empty or cannot be read:" << logFile;
        ui->lb_test_cmd_excute_return_msg->appendPlainText(tr("Log file not found or empty: %1").arg(logFile));
    }
}

QString MainWnd::getLogFilePath() const
{
#ifdef Q_OS_WIN
    // Windows上使用临时目录
    return QDir::tempPath() + "/iot_start.log";
#else
    // Linux上使用系统日志目录
    return "/var/log/iot_start.log";
#endif
}

QString MainWnd::getDevInfoPath() const
{
#ifdef Q_OS_WIN
    // Windows上使用临时目录
    return QDir::tempPath() + "/dev_info/";
#else
    // Linux上使用/tmp目录
    return "/tmp/dev_info/";
#endif
}

#ifdef Q_OS_WIN
void MainWnd::createTestDataForWindows()
{
    // 创建模拟的测试数据文件（Windows测试模式）
    QString devInfoDir = getDevInfoPath();
    QDir dir;
    if (!dir.exists(devInfoDir)) {
        dir.mkpath(devInfoDir);
    }

    // 创建模拟日志文件
    QString logFile = getLogFilePath();
    QFile log(logFile);
    if (log.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&log);
        out << "[" << QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss") << "] Starting IOT test...\n";
        out << "[" << QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss") << "] Module initialized\n";
        out << "[" << QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss") << "] Network test passed\n";
        out << "[" << QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss") << "] Test completed successfully\n";
        log.close();
    }

    // 创建模拟结果文件
    auto writeTestFile = [&](const QString& filename, const QString& content) {
        QFile file(devInfoDir + filename);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            out << content;
            file.close();
        }
    };

    writeTestFile("IMAGE_VERSION", "1.0.0-test");
    writeTestFile("IOT_VERSION", "ME3630_V1.2.3");
    writeTestFile("IOT_IMEI", "123456789012345");
    writeTestFile("IOT_CCID", "89860000000000000000");
    writeTestFile("IOT_CSQ", "25");
    writeTestFile("IOT_CREG", "1");
    writeTestFile("IOT_ACT", "7");

    qDebug() << "[INFO] Windows test data created in" << devInfoDir;
}
#endif

void MainWnd::readScriptResults()
{
    QString devInfoDir = getDevInfoPath();
    qDebug() << "[INFO] Reading script results from" << devInfoDir;
    
    // 读取镜像版本
    QString imageVersion = readFileContent(devInfoDir + "IMAGE_VERSION").trimmed();
    if (!imageVersion.isEmpty()) {
        ui->lb_test_version->setStyleSheet("");
        ui->lb_test_version->setText(imageVersion);
        record_.version = imageVersion;
        qDebug() << "[INFO] Image Version updated from file:" << imageVersion;
    } else {
        qDebug() << "[WARN] IMAGE_VERSION file is empty or not found";
    }

    // 读取IOT模块版本
    QString iotVersion = readFileContent(devInfoDir + "IOT_VERSION").trimmed();
    if (!iotVersion.isEmpty()) {
        ui->lb_test_iot_module_ver->setStyleSheet("");
        ui->lb_test_iot_module_ver->setText(iotVersion);
        record_.iot_module_id = iotVersion;
        qDebug() << "[INFO] IOT Version:" << iotVersion;
    } else if (ui->lb_test_iot_module_ver->text().isEmpty()) {
        setLabelFailed(ui->lb_test_iot_module_ver);
        record_.iot_module_id = "测试失败";
        qDebug() << "[INFO] IOT Version: FAILED (file empty)";
    }

    // 读取IOT IMEI
    QString iotImei = readFileContent(devInfoDir + "IOT_IMEI").trimmed();
    if (!iotImei.isEmpty()) {
        ui->lb_test_iot_imei->setStyleSheet("");
        ui->lb_test_iot_imei->setText(iotImei);
        record_.iot_imei = iotImei;
        qDebug() << "[INFO] IOT IMEI:" << iotImei;
    } else if (ui->lb_test_iot_imei->text().isEmpty()) {
        setLabelFailed(ui->lb_test_iot_imei);
        record_.iot_imei = "测试失败";
        qDebug() << "[INFO] IOT IMEI: FAILED (file empty)";
    }

    // 读取SIM卡ICCID
    QString iccid = readFileContent(devInfoDir + "IOT_CCID").trimmed();
    if (!iccid.isEmpty()) {
        ui->lb_test_iccid->setStyleSheet("");
        ui->lb_test_iccid->setText(iccid);
        record_.iccid = iccid;
        qDebug() << "[INFO] ICCID:" << iccid;
    } else if (ui->lb_test_iccid->text().isEmpty()) {
        setLabelFailed(ui->lb_test_iccid);
        record_.iccid = "测试失败";
        qDebug() << "[INFO] ICCID: FAILED (file empty)";
    }

    // 读取网络注册状态
    QString cregState = readFileContent(devInfoDir + "IOT_CREG").trimmed();
    if (!cregState.isEmpty()) {
        QString networkStatus;
        auto langText = [](const QString& cn, const QString& en) -> QString {
            return (APPMODEL()->CabinetLanguage() == zl::ELanguageType_Cn) ? cn : en;
        };
        if (cregState == "1" || cregState == "5") {
            networkStatus = tr("Pass");
            record_.net_status = zl::ESimNetStatus_Success;
            ui->lb_test_sim_network->setStyleSheet("");
        } else if (cregState == "2") {
            // CREG=2: 正在搜网，最终结果阶段视为关键项未通过。
            networkStatus = langText("搜网中(状态:2)", "Searching (State: 2)");
            record_.net_status = zl::ESimNetStatus_Fail;
            ui->lb_test_sim_network->setStyleSheet("color: #CC8800;");
        } else if (cregState == "0") {
            networkStatus = langText("未注册网络(状态:0)", "Not registered (State: 0)");
            record_.net_status = zl::ESimNetStatus_Fail;
            ui->lb_test_sim_network->setStyleSheet("color: red;");
        } else if (cregState == "3") {
            networkStatus = tr("测试失败");
            record_.net_status = zl::ESimNetStatus_Fail;
            ui->lb_test_sim_network->setStyleSheet("color: red;");
        } else {
            networkStatus = tr("Unknown (State: %1)").arg(cregState);
            record_.net_status = zl::ESimNetStatus_Unknow;
            ui->lb_test_sim_network->setStyleSheet("");
        }
        ui->lb_test_sim_network->setText(networkStatus);
        qDebug() << "[INFO] Network Status:" << networkStatus;
    } else if (ui->lb_test_sim_network->text().isEmpty()) {
        setLabelFailed(ui->lb_test_sim_network);
        record_.net_status = zl::ESimNetStatus_Fail;
        qDebug() << "[INFO] Network Status: FAILED (file empty)";
    }

    // 读取信号强度
    QString csq = readFileContent(devInfoDir + "IOT_CSQ").trimmed();
    if (!csq.isEmpty()) {
        bool ok;
        int rssi = csq.toInt(&ok);
        QString rssiInfo;
        if (ok) {
            if (rssi == 0) {
                rssiInfo = "-113 dBm or lower";
            } else if (rssi == 1) {
                rssiInfo = "-111 dBm";
            } else if (rssi >= 2 && rssi <= 30) {
                int dbm = -113 + 2 * rssi;
                rssiInfo = QString::number(dbm) + " dBm";
            } else if (rssi == 31) {
                rssiInfo = "-51 dBm or higher";
            } else if (rssi == 99) {
                rssiInfo = "Unknown/Not detectable";
            } else {
                rssiInfo = QString("Invalid value (%1)").arg(rssi);
            }
        } else {
            rssiInfo = csq;
        }
        ui->lb_test_rssi->setStyleSheet("");
        ui->lb_test_rssi->setText(rssiInfo);
        record_.signal_strength = rssiInfo;
        qDebug() << "[INFO] Signal Strength:" << rssiInfo;
    } else if (ui->lb_test_rssi->text().isEmpty()) {
        setLabelFailed(ui->lb_test_rssi);
        record_.signal_strength = "Test Failed";
        qDebug() << "[INFO] Signal Strength: FAILED (file empty)";
    }

    // 读取网络制式
    QString networkType = readFileContent(devInfoDir + "IOT_ACT").trimmed();
    if (!networkType.isEmpty()) {
        QString netTypeDisplay;
        if (networkType.contains("LTE", Qt::CaseInsensitive)) {
            netTypeDisplay = tr("LTE (4G)");
        } else if (networkType.contains("UMTS", Qt::CaseInsensitive)) {
            netTypeDisplay = tr("UMTS (3G)");
        } else if (networkType.contains("GSM", Qt::CaseInsensitive)) {
            netTypeDisplay = tr("GSM (2G)");
        } else {
            netTypeDisplay = networkType;
        }
        ui->lb_test_network_type->setStyleSheet("");
        ui->lb_test_network_type->setText(netTypeDisplay);
        record_.network_type = netTypeDisplay;
        qDebug() << "[INFO] Network Type:" << netTypeDisplay;
    } else if (ui->lb_test_network_type->text().isEmpty()) {
        setLabelFailed(ui->lb_test_network_type);
        record_.network_type = "测试失败";
        qDebug() << "[INFO] Network Type: FAILED (file empty)";
    }

    // 读取完整的测试日志
    QString logFile = getLogFilePath();
    QString logContent = readFileContent(logFile);
    if (!logContent.isEmpty()) {
        record_.test_log = logContent;
        m_lastSavedLogSize = logContent.size();
        m_lastLogSyncTimestampMs = QDateTime::currentMSecsSinceEpoch();
        qDebug() << "[INFO] Test log saved, size:" << logContent.size() << "bytes";
    } else {
        qDebug() << "[WARN] Test log file not found or empty:" << logFile;
    }

    ui->lb_test_cmd_excute_return_msg->appendPlainText("\n" + tr("Test results have been loaded successfully."));
    
    // 保存测试记录到数据库
    saveTestRecordToDatabase();
}

QString MainWnd::readFileContent(const QString& filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "[WARN] Cannot open file:" << filePath;
        return QString();
    }

    QTextStream in(&file);
    QString content = in.readAll();
    file.close();

    return content;
}

// ===================== 日志监控相关实现 =====================

void MainWnd::startLogMonitoring()
{
    if (!m_logMonitorTimer) {
        return;
    }

    // 重置日志文件大小
    m_lastLogFileSize = 0;
    
    // 每500毫秒检查一次日志文件
    m_logMonitorTimer->start(500);
    
    qDebug() << "[INFO] Log monitoring started";
}

void MainWnd::stopLogMonitoring()
{
    if (m_logMonitorTimer && m_logMonitorTimer->isActive()) {
        m_logMonitorTimer->stop();
        qDebug() << "[INFO] Log monitoring stopped";
    }
}

void MainWnd::onLogMonitorTimeout()
{
    monitorLogFile();
}

void MainWnd::monitorLogFile()
{
    QString logFile = getLogFilePath();
    QFile file(logFile);
    
    if (!file.exists()) {
        return;
    }

    qint64 currentSize = file.size();
    
    // 如果文件大小没变化，不处理
    if (currentSize == m_lastLogFileSize) {
        return;
    }

    // 打开文件并跳到上次读取的位置
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "[WARN] Cannot open log file:" << logFile;
        return;
    }

    // 跳到上次读取的位置
    file.seek(m_lastLogFileSize);

    // 读取新增内容
    QTextStream in(&file);
    QString newContent = in.readAll();
    
    if (!newContent.isEmpty()) {
        ui->lb_test_cmd_excute_return_msg->appendPlainText(newContent.trimmed());
        
        // 逐行解析日志，实时更新界面测试结果
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
        QStringList lines = newContent.split('\n', Qt::SkipEmptyParts);
#else
        QStringList lines = newContent.split('\n', QString::SkipEmptyParts);
#endif
        for (const QString& line : lines) {
            parseLogLineAndUpdateUI(line);
        }
        
        // 检查测试完成标志
        checkTestCompletion(newContent);

        // 测试阶段已保存后，继续把监控阶段新增日志实时同步到数据库
        if (m_testCompleted) {
            updateTestLogToDatabase(false, false);
        }
        
        // 自动滚动到底部
        QTextCursor cursor = ui->lb_test_cmd_excute_return_msg->textCursor();
        cursor.movePosition(QTextCursor::End);
        ui->lb_test_cmd_excute_return_msg->setTextCursor(cursor);
    }

    m_lastLogFileSize = currentSize;
    file.close();
}

// ===================== APN/NET历史记录管理 =====================

void MainWnd::loadApnNetHistory()
{
    // 预设的APN选项
    QStringList apnList;
    apnList << "3gnet" << "cmnet" << "iot.1nce.net" << "eseye1" << "orange.m2m.spec";

    ui->le_apn->clear();
    ui->le_apn->addItems(apnList);
    ui->le_apn->setEditable(true);

    // 预设的NET选项
    QStringList netList;
    netList << "www.baidu.com" << "www.google.com" << "8.8.8.8"
            << "www.amazon.com" << "1.1.1.1" << "www.bing.com";

    ui->le_net->clear();
    ui->le_net->addItems(netList);
    ui->le_net->setEditable(true);
}

void MainWnd::addApnToHistory(const QString& apn)
{
    Q_UNUSED(apn);
}

void MainWnd::addNetToHistory(const QString& net)
{
    Q_UNUSED(net);
}

void MainWnd::saveTestRecordToDatabase()
{
    // 检查是否有有效的测试结果
    if (record_.record_id.isEmpty()) {
        qDebug() << "[ERROR] Cannot save record: record_id is empty";
        return;
    }

    // 分别保存SIM卡测试记录和IOT测试记录
    
    // 1. 保存SIM卡测试记录
    if (!record_.iccid.isEmpty()) {
        zl::RecordInfo simRecord = record_;
        simRecord.test_type = zl::ETestType_Sim;
        simRecord.sim_test = 1;
        simRecord.iot_test = 0;
        simRecord.simiot_test = 0;
        
        // 生成新的流水号
        simRecord.record_id = TestRecordManager::getInstance()->GeneratedRecordID();
        
        int32_t ret = TestRecordManager::getInstance()->SaveTestRecord(simRecord);
        if (ret == zl::EResult_Success) {
            m_savedSimRecordId = simRecord.record_id;  // 保存记录ID用于后续更新
            qDebug() << "[INFO] SIM card test record saved successfully, ID:" << simRecord.record_id;
        } else {
            qDebug() << "[ERROR] Failed to save SIM card test record";
        }
    }

    // 2. 保存IOT测试记录
    if (!record_.iot_imei.isEmpty()) {
        zl::RecordInfo iotRecord = record_;
        iotRecord.test_type = zl::ETestType_Iot;
        iotRecord.sim_test = 0;
        iotRecord.iot_test = 1;
        iotRecord.simiot_test = 0;
        
        // 生成新的流水号
        iotRecord.record_id = TestRecordManager::getInstance()->GeneratedRecordID();
        
        int32_t ret = TestRecordManager::getInstance()->SaveTestRecord(iotRecord);
        if (ret == zl::EResult_Success) {
            m_savedIotRecordId = iotRecord.record_id;  // 保存记录ID用于后续更新
            qDebug() << "[INFO] IOT test record saved successfully, ID:" << iotRecord.record_id;
        } else {
            qDebug() << "[ERROR] Failed to save IOT test record";
        }
    }
    
    ui->lb_test_cmd_excute_return_msg->appendPlainText(tr("Test records have been saved to database."));
}

void MainWnd::updateTestLogToDatabase(bool showHint, bool forceUpdate)
{
    // 只有已有记录ID时才需要同步日志
    if (m_savedSimRecordId.isEmpty() && m_savedIotRecordId.isEmpty()) {
        return;
    }

    // 节流：默认至少间隔2秒再同步一次，避免高频写数据库
    const qint64 nowMs = QDateTime::currentMSecsSinceEpoch();
    if (!forceUpdate && m_lastLogSyncTimestampMs > 0 && (nowMs - m_lastLogSyncTimestampMs) < 2000) {
        return;
    }

    // 读取完整的测试日志（包含拨号和监控部分）
    QString logFile = getLogFilePath();
    QString completeLog = readFileContent(logFile);
    
    if (completeLog.isEmpty()) {
        qDebug() << "[WARN] Cannot update test log: log file is empty";
        return;
    }

    if (!forceUpdate && completeLog.size() <= m_lastSavedLogSize) {
        return;
    }
    
    qDebug() << "[INFO] Updating test log to database, log size:" << completeLog.size() << "bytes";
    
    // 更新SIM卡测试记录的日志
    if (!m_savedSimRecordId.isEmpty()) {
        int32_t ret = TestRecordManager::getInstance()->UpdateTestLog(m_savedSimRecordId, completeLog);
        if (ret == zl::EResult_Success) {
            qDebug() << "[INFO] SIM card test log updated successfully, ID:" << m_savedSimRecordId;
        } else {
            qDebug() << "[ERROR] Failed to update SIM card test log";
        }
    }
    
    // 更新IOT测试记录的日志
    if (!m_savedIotRecordId.isEmpty()) {
        int32_t ret = TestRecordManager::getInstance()->UpdateTestLog(m_savedIotRecordId, completeLog);
        if (ret == zl::EResult_Success) {
            qDebug() << "[INFO] IOT test log updated successfully, ID:" << m_savedIotRecordId;
        } else {
            qDebug() << "[ERROR] Failed to update IOT test log";
        }
    }

    m_lastSavedLogSize = completeLog.size();
    m_lastLogSyncTimestampMs = nowMs;

    if (showHint) {
        ui->lb_test_cmd_excute_return_msg->appendPlainText(tr("Complete test log (including dial/monitor) has been updated to database."));
    }
}

// 清理超过指定月数的旧日志文件
void MainWnd::cleanupOldLogFiles()
{
    QString logPath;
#ifdef PATH_LOG_FILE
    logPath = PATH_LOG_FILE;
#else
    logPath = "logs/";
#endif

    QDir logDir(logPath);
    if (!logDir.exists()) {
        qDebug() << "[INFO] Log directory does not exist, skip cleanup:" << logPath;
        return;
    }

    // 获取当前日期
    QDate currentDate = QDate::currentDate();
    // 计算cutoff日期（MINI_MONTHS_LOG_FILE个月前）
    QDate cutoffDate = currentDate.addMonths(-MINI_MONTHS_LOG_FILE);

    qDebug() << "[INFO] Cleaning up log files older than" << MINI_MONTHS_LOG_FILE << "month(s), cutoff date:" << cutoffDate.toString("yyyy-MM-dd");

    // 过滤日志文件：yyyy-MM-dd.log 格式
    QStringList filters;
    filters << "*.log";
    QFileInfoList fileList = logDir.entryInfoList(filters, QDir::Files | QDir::NoDotAndDotDot, QDir::Name);

    int deletedCount = 0;
    qint64 totalSize = 0;

    for (const QFileInfo& fileInfo : fileList) {
        QString fileName = fileInfo.baseName(); // 获取不带扩展名的文件名，例如 "2025-01-15"
        
        // 尝试解析日期格式 yyyy-MM-dd
        QDate fileDate = QDate::fromString(fileName, "yyyy-MM-dd");
        
        if (fileDate.isValid() && fileDate < cutoffDate) {
            qint64 fileSize = fileInfo.size();
            QString fullPath = fileInfo.absoluteFilePath();
            
            if (QFile::remove(fullPath)) {
                deletedCount++;
                totalSize += fileSize;
                qDebug() << "[INFO] Deleted old log file:" << fileName << ".log (" << fileSize << "bytes, date:" << fileDate.toString("yyyy-MM-dd") << ")";
            } else {
                qDebug() << "[WARN] Failed to delete log file:" << fullPath;
            }
        }
    }

    if (deletedCount > 0) {
        qDebug() << "[INFO] Log cleanup completed:" << deletedCount << "file(s) deleted, total" << totalSize << "bytes freed";
    } else {
        qDebug() << "[INFO] No old log files to clean up";
    }
}

// 实时解析日志行并更新界面测试结果
void MainWnd::parseLogLineAndUpdateUI(const QString& line)
{
    // 解析镜像版本
    if (line.contains("【镜像版本】")) {
        QRegularExpression re("【镜像版本】(.+)");
        QRegularExpressionMatch match = re.match(line);
        if (match.hasMatch()) {
            QString version = match.captured(1).trimmed();
            ui->lb_test_version->setStyleSheet("");  // 清除红色样式
            ui->lb_test_version->setText(version);
            record_.version = version;
            qDebug() << "[UI UPDATE] Image Version:" << version;
        }
    }
    // 解析IOT模块版本
    else if (line.contains("固件版本：")) {
        QRegularExpression re("固件版本：(.+)");
        QRegularExpressionMatch match = re.match(line);
        if (match.hasMatch()) {
            QString version = match.captured(1).trimmed();
            ui->lb_test_iot_module_ver->setStyleSheet("");  // 清除红色样式
            ui->lb_test_iot_module_ver->setText(version);
            record_.iot_module_id = version;
            qDebug() << "[UI UPDATE] IOT Version:" << version;
        }
    }
    // 解析IMEI
    else if (line.contains("IMEI：")) {
        QRegularExpression re("IMEI：(\\d+)");
        QRegularExpressionMatch match = re.match(line);
        if (match.hasMatch()) {
            QString imei = match.captured(1).trimmed();
            ui->lb_test_iot_imei->setText(imei);
            record_.iot_imei = imei;
            qDebug() << "[UI UPDATE] IMEI:" << imei;
        }
    }
    // 解析SIM卡ICCID（支持包含字母的ICCID，如898604F1092380615367）
    else if (line.contains("SIM卡ICCID：")) {
        QRegularExpression re("SIM卡ICCID：([0-9A-Fa-f]+)");
        QRegularExpressionMatch match = re.match(line);
        if (match.hasMatch()) {
            QString iccid = match.captured(1).trimmed();
            ui->lb_test_iccid->setText(iccid);
            record_.iccid = iccid;
            qDebug() << "[UI UPDATE] ICCID:" << iccid;
        }
    }
    // 解析信号强度
    else if (line.contains("信号强度CSQ：")) {
        QRegularExpression re("信号强度CSQ：(\\d+).*精准dBm：(.+)");
        QRegularExpressionMatch match = re.match(line);
        if (match.hasMatch()) {
            QString csq = match.captured(1).trimmed();
            QString dbm = match.captured(2).trimmed();
            ui->lb_test_rssi->setText(dbm);
            record_.signal_strength = dbm;
            qDebug() << "[UI UPDATE] Signal Strength:" << dbm;
        }
    }
    // 解析运营商和网络制式（格式化输出）
    else if (line.contains("当前运营商：") && line.contains("网络制式：")) {
        QRegularExpression re("当前运营商：(.+?)\\s*\\|\\s*网络制式：(.+)");
        QRegularExpressionMatch match = re.match(line);
        if (match.hasMatch()) {
            QString oper = match.captured(1).trimmed();
            QString netType = match.captured(2).trimmed();
            
            // 只有在网络制式非空时才更新
            if (!netType.isEmpty()) {
                // 转换网络制式显示格式
                QString netTypeDisplay;
                if (netType.contains("LTE", Qt::CaseInsensitive)) {
                    netTypeDisplay = tr("LTE (4G)") + " - " + netType;
                } else if (netType.contains("UMTS", Qt::CaseInsensitive)) {
                    netTypeDisplay = tr("UMTS (3G)") + " - " + netType;
                } else if (netType.contains("GSM", Qt::CaseInsensitive)) {
                    netTypeDisplay = tr("GSM (2G)") + " - " + netType;
                } else {
                    netTypeDisplay = netType;
                }
                
                ui->lb_test_network_type->setText(netTypeDisplay);
                record_.network_type = netTypeDisplay;
                qDebug() << "[UI UPDATE] Network Type:" << netTypeDisplay;
                qDebug() << "[UI UPDATE] Operator:" << oper;
            }
        }
    }
    // 解析AT指令原始响应中的网络制式（+ZCELLINFO:）
    else if (line.contains("+ZCELLINFO:")) {
        // 匹配格式: +ZCELLINFO: 0XFFFE,0XFFFFFFFF,0X00000000,LTE B3,1600
        // 或: +ZCELLINFO: 0XFFFE,0XFFFFFFFF,PGSM_900
        QRegularExpression re("\\+ZCELLINFO:\\s*[^,]+,[^,]+,(?:[^,]+,)?([A-Z0-9_ ]+)");
        QRegularExpressionMatch match = re.match(line);
        if (match.hasMatch()) {
            QString netType = match.captured(1).trimmed();
            
            // 过滤掉无效的网络制式（如 0X00000000）
            if (!netType.isEmpty() && !netType.startsWith("0X")) {
                // 转换网络制式显示格式
                QString netTypeDisplay;
                if (netType.contains("LTE", Qt::CaseInsensitive)) {
                    netTypeDisplay = tr("LTE (4G)") + " - " + netType;
                } else if (netType.contains("UMTS", Qt::CaseInsensitive)) {
                    netTypeDisplay = tr("UMTS (3G)") + " - " + netType;
                } else if (netType.contains("GSM", Qt::CaseInsensitive) || netType.contains("PGSM", Qt::CaseInsensitive)) {
                    netTypeDisplay = tr("GSM (2G)") + " - " + netType;
                } else {
                    netTypeDisplay = netType;
                }
                
                ui->lb_test_network_type->setText(netTypeDisplay);
                record_.network_type = netTypeDisplay;
                qDebug() << "[UI UPDATE] Network Type (from AT response):" << netTypeDisplay;
            }
        }
    }
    // 解析SIM卡状态
    else if (line.contains("SIM卡检测通过")) {
        ui->lb_test_sim_network->setText(tr("Pass"));
        record_.net_status = zl::ESimNetStatus_Success;
        qDebug() << "[UI UPDATE] SIM Status: Pass";
    }
    // 解析测试失败（脚本中 [ERROR] 行，按关键词定位失败字段）
    else if (line.contains("[ERROR]")) {
        // IMEI 查询失败
        if ((line.contains("IMEI") || line.contains("+CGSN") || line.contains("+EGMR"))
            && ui->lb_test_iot_imei->text().isEmpty()) {
            setLabelFailed(ui->lb_test_iot_imei);
            record_.iot_imei = "测试失败";
            qDebug() << "[UI UPDATE] IMEI: FAILED";
        }
        // IOT 版本查询失败
        else if ((line.contains("GMI") || line.contains("固件版本") || line.contains("ATI"))
                 && ui->lb_test_iot_module_ver->text().isEmpty()) {
            setLabelFailed(ui->lb_test_iot_module_ver);
            record_.iot_module_id = "测试失败";
            qDebug() << "[UI UPDATE] IOT Version: FAILED";
        }
        // SIM ICCID 查询失败
        else if ((line.contains("ICCID") || line.contains("CCID") || line.contains("ZGETICCID") || line.contains("QCCID"))
                 && ui->lb_test_iccid->text().isEmpty()) {
            setLabelFailed(ui->lb_test_iccid);
            record_.iccid = "测试失败";
            qDebug() << "[UI UPDATE] ICCID: FAILED";
        }
        // 网络注册/SIM卡失败
        else if ((line.contains("CGREG") || line.contains("网络注册") || line.contains("SIM卡"))
                 && ui->lb_test_sim_network->text().isEmpty()) {
            setLabelFailed(ui->lb_test_sim_network);
            record_.net_status = zl::ESimNetStatus_Fail;
            qDebug() << "[UI UPDATE] Network/SIM: FAILED";
        }
    }
    // 脚本明确报告 SIM 卡检测失败
    else if (line.contains("SIM卡检测失败") || line.contains("SIM卡状态：检测失败")) {
        setLabelFailed(ui->lb_test_iccid);
        setLabelFailed(ui->lb_test_sim_network);
        record_.iccid = "测试失败";
        record_.net_status = zl::ESimNetStatus_Fail;
        qDebug() << "[UI UPDATE] SIM card detection: FAILED";
    }
}

// 新增函数：检查测试完成标志
void MainWnd::checkTestCompletion(const QString& output)
{
    // 检查测试阶段完成的标志（兼容中英文脚本输出）
    const bool completed =
        output.contains("模块测试完成（关键项通过）") ||
        output.contains("模块测试完成") ||
        output.contains("关键项通过") ||
        output.contains("Test phase completed", Qt::CaseInsensitive) ||
        output.contains("test completed successfully", Qt::CaseInsensitive);

    if (completed) {
        if (!m_testCompleted) {
            m_testCompleted = true;
            qDebug() << "[INFO] Test phase completed, saving results to database immediately";
            
            // 停止超时计时器
            if (m_timeoutTimer && m_timeoutTimer->isActive()) {
                m_timeoutTimer->stop();
                qDebug() << "[INFO] Timeout timer stopped - test phase completed";
            }
            
            // 显示提示信息
            ui->lb_test_cmd_excute_return_msg->appendPlainText("\n========================================");
            ui->lb_test_cmd_excute_return_msg->appendPlainText(tr("✅ Test phase completed successfully!"));
            ui->lb_test_cmd_excute_return_msg->appendPlainText(tr("📊 All test results have been displayed above."));
            ui->lb_test_cmd_excute_return_msg->appendPlainText("========================================");
            
            // 立即读取所有测试结果并保存到数据库
            qDebug() << "[INFO] Reading test results and saving to database...";
            readScriptResults();
            qDebug() << "[INFO] Test results saved to database successfully";
            
            // 显示监控模式提示
            ui->lb_test_cmd_excute_return_msg->appendPlainText("\n========================================");
            ui->lb_test_cmd_excute_return_msg->appendPlainText(tr("💾 Test results have been saved to database!"));
            ui->lb_test_cmd_excute_return_msg->appendPlainText(tr("🔄 Script is now running monitoring mode..."));
            ui->lb_test_cmd_excute_return_msg->appendPlainText(tr("📋 You can now use the interface normally, monitoring will continue in background."));
            ui->lb_test_cmd_excute_return_msg->appendPlainText(tr("ℹ️  The script will keep running for network monitoring."));
            ui->lb_test_cmd_excute_return_msg->appendPlainText("========================================");
            
            // 隐藏测试执行中遮罩
            showTestingOverlay(false);
            
            // 恢复界面状态，但不终止脚本
            setInputsEnabled(true);
            setButtonExecuting(ui->btn_nor_all_test, false);
            m_currentExecutingButton = nullptr;
        }
    }
}

// ===================== 测试执行中遮罩 =====================

void MainWnd::showTestingOverlay(bool show)
{
    if (show) {
        if (!m_testingOverlay) {
            m_testingOverlay = new TestProgressWidget(this);

            // 内层卡片 - 深色紧凑风格匹配设计稿
            QWidget* card = new QWidget(m_testingOverlay);
            card->setObjectName("testCard");
            card->setFixedSize(180, 100);
            card->setStyleSheet(
                "#testCard { background: #2B2B2B; border-radius: 14px; }"
            );

            QVBoxLayout* cardLayout = new QVBoxLayout(card);
            cardLayout->setContentsMargins(16, 14, 16, 14);
            cardLayout->setSpacing(6);

            // 旋转图标
            m_spinIconLabel = new QLabel(card);
            m_spinIconLabel->setFixedSize(26, 26);
            m_spinIconLabel->setScaledContents(false);
            m_spinIconLabel->setAlignment(Qt::AlignCenter);
            m_spinIconLabel->setStyleSheet("background: transparent;");

            // 提示文字（存引用供语言切换时更新）
            bool isCnOverlay = (APPMODEL()->CabinetLanguage() == zl::ELanguageType_Cn);
            m_spinTextLabel = new QLabel(isCnOverlay ? "测试执行中~" : "Testing~", card);
            m_spinTextLabel->setAlignment(Qt::AlignCenter);
            m_spinTextLabel->setStyleSheet(
                "font: bold 13pt '微软雅黑'; color: #FFFFFF; background: transparent;"
            );

            QHBoxLayout* iconRow = new QHBoxLayout();
            iconRow->setContentsMargins(0, 0, 0, 0);
            iconRow->addStretch();
            iconRow->addWidget(m_spinIconLabel);
            iconRow->addStretch();

            cardLayout->addLayout(iconRow);
            cardLayout->addWidget(m_spinTextLabel);
        }

        // 调整遮罩覆盖整个 MainWnd
        m_testingOverlay->setGeometry(0, 0, width(), height());

        // 将卡片居中
        QWidget* card = m_testingOverlay->findChild<QWidget*>("testCard");
        if (card) {
            card->move((width() - card->width()) / 2,
                       (height() - card->height()) / 2);
        }

        // 绘制细线辐射型 spinner（匹配设计稿：8根白色细线，渐变透明度）
        auto drawSpinner = [this]() {
            if (!m_spinIconLabel) return;
            // 以4倍分辨率绘制后缩放，确保细线平滑
            const int sz = 104;
            QPixmap pm(sz, sz);
            pm.fill(Qt::transparent);
            QPainter p(&pm);
            p.setRenderHint(QPainter::Antialiasing);
            p.translate(sz / 2.0, sz / 2.0);

            const int segments = 8;
            // 线段参数（相对于 sz/2 半径）
            const double innerR = 0.30 * (sz / 2.0);  // 线起点（离中心距离）
            const double outerR = 0.78 * (sz / 2.0);  // 线终点
            const double lineWidth = sz * 0.06;        // 线宽（对应 0.4px @26px）

            for (int i = 0; i < segments; i++) {
                // 当前高亮段 = m_spinAngle，越"旧"越透明
                int age = (i - m_spinAngle % segments + segments) % segments;
                // alpha: 当前段255，往后逐步衰减到约30
                int alpha = qMax(30, 255 - age * 32);

                p.save();
                p.rotate(i * (360.0 / segments));
                QPen pen(QColor(255, 255, 255, alpha));
                pen.setWidthF(lineWidth);
                pen.setCapStyle(Qt::RoundCap);
                p.setPen(pen);
                p.drawLine(QPointF(0, -innerR), QPointF(0, -outerR));
                p.restore();
            }
            p.end();
            // 缩放到 26×26（与设计稿一致）
            m_spinIconLabel->setPixmap(pm.scaled(26, 26, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        };

        // 第一帧
        drawSpinner();

        if (!m_spinTimer) {
            m_spinTimer = new QTimer(this);
            m_spinTimer->setInterval(80);
            connect(m_spinTimer, &QTimer::timeout, this, [this, drawSpinner]() mutable {
                if (!m_spinIconLabel) return;
                m_spinAngle = (m_spinAngle + 1) % 8;
                drawSpinner();
            });
        }
        m_spinAngle = 0;
        m_spinTimer->start();

        m_testingOverlay->raise();
        m_testingOverlay->show();
    } else {
        if (m_spinTimer) m_spinTimer->stop();
        if (m_testingOverlay) {
            m_testingOverlay->hide();
        }
    }
}

// ===================== 清空测试结果 =====================

void MainWnd::doClearTestResult()
{
    // 注意：不清空模块类型、APN、NET下拉框的选择，只清空测试结果显示
    // 保留用户的输入选择，方便连续测试
    
    // ui->le_apn->setCurrentIndex(-1);  // 保留APN选择
    // ui->le_net->setCurrentIndex(-1);  // 保留NET选择
    // ui->cb_module_type->setCurrentIndex(0);  // 保留模块类型选择

    resetVersionInfo();
    resetSimInfo();
    resetIotInfo();
    resetCmdResultInfo();
    resetRecord();
}

// ===================== 设置标签为失败状态 =====================

void MainWnd::setLabelFailed(QLabel* label)
{
    if (!label) return;
    label->setText(tr("测试失败"));
    label->setStyleSheet("color: red;");
}

void MainWnd::setLabelFailed(QLineEdit* edit)
{
    if (!edit) return;
    edit->setText(tr("测试失败"));
    edit->setStyleSheet("color: red;");
}
