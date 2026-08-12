#include "MsgWnd.h"
#include "ui_MsgWnd.h"

#include "MainApp.h"

#include <QApplication>
#include <QScreen>
#include <QTimer>
#include <QEvent>
#include <QPainter>

#include "GlobalSignal.h"

#include "AppModel.h"

#define QUIT_TIME       10
#define LONG_QUIT_TIME  20
#define MAX_QUIT_TIME   30

MsgWnd* MsgWnd::g_msg_view = Q_NULLPTR;

void MsgWnd::ShowNormalInfo(const QString content1, const QString content2, const QString content3, QWidget* parent, bool modal)
{    
    if (g_msg_view)
    {
        g_msg_view->showNormal(content1, content2, content3);
    }
    else
    {
        auto msgBox = new MsgWnd(EMsgBoxType_NormalInfo, parent, content1, content2, content3, modal);
        msgBox->show();

        g_msg_view = msgBox;
    }
}

void MsgWnd::ShowFailMsgReturnHome(const QString content1, const QString content2, const QString content3, QWidget* parent, bool modal)
{
    auto msgBox = new MsgWnd(EMsgBoxType_LoginFail, parent, content1, content2, content3, modal);
    msgBox->show();
}

void MsgWnd::ConfirmInfo(const QString content1, const QString content2, const QString content3, QWidget* parent, bool modal)
{    
    if (g_msg_view)
    {
        g_msg_view->showConfirmInfo(content1, content2, content3);
    }
    else
    {
        auto msgBox = new MsgWnd(EMsgBoxType_Confirm, parent, content1, content2, content3, modal);
        msgBox->show();

        g_msg_view = msgBox;
    }
}

void MsgWnd::ShowClearConfirm(QWidget* parent, bool modal)
{
    bool isCn = (APPMODEL()->CabinetLanguage() == zl::ELanguageType_Cn);
    QString title   = isCn ? "系统提示" : "System Notice";
    QString content = isCn ? "确定清空测试结果吗？" : "Clear all test results?";

    if (g_msg_view)
    {
        g_msg_view->content1_ = title;
        g_msg_view->content2_ = content;
        g_msg_view->content3_ = "CLEAR_TEST_RESULT";
        g_msg_view->type_ = EMsgBoxType_CleanConfirm;
        g_msg_view->ui->wnd_msg_content->setCurrentWidget(g_msg_view->ui->normal);
        g_msg_view->resetContent();
    }
    else
    {
        auto msgBox = new MsgWnd(EMsgBoxType_CleanConfirm, parent, title, content,
                                 "CLEAR_TEST_RESULT", modal);
        msgBox->show();
        g_msg_view = msgBox;
    }
}

void MsgWnd::ShowDeleteConfirm(QWidget* parent, bool modal)
{
    bool isCn = (APPMODEL()->CabinetLanguage() == zl::ELanguageType_Cn);
    QString title   = isCn ? "系统提示" : "System Notice";
    QString content = isCn ? "确定删除所选数据吗？" : "Delete selected records?";

    if (g_msg_view)
    {
        g_msg_view->content1_ = title;
        g_msg_view->content2_ = content;
        g_msg_view->content3_ = "DELETE_SELECTED_RECORDS";
        g_msg_view->type_ = EMsgBoxType_DeleteConfirm;
        g_msg_view->ui->wnd_msg_content->setCurrentWidget(g_msg_view->ui->normal);
        g_msg_view->resetContent();
    }
    else
    {
        auto msgBox = new MsgWnd(EMsgBoxType_DeleteConfirm, parent, title, content,
                                 "DELETE_SELECTED_RECORDS", modal);
        msgBox->show();
        g_msg_view = msgBox;
    }
}

MsgWnd::MsgWnd(EMsgBoxType type, QWidget *parent, QString content1, QString content2, QString content3, bool modal)
    :QWidget(parent)
    , ui(new Ui::MsgWnd)
    , parent_(parent)
    , type_(type)
    , content1_(content1)
    , content2_(content2)
    , content3_(content3)
    , timer_(new QTimer(this))
    , timer_cnt_(0)
    , timer_flag_(false)
{
    ui->setupUi(this);

    if (modal)
    {
        setAttribute(Qt::WA_ShowModal, true);
    }

    if (parent != Q_NULLPTR)
        parent_ = parent;
    else
        parent_ = MainApp::Instance()->GetBaseWidget();

    QRect container = parent_->geometry();
    QRect curPos = QRect(QPoint(0, 0), size());

    if (parent != Q_NULLPTR)
    {
        parent_->installEventFilter(this);
        setParent(parent_);

        curPos.moveCenter(parent_->mapToGlobal(container.center()));
    }
    else
    {
        curPos.moveCenter(container.center());
    }

    setGeometry(curPos);

    setWindowFlag(Qt::FramelessWindowHint);
    setWindowFlag(Qt::Tool);
    setAttribute(Qt::WA_AlwaysStackOnTop, true);
    setAttribute(Qt::WA_TranslucentBackground, true);

    QObject::connect(GlobalSignal::getInstance(), &GlobalSignal::close_msg_wnd, this, &MsgWnd::event_close_wnd);
    QObject::connect(GlobalSignal::getInstance(), &GlobalSignal::switchLanguage, this, &MsgWnd::lang_change);
}

MsgWnd::~MsgWnd()
{
    QObject::disconnect(GlobalSignal::getInstance(), &GlobalSignal::close_msg_wnd, this, &MsgWnd::event_close_wnd);
    QObject::disconnect(GlobalSignal::getInstance(), &GlobalSignal::switchLanguage, this, &MsgWnd::lang_change);

    timer_flag_ = false;
    if (timer_->isActive()) timer_->stop();

    delete ui;
}

void MsgWnd::lang_change()
{
    if (ui) ui->retranslateUi(this);
}

void MsgWnd::mousePressEvent(QMouseEvent * event)
{
    QWidget::mousePressEvent(event);
}

void MsgWnd::showEvent(QShowEvent * event)
{
    resetContent();

    switch (type_)
    {
    case EMsgBoxType_NormalInfo:
        ui->wnd_msg_content->setCurrentWidget(ui->normal);
        break;
    case EMsgBoxType_Confirm:
        ui->wnd_msg_content->setCurrentWidget(ui->page_confirm);
        break;
    case EMsgBoxType_LoginFail:
        ui->wnd_msg_content->setCurrentWidget(ui->normal);
        break;
    case EMsgBoxType_CleanConfirm:
        ui->wnd_msg_content->setCurrentWidget(ui->normal);
        break;
    case EMsgBoxType_DeleteConfirm:
        ui->wnd_msg_content->setCurrentWidget(ui->normal);
        break;
    default:
        ui->wnd_msg_content->setCurrentWidget(ui->normal);
        break;
    }

    QWidget::showEvent(event);
}

void MsgWnd::closeEvent(QCloseEvent * event)
{
    g_msg_view = Q_NULLPTR;

    deleteLater();

    QWidget::closeEvent(event);
}

void MsgWnd::event_close_wnd()
{
    close();
}

bool MsgWnd::eventFilter(QObject * watched, QEvent * event)
{
    if (qobject_cast<QWidget*>(watched) == parent_)
    {
        if (event->type() == QEvent::Resize || event->type() == QEvent::Move)
        {
            setGeometry(parent_->geometry());
            update();
        }
    }

    return QWidget::eventFilter(watched, event);
}

void MsgWnd::resetContent(bool flag)
{
    ui->lb_normal_tip1->clear();
    ui->lb_normal_tip2->clear();
    ui->lb_normal_tip3->clear();

    showTopleftQuitBtn(false);
    show_timer(false);

    if (!flag) return;

    switch (type_) {
    case EMsgBoxType_NormalInfo:
    {
        ui->btn_nor_normal_cancel->hide();

        ui->lb_normal_tip1->setVisible(!content1_.isEmpty());
        ui->lb_normal_tip2->setVisible(!content2_.isEmpty());
        ui->lb_normal_tip3->setVisible(!content3_.isEmpty());

        ui->lb_normal_tip1->setText(content1_);
        ui->lb_normal_tip2->setText(content2_);
        ui->lb_normal_tip3->setText(content3_);
    }
        break;
    case EMsgBoxType_Confirm:
    {
        ui->lb_title_confirm->setVisible(!content1_.isEmpty());
        ui->lb_tips_receive_info->setVisible(!content2_.isEmpty());

        ui->lb_title_confirm->setText(content1_);
        ui->lb_tips_receive_info->setText(content2_);

        ui->btn_nor_save_receive->setVisible(!content3_.isEmpty());
    }
        break;
    case EMsgBoxType_LoginFail:
    {
        ui->btn_nor_normal_cancel->hide();

        ui->lb_normal_tip1->setVisible(!content1_.isEmpty());
        ui->lb_normal_tip2->setVisible(!content2_.isEmpty());
        ui->lb_normal_tip3->setVisible(!content3_.isEmpty());

        ui->lb_normal_tip1->setText(content1_);
        ui->lb_normal_tip2->setText(content2_);
        ui->lb_normal_tip3->setText(content3_);

        qint32 time = QUIT_TIME;
        show_timer(true, time);
    }
        break;
    case EMsgBoxType_CleanConfirm:
    {
        bool isCn = (APPMODEL()->CabinetLanguage() == zl::ELanguageType_Cn);
        ui->btn_nor_normal_cancel->show();
        ui->btn_nor_normal_cancel->setText(isCn ? "取消" : "Cancel");
        ui->btn_nor_normal_ok->setText(isCn ? "确定" : "OK");
        ui->lb_normal_icon->setPixmap(
            QPixmap(":/img/backend/Frame.png").scaled(120, 120, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        ui->lb_normal_title->setText(content1_);
        ui->lb_normal_tip1->setVisible(true);
        ui->lb_normal_tip1->setText(content2_);
        ui->lb_normal_tip2->setVisible(false);
        ui->lb_normal_tip3->setVisible(false);
    }
        break;
    case EMsgBoxType_DeleteConfirm:
    {
        bool isCn = (APPMODEL()->CabinetLanguage() == zl::ELanguageType_Cn);
        ui->btn_nor_normal_cancel->show();
        ui->btn_nor_normal_cancel->setText(isCn ? "取消" : "Cancel");
        ui->btn_nor_normal_ok->setText(isCn ? "确定" : "OK");
        ui->lb_normal_icon->setPixmap(
            QPixmap(":/img/backend/Frame.png").scaled(120, 120, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        ui->lb_normal_title->setText(content1_);
        ui->lb_normal_tip1->setVisible(true);
        ui->lb_normal_tip1->setText(content2_);
        ui->lb_normal_tip2->setVisible(false);
        ui->lb_normal_tip3->setVisible(false);
    }
        break;
    default:
        break;
    }
}

void MsgWnd::showTopleftQuitBtn(bool show)
{
    ui->btn_top_left_back->setVisible(show);
}

void MsgWnd::on_btn_nor_normal_ok_clicked()
{
    if (type_ == EMsgBoxType_LoginFail)
    {
        emit GlobalSignal::getInstance()->msg_wnd_back_home();
    }
    else if (type_ == EMsgBoxType_CleanConfirm)
    {
        // 发出确认信号触发清空逻辑
        emit GlobalSignal::getInstance()->user_confirm(content3_);
    }
    else if (type_ == EMsgBoxType_DeleteConfirm)
    {
        emit GlobalSignal::getInstance()->user_confirm(content3_);
    }

    close();
}

void MsgWnd::on_btn_nor_normal_cancel_clicked()
{
    close();
}

void MsgWnd::on_btn_nor_save_input_clicked()
{
    QString input = ui->edt_input_info->text();

    if (input.isEmpty()) return;

    emit GlobalSignal::getInstance()->user_confirm(input);

    close();
}

void MsgWnd::on_btn_nor_save_receive_clicked()
{
    if (content3_.isEmpty()) return;

    emit GlobalSignal::getInstance()->user_confirm(content3_);

    close();
}

void MsgWnd::show_timer(bool show, int32_t time)
{
    if (show)
    {
        timer_->start(1000);
        timer_flag_ = true;
        timer_cnt_ = time;

        ui->lb_fetch_timer->show();
        ui->lb_fetch_timer->setText(timer_flag_ ? QString::number(timer_cnt_) : QString(""));

        connect(timer_, &QTimer::timeout, this, [&]() {

            if (!timer_flag_) return;

            if (timer_cnt_ <= 0)
            {
                timer_flag_ = false;

                if (type_ == EMsgBoxType_LoginFail)
                {
                    emit GlobalSignal::getInstance()->msg_wnd_back_home();
                }

                close();
                return;
            }

            ui->lb_fetch_timer->setText(QString::number(timer_cnt_));
            timer_cnt_ -= 1;
            timer_cnt_ = timer_cnt_ > 0 ? timer_cnt_ : 0;
        });
    }
    else
    {
        if (timer_->isActive()) timer_->stop();

        ui->lb_fetch_timer->hide();
    }
}

void MsgWnd::showNormal(QString info1, QString info2, QString info3)
{
    content1_ = info1;
    content2_ = info2;
    content3_ = info3;

    type_ = EMsgBoxType_NormalInfo;

    ui->wnd_msg_content->setCurrentWidget(ui->normal);
    resetContent();
}

void MsgWnd::showConfirmInfo(QString info1, QString info2, QString info3)
{
    content1_ = info1;
    content2_ = info2;
    content3_ = info3;

    type_ = EMsgBoxType_Confirm;

    ui->wnd_msg_content->setCurrentWidget(ui->page_confirm);
    resetContent();
}
