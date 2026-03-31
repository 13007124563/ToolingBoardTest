#include "MainApp.h"

#include <QApplication>
#include <QCoreApplication>
#include <QDebug>
#include <QVBoxLayout>
#include <QStackedWidget>
#include <QScrollArea>
#include <QTimer>
#include <QDebug>
#include <QTranslator>

#include "DBConnector.h"

#include "AppModel.h"

#include "BannerWnd.h"
#include "MainWnd.h"
#include "BackendWnd.h"

#include "GlobalSignal.h"
#include "MovableInit.h"
#include "keyboard/InputMethodManager.h"

//#define INDEX_HOMEWND           0
#define INDEX_CONTAINER_WND		0

#define C_INDEX_MAIN            0
#define C_INDEX_BEWND           1


MainApp* MainApp::Instance_ = nullptr;

MainApp * MainApp::Instance()
{
    if (Instance_ == nullptr)
    {
        Instance_ = new MainApp;
    }

    return Instance_;
}

void MainApp::Release()
{
    delete Instance_;
    Instance_ = nullptr;
}

MainApp::MainApp()
: base_widget_(new QStackedWidget)
, container_(new QStackedWidget)
, main_wnd_(new MainWnd)
, banner_wnd_(new BannerWnd)
, backend_wnd_(new BackendWnd)
, m_workflow_status(EWF_Unknow)
, serial_connect_(Q_NULLPTR)
{
    qDebug() << "[NEED]" << "****** Start Application ******";
    base_widget_->setWindowFlags(Qt::FramelessWindowHint);
    base_widget_->installEventFilter(this);
    container_->setMouseTracking(true);

    base_widget_->setProperty("canMove", true);
    MovableInit::Instance()->start();

    init_configuration();
    init_database();

    init_serial_connect();

    // init view
    init_view();

    backend_wnd_->LoadData();

    update_app_configuration();

    event_select_language(APPMODEL()->CabinetLanguage());
}

MainApp::~MainApp()
{
    if (serial_connect_)
    {
        serial_connect_->disconnect();

        disconnect(serial_connect_, &CSerialCommandWrapper::commandResult, this, &MainApp::event_serial_command_result);

        disconnect(serial_connect_, &CSerialCommandWrapper::msgData, this, &MainApp::event_serial_msg_data);

        delete serial_connect_;
        serial_connect_ = Q_NULLPTR;
    }

    QObject::disconnect(GlobalSignal::getInstance(), &GlobalSignal::work_flow_change, this, &MainApp::event_work_flow_change);
    QObject::disconnect(GlobalSignal::getInstance(), &GlobalSignal::backend_page_change, this, &MainApp::event_backend_page_change);
    QObject::disconnect(GlobalSignal::getInstance(), &GlobalSignal::select_language, this, &MainApp::event_select_language);
    QObject::disconnect(GlobalSignal::getInstance(), &GlobalSignal::reconnect_com, this, &MainApp::event_reconnect_com);

    if (backend_wnd_) disconnect(backend_wnd_, &BackendWnd::configChanged, this, &MainApp::event_config_changed);

    base_widget_->close();
    base_widget_->deleteLater();

    zl::CDBConnector::Release();

    qDebug() << "[NEED]" << "****** Exit Application ******";

    QApplication::exit(0);
}

int32_t MainApp::Run()
{
    if (base_widget_) base_widget_->showNormal();

    // update cabinet information
    qDebug() << "[INFO]" << "update cabinet information...";

    return 0;
}

void MainApp::init_configuration()
{
    int32_t ret = CAppModel::Instance()->LoadAppConfiguration();
    if (ret != zl::EResult_Success)
    {
        qDebug() << "[ERROR]" << "load configurations failed: " << ret;
        return;
    }
    qDebug() << "[INFO]" << "load configuration success";
}

void MainApp::init_database()
{
    int32_t ret = zl::CDBConnector::Instance()->Init_sqlite();
    if (ret != zl::CDBConnector::EDBError_Success)
    {
        qDebug() << "[ERROR]" << "init database failed: " << ret;
        return;
    }
}

void MainApp::init_serial_connect()
{
#ifdef Q_OS_WIN
    // Windows平台：初始化串口通信
    qRegisterMetaType<zl::zlmodules::CommandResultType>("zl::zlmodules::CommandResultType");

    if (serial_connect_ == Q_NULLPTR)
        serial_connect_ = new CSerialCommandWrapper;

    if (!serial_connect_)
    {
        qDebug() << "[ERROR]" << "init scaner error, new object failed";
        return;
    }

    connect(serial_connect_, &CSerialCommandWrapper::commandResult,
        this, &MainApp::event_serial_command_result);

    connect(serial_connect_, &CSerialCommandWrapper::msgData,
        this, &MainApp::event_serial_msg_data);

    serial_port_connect();
#else
    // Linux平台：不使用串口，通过shell脚本测试
    qDebug() << "[INFO] Linux platform: Serial communication disabled, using shell script mode";
    serial_connect_ = new CSerialCommandWrapper; // 创建空对象避免nullptr
#endif
}

void MainApp::serial_port_connect()
{
#ifdef Q_OS_WIN
    if (!serial_connect_)
    {
        qDebug() << "[ERROR]" << "scaner port connect error, object is null";
        return;
    }

    auto config = APPMODEL()->Config().serial_config;

    bool isOK = true;
    int32_t cur_baudrate = config.baud_rate.toInt(&isOK);

    if (!isOK)
    {
        qDebug() << "[ERROR]" << "init scaner error, config baudrate is: " << config.baud_rate;
        return;
    }

    QString msg;

    int32_t ret = serial_connect_->connect(config.port, cur_baudrate);
    if (ret != 0)
    {
        qDebug() << "[ERROR]" << "init serial error, connect serial port failed";

        msg = tr("Serial %1, baudrate %2, connect failed").arg(config.port).arg(config.baud_rate);
    }
    else
    {
        qDebug() << "[INFO]" << "serial connect success, port: " << config.port << " baudrate: " << cur_baudrate;
    }

    if (main_wnd_) main_wnd_->SetConnectMsg(msg);
#else
    // Linux平台：无需串口连接
    qDebug() << "[INFO] Linux platform: skip serial port connection";
#endif
}

void MainApp::reconnectSerialConnect()
{
#ifdef Q_OS_WIN
    if (serial_connect_)
    {
        serial_connect_->disconnect();
    }

    serial_port_connect();
#else
    qDebug() << "[INFO] Linux platform: reconnect not needed";
#endif
}

void MainApp::init_view()
{
    qRegisterMetaType<zl::EWF>("zl::EWF");
    qRegisterMetaType<zl::EBP>("zl::EBP");
    qRegisterMetaType<zl::ELanguageType>("zl::ELanguageType");

    // Resolution: 1280 x 800
    // Banner: 1280 x 90
    // Content: 1280 x 710
    banner_wnd_->setFixedSize(1280, 90);
    main_wnd_->setFixedSize(1280, 710);
    backend_wnd_->setFixedSize(1280, 710);

    container_->setFixedSize(1280, 710);
    base_widget_->setFixedSize(1280, 800);

    container_->addWidget(main_wnd_);
    container_->addWidget(backend_wnd_);

    QWidget* w = new QWidget;
    QVBoxLayout* lay = new QVBoxLayout;
    lay->addWidget(banner_wnd_);
    lay->addWidget(container_);
    lay->setSpacing(0);
    lay->setContentsMargins(0, 0, 0, 0);
    w->setLayout(lay);
    base_widget_->addWidget(w);

    // 初始化屏幕键盘输入法（以顶层窗口为 parent，切换页面时键盘始终可见）
    InputMethodManager::instance(base_widget_);

    // GlobalSignal event
    QObject::connect(GlobalSignal::getInstance(), &GlobalSignal::work_flow_change, this, &MainApp::event_work_flow_change);
    QObject::connect(GlobalSignal::getInstance(), &GlobalSignal::backend_page_change, this, &MainApp::event_backend_page_change);
    QObject::connect(GlobalSignal::getInstance(), &GlobalSignal::select_language, this, &MainApp::event_select_language);
    QObject::connect(GlobalSignal::getInstance(), &GlobalSignal::reconnect_com, this, &MainApp::event_reconnect_com);

    // banner widget event
    connect(banner_wnd_, &BannerWnd::exitApp, this, [&]() {
        if (backend_wnd_) backend_wnd_->NotifyQuitBackend();
        exit_application();
    });

    connect(banner_wnd_, &BannerWnd::backToHome, this, [&]() {
        if (container_->currentIndex() == C_INDEX_BEWND)
        {
            set_current_view(EViewType_Home);

            if (banner_wnd_) banner_wnd_->Show_home_banner();

            if (backend_wnd_) backend_wnd_->NotifyQuitBackend();
        }
    });

    connect(banner_wnd_, &BannerWnd::enterBackend, this, &MainApp::event_enter_backend);

    // backend widget event
    connect(backend_wnd_, &BackendWnd::configChanged, this, &MainApp::event_config_changed);
    connect(backend_wnd_, &BackendWnd::requestBackToHome, this, [&]() {
        if (container_->currentIndex() == C_INDEX_BEWND)
        {
            set_current_view(EViewType_Home);
            if (banner_wnd_) banner_wnd_->Show_home_banner();
            if (backend_wnd_) backend_wnd_->NotifyQuitBackend();
        }
    });

    event_work_flow_change(EWF_Home);
}

void MainApp::update_app_configuration()
{
    // 使用 QObject 上下文查找翻译，与 AppModel.cpp 中注册的上下文保持一致
    QString title = QCoreApplication::translate("QObject", "IOT Test Tool");
    // Append version to title to match UI design: "Title(Version: 1.0...)"
    if (!APPMODEL()->Version().isEmpty()) {
        title += tr(" (Version: V%1)").arg(APPMODEL()->Version());
    }

    if (banner_wnd_) banner_wnd_->SetCabinetInfo(APPMODEL()->CabinetName(), title);
    if (backend_wnd_) backend_wnd_->UpdatePanelAdvance();

    // MainWnd version label removed from UI, so this call is no longer needed/valid
    // if (main_wnd_) main_wnd_->SetCabinetInfo(APPMODEL()->Version());
}

void MainApp::set_current_view(EViewType type)
{
    if (!base_widget_ || !container_) return;

    switch (type)
    {
    case EViewType_Home:
    {
        base_widget_->setCurrentIndex(INDEX_CONTAINER_WND);
        container_->setCurrentIndex(C_INDEX_MAIN);
    }
        break;
    case EViewType_Main:
    {
        base_widget_->setCurrentIndex(INDEX_CONTAINER_WND);
        container_->setCurrentIndex(C_INDEX_MAIN);
    }
        break;
    case EViewType_Backend:
    {
        base_widget_->setCurrentIndex(INDEX_CONTAINER_WND);
        container_->setCurrentIndex(C_INDEX_BEWND);
    }
        break;
    default:
        break;
    }
}

bool MainApp::Is_backend_show()
{
    if (!container_) return false;
    if (!base_widget_) return false;

    return (base_widget_->currentIndex() == INDEX_CONTAINER_WND && container_->currentIndex() == C_INDEX_BEWND);
}

void MainApp::exit_application()
{
    deleteLater();
    QApplication::exit(0);
}

bool MainApp::eventFilter(QObject * watched, QEvent * event)
{
    if (base_widget_ == qobject_cast<QWidget*>(watched))
    {
        if (event->type() == QEvent::Close)
        {
            exit_application();
        }
    }

    return QObject::eventFilter(watched, event);
}

void MainApp::event_work_flow_change(zl::EWF workFlow)
{

    set_current_view(workFlow == EWF_Home ? EViewType_Home:EViewType_Main);

//    zl::EWF pre = m_workflow_status;
    m_workflow_status = workFlow;

    if (banner_wnd_)
        banner_wnd_->SetBannerWorkFlow(workFlow);
    else
        qDebug() << "[ERROR]" << "work flow status changed, banner wnd is null";

//    if (main_wnd_)
//        main_wnd_->SetMainWndWorkFlow(workFlow, pre);
//    else
//        qDebug() << "[ERROR]" << "work flow status changed, main wnd is null";
}

void MainApp::event_backend_page_change(zl::EBP backendPage)
{
    if (!backend_wnd_)
    {
        qDebug() << "[ERROR]" << "backend page changed, backend wnd is null";
        return;
    }

    if (!banner_wnd_)
    {
        qDebug() << "[ERROR]" << "backend page changed, banner wnd is null";
        return;
    }

    set_current_view(EViewType_Backend);
    backend_wnd_->SetBackendPage(backendPage);

    banner_wnd_->SetBannerBackendModule(backendPage);

    if (backendPage == EBP_Select_page)
        banner_wnd_->Show_backend_select_banner();
    else
        banner_wnd_->Show_backend_page_banner();
}

void MainApp::event_select_language(zl::ELanguageType type)
{
    zl::ELanguageType lang = zl::ELanguageType(type);

    if (lang != zl::ELanguageType_Cn && lang != zl::ELanguageType_En) return;

    static QTranslator* translator;

    if (translator != NULL)
    {
        qApp->removeTranslator(translator);
        delete translator;
        translator = NULL;
    }

    translator = new QTranslator;
    switch (lang) {
    case zl::ELanguageType_Cn:
    {
        translator->load(":/ToolingBoardTest_cn.qm");
    }
        break;
    case zl::ELanguageType_En:
    {
        translator->load(":/ToolingBoardTest_en.qm");
    }
        break;
    default:
        return;
        break;
    }

    qApp->installTranslator(translator);

    // Must update model language BEFORE emitting switchLanguage,
    // so that lang_change() slots see the correct new language value.
    APPMODEL()->SetCabinetLanguage(lang);

    emit GlobalSignal::getInstance()->switchLanguage();

    // Re-apply configuration (e.g. Title with Version) after language switch
    update_app_configuration();

    APPMODEL()->SaveAppConfiguration();
}

void MainApp::event_enter_backend()
{
    if (base_widget_->currentIndex() != INDEX_CONTAINER_WND || container_->currentIndex() != C_INDEX_MAIN)
        return;

    set_current_view(EViewType_Backend);

    backend_wnd_->ShowBackendPage();
    banner_wnd_->Show_backend_select_banner();
    banner_wnd_->SetBannerBackendModule(EBP_Select_page);
}

void MainApp::event_config_changed()
{
    reconnectSerialConnect();
}

void MainApp::event_reconnect_com()
{
    reconnectSerialConnect();
}

void MainApp::event_serial_command_result(const unsigned char cmd, zl::zlmodules::CommandResultType type, QByteArray data)
{
    if (main_wnd_)
        main_wnd_->SerialCommandResult(cmd, type, data);
}

void MainApp::event_serial_msg_data(const unsigned char cmd, QByteArray data)
{
    if (main_wnd_)
        main_wnd_->SerialMsgInfo(cmd, data);
}
