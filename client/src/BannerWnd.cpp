#include "BannerWnd.h"
#include "ui_BannerWnd.h"

#include "GlobalSignal.h"
#include "AppModel.h"

#include <QTimer>
#include <QDateTime>
#include <QDate>
#include <QDebug>
#include <QLabel>
#include <QComboBox>
#include <QHBoxLayout>

#include "MainApp.h"

#define DATETIME_FORMAT_DATE          "yyyy-MM-dd"
#define DATETIME_FORMAT_TIME          "hh:mm:ss"

using namespace zl;

BannerWnd::BannerWnd(QWidget *parent) :
    QWidget(parent)
  , ui(new Ui::BannerWnd)
  , m_timer(new QTimer(this))
  , m_timer_cnt(0)
  , m_timer_flag(false)
  , m_backend_page(EBP_Unknow)
{
    ui->setupUi(this);

    this->setFixedSize(this->width(), this->height());

    // init
    ui->lb_title->installEventFilter(this);

    // 创建语言切换控件
    QWidget* langWidget = new QWidget(ui->wnd_top_info);
    QHBoxLayout* langLayout = new QHBoxLayout(langWidget);
    langLayout->setContentsMargins(0, 0, 20, 0);
    langLayout->setSpacing(5);

    QLabel* lbLang = new QLabel(tr("语言/language:"), langWidget);
    lbLang->setStyleSheet("font: 12pt '微软雅黑'; color: #333333;");

    QComboBox* cbLang = new QComboBox(langWidget);
    cbLang->addItem("中文");
    cbLang->addItem("English");
    m_langCombo = cbLang;
    // 根据当前配置的语言设置下拉框索引，保持UI与实际语言一致
    cbLang->setCurrentIndex(APPMODEL()->CabinetLanguage() == zl::ELanguageType_Cn ? 0 : 1);
    cbLang->setFixedSize(100, 30);
    cbLang->setStyleSheet(
        "QComboBox { border: 1px solid #CCCCCC; border-radius: 4px; padding: 2px; "
        "            font: 10pt '微软雅黑'; color: #333333; background: #FFFFFF; } "
        "QComboBox::drop-down { subcontrol-origin: padding; subcontrol-position: top right; "
        "    width: 30px; border-left: none; border-top-right-radius: 3px; "
        "    border-bottom-right-radius: 3px; background: transparent; } "
        "QComboBox::down-arrow { image: url(:/img/backend/icon-up.png); width: 20px; height: 13px; }");
    langLayout->addWidget(lbLang);
    langLayout->addWidget(cbLang);

    // 在 logo 和标题之间动态插入竖线分隔符，并插入语言切换控件
    QHBoxLayout* topLayout = qobject_cast<QHBoxLayout*>(ui->wnd_top_info->layout());
    if (topLayout) {
        // 初始 layout: [lb_logo(0), lb_title(1), wnd_time(2)]
        // 步骤1: 在 index=1 前插入 16px + 竖线 + 16px
        QWidget* sep = new QWidget(ui->wnd_top_info);
        sep->setFixedSize(2, 50);
        sep->setStyleSheet("background-color:#CCCCCC;");
        topLayout->insertSpacing(1, 16);
        topLayout->insertWidget(2, sep);
        topLayout->insertSpacing(3, 16);
        // 现在: lb_logo(0) sp(1) sep(2) sp(3) lb_title(4) wnd_time(5)

        // 步遹2: 在 lb_title(4) 与 wnd_time 之间插入 stretch + langWidget，使语言框靠右贴近时间显示
        topLayout->insertStretch(5);       // 拉伸在 lb_title(4) 之后
        topLayout->insertWidget(6, langWidget);  // langWidget 在 stretch(5) 之后、wnd_time 之前
    }

    // Connect Language Switch
    connect(cbLang, static_cast<void(QComboBox::*)(int)>(&QComboBox::activated), this, [=](int index){
         if (index == 0) GlobalSignal::getInstance()->select_language(zl::ELanguageType_Cn);
         else GlobalSignal::getInstance()->select_language(zl::ELanguageType_En);
    });

    QObject::connect(GlobalSignal::getInstance(), &GlobalSignal::switchLanguage, this, &BannerWnd::lang_change);

    // show date time
    auto timer = new QTimer(this);
    timer->start(1000);
    connect(timer, &QTimer::timeout, this, [&]() {
        // lb_day_time reused for Date
        ui->lb_day_time->setText(QDateTime::currentDateTime().toString(DATETIME_FORMAT_DATE));
        // lb_date_time reused for Time
        ui->lb_date_time->setText(QDateTime::currentDateTime().toString(DATETIME_FORMAT_TIME));
    });
}

BannerWnd::~BannerWnd()
{
    QObject::disconnect(GlobalSignal::getInstance(), &GlobalSignal::switchLanguage, this, &BannerWnd::lang_change);

    if (m_timer->isActive()) m_timer->stop();

    delete ui;
}

void BannerWnd::lang_change()
{
    if (ui) ui->retranslateUi(this);

    // BannerWnd 在配置加载前已构造，语言切换时需要显式同步下拉框索引。
    if (m_langCombo) {
        m_langCombo->setCurrentIndex(APPMODEL()->CabinetLanguage() == zl::ELanguageType_Cn ? 0 : 1);
    }

    // retranslateUi 不会重置 notr 标识的控件，但为了安全立即刷新时间，防止闪烁
    ui->lb_day_time->setText(QDateTime::currentDateTime().toString(DATETIME_FORMAT_DATE));
    ui->lb_date_time->setText(QDateTime::currentDateTime().toString(DATETIME_FORMAT_TIME));

    set_backend_title(m_backend_page);
}

QString BannerWnd::CurrentDayOfWeek()
{
    QString dayOfWeek = "";

    int32_t index = QDate::currentDate().dayOfWeek();

    switch (index) {
    case 1:
       dayOfWeek = tr("Mon");
        break;
    case 2:
       dayOfWeek = tr("Tue");
        break;
    case 3:
       dayOfWeek = tr("Wed");
        break;
    case 4:
       dayOfWeek = tr("Thu");
        break;
    case 5:
       dayOfWeek = tr("Fri");
        break;
    case 6:
       dayOfWeek = tr("Sat");
        break;
    case 7:
       dayOfWeek = tr("Sun");
        break;
    default:
        break;
    }

    return dayOfWeek;
}

bool BannerWnd::eventFilter(QObject * watched, QEvent * event)
{
    // Double-click on title to enter backend is disabled per user requirement
    // if (qobject_cast<QLabel*>(watched) == ui->lb_title)
    // {
    //     if (event->type() == QEvent::MouseButtonDblClick)
    //     {
    //         emit enterBackend();
    //     }
    // }

    return QWidget::eventFilter(watched, event);
}

void BannerWnd::Show_home_banner()
{
    toggle_show_top_head(true);
}

void BannerWnd::Show_backend_select_banner()
{
    ui->stackedWidget_banner->setCurrentWidget(ui->page_back_sel);

    toggle_show_top_head(false);
}

void BannerWnd::Show_backend_page_banner()
{
    toggle_show_top_head(true);
}

void BannerWnd::SetCabinetInfo(QString name, QString title)
{
    Q_UNUSED(name);

    ui->lb_title->setText(title);
}

void BannerWnd::SetBannerWorkFlow(zl::EWF workFlow)
{
    if (workFlow == EWF_Home)
        Show_home_banner();
}

void BannerWnd::SetBannerBackendModule(zl::EBP backendPage)
{
    m_backend_page = backendPage;

    set_backend_title(backendPage);
}

void BannerWnd::set_backend_title(zl::EBP backendPage)
{
    QString backend_title = "";

    switch (backendPage)
    {
    case zl::EBP_Select_page:
        break;
    case zl::EBP_Record_page:
        backend_title = tr("Query Test Record");
        break;
    case zl::EBP_Advance_page:
        backend_title = tr("Setting");
        break;
    default:
        break;
    }

    ui->lb_backend_title->setText(backend_title);
}

void BannerWnd::toggle_show_top_head(bool show)
{
    ui->wnd_top_info->setVisible(show);
    ui->stackedWidget_banner->setVisible(!show);
}

void BannerWnd::on_btn_quit_bk_clicked()
{
    emit backToHome();
}

void BannerWnd::on_btn_exit_clicked()
{
    emit exitApp();
}

void BannerWnd::on_btn_bk_back_clicked()
{
    emit GlobalSignal::getInstance()->backend_page_change(zl::EBP_Select_page);
}
