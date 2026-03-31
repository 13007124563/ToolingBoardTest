#ifndef MSGWND_H
#define MSGWND_H

#include <QWidget>

namespace Ui {
class MsgWnd;
}

class MsgWnd : public QWidget
{
    Q_OBJECT

public:
    static void ShowNormalInfo(const QString content1, const QString content2 = "", const QString content3 = "", QWidget* parent = Q_NULLPTR, bool modal = true);

    // 确认清空测试结果（带图标的确认对话框）
    static void ShowClearConfirm(QWidget* parent = Q_NULLPTR, bool modal = true);
    static void ShowDeleteConfirm(QWidget* parent = Q_NULLPTR, bool modal = true);

    // 提示登录失败
    static void ShowFailMsgReturnHome(const QString content1, const QString content2 = "", const QString content3 = "", QWidget* parent = Q_NULLPTR, bool modal = true);

    // 确认接收到的信息
    static void ConfirmInfo(const QString content1, const QString content2, const QString content3, QWidget* parent = Q_NULLPTR, bool modal = true);

protected:

    enum EMsgBoxType {
        EMsgBoxType_NormalInfo = 0,
        EMsgBoxType_Confirm,
        EMsgBoxType_LoginFail,
        EMsgBoxType_CleanConfirm,  // 清空测试结果确认
        EMsgBoxType_DeleteConfirm   // 删除所选记录确认
    };

    enum EMsgBoxBottomBtn {
        EMsgBoxType_None = 0,
        EMsgBoxType_One = 1,
        EMsgBoxType_Two = 2
    };

    MsgWnd() = default;
    MsgWnd(EMsgBoxType type, QWidget* parent, QString content1, QString content2, QString content3, bool modal = true);

    ~MsgWnd();

    void show_timer(bool show, int32_t time = 10);

    void showEvent(QShowEvent *event) override;
    void closeEvent(QCloseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event) override;

    void resetContent(bool flag = true);

protected slots:

    void lang_change();
    void on_btn_nor_normal_ok_clicked();
    void on_btn_nor_normal_cancel_clicked();

    void on_btn_nor_save_input_clicked();
    void on_btn_nor_save_receive_clicked();

    void event_close_wnd();


private:
    void showTopleftQuitBtn(bool show);

    void showNormal(QString info1, QString info2, QString info3);

    void showConfirmInfo(QString info1, QString info2, QString info3);
private:
    static MsgWnd* g_msg_view;

    Ui::MsgWnd *ui;

    QWidget*		parent_;        // 容器
    EMsgBoxType     type_;          // 类型
    QString			content1_;      // 入参内容记录
    QString			content2_;      // 入参内容记录
    QString         content3_;      // 入参内容记录

    QTimer*			timer_;         // 倒计时时钟
    int32_t						timer_cnt_;                 // 当前显示的倒计时数字
    bool						timer_flag_;                // 是否显示倒计时
};

#endif // MSGWND_H
