#ifndef GLOBAL_SIGNAL_H
#define GLOBAL_SIGNAL_H

#include <QtCore/QObject>
#include <QtWidgets/QWidget>

#include "CommonType.h"

enum EUserConfirmType {
    EUserConfirmType_Unknown = -1,
    EUserConfirmType_ResetSender = 0,
    EUserConfirmType_CleanBox = 1
};

// 全局信号工具，收发全局信号。
class GlobalSignal : public QObject
{
	Q_OBJECT

public:

	static GlobalSignal* getInstance();


private:
	GlobalSignal();
	GlobalSignal(const GlobalSignal &);
	GlobalSignal& operator = (const GlobalSignal &);


private:
	static GlobalSignal* m_instance;


signals:
    // 前端界面page发生改变 向外通知
    void work_flow_change(zl::EWF workFlow);

    void msg_wnd_back_home();

    // 后台界面page发生改变 向外部通知
    void backend_page_change(zl::EBP backendPage);

    // 弹窗关闭
    void close_msg_wnd();

    // 用户确认结果
    void user_confirm(QString info);

    // 重连串口
    void reconnect_com();

    // 通知重置当前阶段的倒计时
    void reset_timer();

    // 通知选择当前语言
    void select_language(zl::ELanguageType type);

    // 切换语言
    void switchLanguage();
};



#endif // GLOBAL_SIGNAL_H
