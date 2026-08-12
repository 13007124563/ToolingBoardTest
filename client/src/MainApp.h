#ifndef MAINAPP_H
#define MAINAPP_H

#include <QObject>

#include "CommonType.h"
#include <QPixmap>
#include <QMutex>

// 包含CSerialCommandWrapper头文件以获取类型定义
// Linux和Windows都需要这个头文件来访问CommandResultType等类型
#include "CSerialCommandWrapper.h"

//class HomeWnd;
class BannerWnd;
class MainWnd;
class BackendWnd;

class QStackedWidget;

using namespace zl;
class MainApp : public QObject
{
    Q_OBJECT
public:
    static MainApp* Instance();
    static void Release();

    int32_t Run();

    QWidget* GetBaseWidget(){ return (QWidget*)base_widget_;}

    EWF CurrentWorkFlow() { return m_workflow_status;}

    CSerialCommandWrapper* GetSerialConnectObj() { return serial_connect_;}

    bool Is_backend_show();

protected:

    enum EViewType {
        EViewType_Home      = 0,
        EViewType_Main      = 1,
        EViewType_Backend   = 2
    };

    void set_current_view(EViewType type);

    void init_view();

    void init_configuration();
    void init_database();

    void init_serial_connect();

    void serial_port_connect();

    void exit_application();

    void update_app_configuration();

    bool eventFilter(QObject *watched, QEvent *event) override;

    // 重连串口
    void reconnectSerialConnect();
private:
    MainApp();
    ~MainApp();

private slots:

    void event_work_flow_change(zl::EWF workFlow);

    void event_backend_page_change(zl::EBP backendPage);

    void event_select_language(zl::ELanguageType type);

    void event_enter_backend();

    void event_config_changed();

    void event_reconnect_com();

    void event_serial_command_result(const unsigned char cmd, zl::zlmodules::CommandResultType type, QByteArray data);

    void event_serial_msg_data(const unsigned char cmd, QByteArray data);

public:

    QStackedWidget*         base_widget_;
    QStackedWidget*			container_;

    MainWnd*				main_wnd_;
    BannerWnd*              banner_wnd_;
    BackendWnd*             backend_wnd_;

private:
    static MainApp*			Instance_;

    EWF						m_workflow_status;

    // 扫描器
    CSerialCommandWrapper*      serial_connect_;
};

#endif // MAINAPP_H
