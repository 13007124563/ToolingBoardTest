#ifndef BANNERWND_H
#define BANNERWND_H

#include <QWidget>
#include "CommonType.h"

class QTimer;
class QComboBox;

namespace Ui {
class BannerWnd;
}

class BannerWnd : public QWidget
{
    Q_OBJECT

public:
    explicit BannerWnd(QWidget *parent = 0);
    ~BannerWnd();

    static QString CurrentDayOfWeek();

    void Show_home_banner();

    void Show_backend_select_banner();
    void Show_backend_page_banner();

    void SetCabinetInfo(QString name, QString title);

    void SetBannerWorkFlow(zl::EWF workFlow);

    void SetBannerBackendModule(zl::EBP backendPage);

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

    void set_backend_title(zl::EBP backendPage);

    void toggle_show_top_head(bool show);

signals:
    // 后台退出软件
    void exitApp();

    // 后台回到首页
    void backToHome();

    // 进入后台
    void enterBackend();

protected slots:

    void lang_change();

    void on_btn_quit_bk_clicked();

    void on_btn_exit_clicked();

    void on_btn_bk_back_clicked();

private:
    Ui::BannerWnd *ui;

    QTimer*						m_timer;
    int32_t						m_timer_cnt;                 // 当前显示的倒计时数字
    bool						m_timer_flag;

    zl::EBP                     m_backend_page;                     // 后台当前操作模块
    QComboBox*                  m_langCombo{nullptr};               // 语言切换下拉框
};

#endif // BANNERWND_H
