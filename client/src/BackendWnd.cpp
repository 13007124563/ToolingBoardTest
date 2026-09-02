#include "BackendWnd.h"
#include "ui_BackendWnd.h"

#include <QButtonGroup>
#include <QAbstractButton>
#include <QPushButton>

#include <QStandardItem>
#include <QStandardItemModel>
#include <QHeaderView>
#include <QCheckBox>
#include <QHBoxLayout>
#include <QCheckBox>
#include <QHBoxLayout>
#include <QCheckBox>
#include <QHBoxLayout>

// Qt6兼容性：QTextCodec被移除，使用QStringConverter
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <QStringConverter>
#else
#include <QTextCodec>
#endif

#include <QTimer>
#include <QComboBox>
#include <QListView>
#include <QCompleter>
#include <QStringListModel>
#include <QDebug>

#include "AppModel.h"
#include "BannerWnd.h"

#include "commondefine.h"
#include "tablehelper.h"
// #include "excelhelper.h"  // Disabled for Linux compatibility

#include "TestRecordManager.h"
#include "DBConnector.h"
#include "MsgWnd.h"
#include "protocol/protocolconstants.h"

#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>
#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QLabel>
#include <QPainter>
#include <QMouseEvent>
#include <QStyledItemDelegate>

// 复选框自定义 delegate：绘制白底灰框 / 蓝底白勾
class CheckboxDelegate : public QStyledItemDelegate {
public:
    explicit CheckboxDelegate(QObject* parent = nullptr) : QStyledItemDelegate(parent) {}

    void paint(QPainter* painter, const QStyleOptionViewItem& option,
               const QModelIndex& index) const override
    {
        if (index.column() != 0) {
            QStyledItemDelegate::paint(painter, option, index);
            return;
        }
        bool checked = index.data(Qt::CheckStateRole).toInt() == Qt::Checked;
        QRect rect = option.rect;

        // 先用白色填充整个单元格，防止行选中蓝色背景覆盖复选框
        painter->fillRect(rect, Qt::white);

        int sz = 18;
        int x = rect.x() + (rect.width()  - sz) / 2;
        int y = rect.y() + (rect.height() - sz) / 2;
        QRect box(x, y, sz, sz);

        painter->save();
        painter->setRenderHint(QPainter::Antialiasing);
        if (checked) {
            painter->setBrush(QColor("#005FA7"));
            painter->setPen(QPen(QColor("#005FA7"), 1.5));
            painter->drawRoundedRect(box, 3, 3);
            // 白色 ✔
            painter->setPen(QPen(Qt::white, 2.5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
            QPolygonF tick;
            tick << QPointF(x + 3,  y + 9)
                 << QPointF(x + 7,  y + 13)
                 << QPointF(x + 15, y + 4);
            painter->drawPolyline(tick);
        } else {
            painter->setBrush(Qt::white);
            painter->setPen(QPen(QColor("#999999"), 1.5));
            painter->drawRoundedRect(box, 3, 3);
        }
        painter->restore();
    }

    bool editorEvent(QEvent* event, QAbstractItemModel* model,
                     const QStyleOptionViewItem& /*option*/,
                     const QModelIndex& index) override
    {
        if (index.column() == 0 && event->type() == QEvent::MouseButtonRelease) {
            Qt::CheckState cur = (Qt::CheckState)index.data(Qt::CheckStateRole).toInt();
            model->setData(index,
                           cur == Qt::Checked ? Qt::Unchecked : Qt::Checked,
                           Qt::CheckStateRole);
            return true;
        }
        return false;
    }
};

#define CB_ALL_ITEM                 "ALL"
#define CB_DEFAULT_ITEM             "NONE"

BackendWnd::BackendWnd(QWidget *parent) :
    QWidget(parent)
  , ui(new Ui::BackendWnd)
  , btnGpBackendSel(new QButtonGroup())
  , btnGpRecordSel(nullptr)
  , model_sim_record_result_(new QStandardItemModel)
  , model_iot_record_result_(new QStandardItemModel)
  , model_serial_record_result_(new QStandardItemModel)
  , model_extra_record_result_(new QStandardItemModel)
  , sim_header_checkbox_(nullptr)
  , iot_header_checkbox_(nullptr)
  , serial_header_checkbox_(nullptr)
  , extra_header_checkbox_(nullptr)
  , completer_sim_page_sim_iccid(new QCompleter(this))
  , completer_sim_page_firmware_version(new QCompleter(this))
  , completer_iot_page_iot_module_id(new QCompleter(this))
  , completer_iot_page_iot_imei(new QCompleter(this))
  , completer_iot_page_firmware_version(new QCompleter(this))
{
    ui->setupUi(this);

    init();

    QObject::connect(GlobalSignal::getInstance(), &GlobalSignal::switchLanguage, this, &BackendWnd::lang_change);
    QObject::connect(GlobalSignal::getInstance(), &GlobalSignal::user_confirm, this, &BackendWnd::event_user_confirm);
}

BackendWnd::~BackendWnd()
{
    QObject::disconnect(GlobalSignal::getInstance(), &GlobalSignal::switchLanguage, this, &BackendWnd::lang_change);
    QObject::disconnect(GlobalSignal::getInstance(), &GlobalSignal::user_confirm, this, &BackendWnd::event_user_confirm);

    sim_record_vec_.clear();
    iot_record_vec_.clear();
    serial_record_vec_.clear();

    if (btnGpBackendSel)
        disconnect(btnGpBackendSel, nullptr, this, nullptr);

    if (btnGpRecordSel)
        disconnect(btnGpRecordSel, nullptr, this, nullptr);

    if (btnGpBackendSel)
    {
        delete btnGpBackendSel;
        btnGpBackendSel = Q_NULLPTR;
    }

    if (btnGpRecordSel)
    {
        delete btnGpRecordSel;
        btnGpRecordSel = Q_NULLPTR;
    }

    if (model_sim_record_result_)
    {
        delete model_sim_record_result_;
        model_sim_record_result_ = Q_NULLPTR;
    }

    if (model_iot_record_result_)
    {
        delete model_iot_record_result_;
        model_iot_record_result_ = Q_NULLPTR;
    }

    if (model_serial_record_result_)
    {
        delete model_serial_record_result_;
        model_serial_record_result_ = Q_NULLPTR;
    }

    if (model_extra_record_result_)
    {
        delete model_extra_record_result_;
        model_extra_record_result_ = Q_NULLPTR;
    }

    delete ui;
}

void BackendWnd::lang_change()
{
    if (ui) ui->retranslateUi(this);

    init_table_view();

    init_sim_page_combo_normal();
    init_iot_page_combo_normal();
    init_serial_page_combo_normal();
    init_extra_page_combo_normal();
}

void BackendWnd::ShowBackendPage()
{
    ui->btn_page_record->setEnabled(true);
    ui->btn_page_advance->setEnabled(true);

    SetBackendPage(zl::EBP_Select_page);
}

void BackendWnd::SetBackendPage(zl::EBP backendPage)
{
    ui->stackedWidget_bk->setCurrentIndex(backendPage);

    if (backendPage == EBP_Record_page)
    {
        update_combo_completer();
    }
}

void BackendWnd::NotifyQuitBackend()
{

}

void BackendWnd::LoadData()
{
    update_advance_info();
}

void BackendWnd::UpdatePanelAdvance()
{
    update_advance_info();
}

void BackendWnd::init()
{
    initSelectPage();

    init_table_view();

    init_record_page();

    init_advance_page();
}

void BackendWnd::init_table_view()
{
    // sim卡 测试记录查询结果列表 - 添加复选框列
    QStringList list_sim_record = { "", tr("Serial Number"), tr("Sim ICCID"), tr("Network Status"), tr("Network Type"), tr("Signal Strength"), tr("Test Time"), tr("Operation")};
    QVector<int> widths_sim_record = {50, 170, 220, 120, 110, 100, 180, 90};

    TableHelper::initTableHeader(list_sim_record, widths_sim_record, model_sim_record_result_, ui->tb_sim_record_result);
    
    // 设置第一列为复选框列，允许编辑以交互复选框
    ui->tb_sim_record_result->setSelectionMode(QAbstractItemView::NoSelection);
    ui->tb_sim_record_result->horizontalHeader()->setFixedHeight(40);
    // 测试时间列（index 6）自动拉伸充满剩余宽度，操作列（index 7）固定 90px
    ui->tb_sim_record_result->horizontalHeader()->setSectionResizeMode(6, QHeaderView::Stretch);
    ui->tb_sim_record_result->horizontalHeader()->setSectionResizeMode(7, QHeaderView::Fixed);
    ui->tb_sim_record_result->horizontalHeader()->resizeSection(7, 90);
    // 隐藏左侧行号列
    ui->tb_sim_record_result->verticalHeader()->setVisible(false);
    // 使用自定义 delegate 渲染复选框（白底灰框/蓝底白勾）
    ui->tb_sim_record_result->setItemDelegateForColumn(0, new CheckboxDelegate(this));
    ui->tb_sim_record_result->setStyleSheet("");  // 清除旧 indicator 样式
    
    // 在表头第一列添加全选复选框 - 只在首次初始化时创建，语言切换时仅更新表头文字
    if (!sim_header_checkbox_) {
        qDebug() << "[DEBUG] SIM header checkbox created (first init)";
        sim_header_checkbox_ = new QCheckBox(ui->tb_sim_record_result->horizontalHeader());
        sim_header_checkbox_->setGeometry(15, 11, 25, 25);
        // Qt 样式表不支持 ::after 伪元素，使用简单的颜色和边框样式
        sim_header_checkbox_->setStyleSheet(
            "QCheckBox::indicator { "
            "   width: 18px; "
            "   height: 18px; "
            "   border: 2px solid #999999; "
            "   background-color: #FFFFFF; "
            "   border-radius: 3px; "
            "} "
            "QCheckBox::indicator:checked { "
            "   background-color: #005FA7; "
            "   border: 2px solid #005FA7; "
            "} "
            "QCheckBox::indicator:hover { "
            "   border: 2px solid #005FA7; "
            "}"
        );
        connect(sim_header_checkbox_, &QCheckBox::clicked, this, &BackendWnd::on_sim_header_checkbox_clicked);
    }
    sim_header_checkbox_->show();
    sim_header_checkbox_->raise();
    
    qDebug() << "[DEBUG] SIM table initialized with columns:" << list_sim_record;

    // IOT 测试记录查询结果列表 - 添加复选框列
    QStringList list_iot_record = { "", tr("Serial Number"), tr("IOT Version"), tr("IOT IMEI"), tr("Test Time"), tr("Operation")};
    QVector<int> widths_iot_record = {50, 190, 160, 220, 200, 90};

    TableHelper::initTableHeader(list_iot_record, widths_iot_record, model_iot_record_result_, ui->tb_iot_record_result);
    
    // 设置IOT表格的选择行为
    ui->tb_iot_record_result->setSelectionMode(QAbstractItemView::NoSelection);
    ui->tb_iot_record_result->horizontalHeader()->setFixedHeight(40);
    // 测试时间列（index 4）自动拉伸充满剩余宽度，操作列（index 5）固定 90px
    ui->tb_iot_record_result->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Stretch);
    ui->tb_iot_record_result->horizontalHeader()->setSectionResizeMode(5, QHeaderView::Fixed);
    ui->tb_iot_record_result->horizontalHeader()->resizeSection(5, 90);
    // 隐藏左侧行号列
    ui->tb_iot_record_result->verticalHeader()->setVisible(false);
    // 使用自定义 delegate 渲染复选框（白底灰框/蓝底白勾）
    ui->tb_iot_record_result->setItemDelegateForColumn(0, new CheckboxDelegate(this));
    ui->tb_iot_record_result->setStyleSheet("");  // 清除旧 indicator 样式
    
    // 在IOT表头第一列添加全选复选框 - 只在首次初始化时创建，语言切换时仅更新表头文字
    if (!iot_header_checkbox_) {
        qDebug() << "[DEBUG] IOT header checkbox created (first init)";
        iot_header_checkbox_ = new QCheckBox(ui->tb_iot_record_result->horizontalHeader());
        iot_header_checkbox_->setGeometry(15, 11, 25, 25);
        // Qt 样式表不支持 ::after 伪元素，使用简单的颜色和边框样式
        iot_header_checkbox_->setStyleSheet(
            "QCheckBox::indicator { "
            "   width: 18px; "
            "   height: 18px; "
            "   border: 2px solid #999999; "
            "   background-color: #FFFFFF; "
            "   border-radius: 3px; "
            "} "
            "QCheckBox::indicator:checked { "
            "   background-color: #005FA7; "
            "   border: 2px solid #005FA7; "
            "} "
            "QCheckBox::indicator:hover { "
            "   border: 2px solid #005FA7; "
            "}"
        );
        connect(iot_header_checkbox_, &QCheckBox::clicked, this, &BackendWnd::on_iot_header_checkbox_clicked);
    }
    iot_header_checkbox_->show();
    iot_header_checkbox_->raise();
    
    qDebug() << "[DEBUG] IOT table initialized with columns:" << list_iot_record;

    // 串口测试记录查询结果列表
    QStringList list_serial_record = { "", tr("Serial Number"), tr("Command"), tr("Test Result"), tr("Test Time"), tr("Operation")};
    const bool isCn = (APPMODEL()->CabinetLanguage() == zl::ELanguageType_Cn);
    if (isCn) {
        list_serial_record = QStringList() << "" << QStringLiteral("流水号") << QStringLiteral("命令字")
                                           << QStringLiteral("测试结果") << QStringLiteral("测试时间") << QStringLiteral("操作");
    }
    QVector<int> widths_serial_record = {50, 190, 160, 280, 200, 90};

    TableHelper::initTableHeader(list_serial_record, widths_serial_record, model_serial_record_result_, ui->tb_serial_record_result);

    ui->tb_serial_record_result->setSelectionMode(QAbstractItemView::NoSelection);
    ui->tb_serial_record_result->horizontalHeader()->setFixedHeight(40);
    ui->tb_serial_record_result->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
    ui->tb_serial_record_result->horizontalHeader()->setSectionResizeMode(5, QHeaderView::Fixed);
    ui->tb_serial_record_result->horizontalHeader()->resizeSection(5, 90);
    ui->tb_serial_record_result->verticalHeader()->setVisible(false);
    ui->tb_serial_record_result->setItemDelegateForColumn(0, new CheckboxDelegate(this));
    ui->tb_serial_record_result->setStyleSheet("");

    if (!serial_header_checkbox_) {
        qDebug() << "[DEBUG] Serial header checkbox created (first init)";
        serial_header_checkbox_ = new QCheckBox(ui->tb_serial_record_result->horizontalHeader());
        serial_header_checkbox_->setGeometry(15, 11, 25, 25);
        serial_header_checkbox_->setStyleSheet(
            "QCheckBox::indicator { "
            "   width: 18px; "
            "   height: 18px; "
            "   border: 2px solid #999999; "
            "   background-color: #FFFFFF; "
            "   border-radius: 3px; "
            "} "
            "QCheckBox::indicator:checked { "
            "   background-color: #005FA7; "
            "   border: 2px solid #005FA7; "
            "} "
            "QCheckBox::indicator:hover { "
            "   border: 2px solid #005FA7; "
            "}"
        );
        connect(serial_header_checkbox_, &QCheckBox::clicked, this, &BackendWnd::on_serial_header_checkbox_clicked);
    }
    serial_header_checkbox_->show();
    serial_header_checkbox_->raise();

    qDebug() << "[DEBUG] Serial table initialized with columns:" << list_serial_record;

    // 新增测试记录查询结果列表
    QStringList list_extra_record = { "", tr("Serial Number"), tr("Test Type"), tr("Test Result"), tr("Test Time"), tr("Operation")};
    if (isCn) {
        list_extra_record = QStringList() << "" << QStringLiteral("流水号") << QStringLiteral("测试类型")
                                          << QStringLiteral("测试结果") << QStringLiteral("测试时间") << QStringLiteral("操作");
    }
    QVector<int> widths_extra_record = {50, 190, 200, 280, 200, 90};

    TableHelper::initTableHeader(list_extra_record, widths_extra_record, model_extra_record_result_, ui->tb_extra_record_result);

    ui->tb_extra_record_result->setSelectionMode(QAbstractItemView::NoSelection);
    ui->tb_extra_record_result->horizontalHeader()->setFixedHeight(40);
    ui->tb_extra_record_result->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
    ui->tb_extra_record_result->horizontalHeader()->setSectionResizeMode(5, QHeaderView::Fixed);
    ui->tb_extra_record_result->horizontalHeader()->resizeSection(5, 90);
    ui->tb_extra_record_result->verticalHeader()->setVisible(false);
    ui->tb_extra_record_result->setItemDelegateForColumn(0, new CheckboxDelegate(this));
    ui->tb_extra_record_result->setStyleSheet("");

    if (!extra_header_checkbox_) {
        qDebug() << "[DEBUG] Extra header checkbox created (first init)";
        extra_header_checkbox_ = new QCheckBox(ui->tb_extra_record_result->horizontalHeader());
        extra_header_checkbox_->setGeometry(15, 11, 25, 25);
        extra_header_checkbox_->setStyleSheet(
            "QCheckBox::indicator { "
            "   width: 18px; "
            "   height: 18px; "
            "   border: 2px solid #999999; "
            "   background-color: #FFFFFF; "
            "   border-radius: 3px; "
            "} "
            "QCheckBox::indicator:checked { "
            "   background-color: #005FA7; "
            "   border: 2px solid #005FA7; "
            "} "
            "QCheckBox::indicator:hover { "
            "   border: 2px solid #005FA7; "
            "}"
        );
        connect(extra_header_checkbox_, &QCheckBox::clicked, this, &BackendWnd::on_extra_header_checkbox_clicked);
    }
    extra_header_checkbox_->show();
    extra_header_checkbox_->raise();

    qDebug() << "[DEBUG] Extra table initialized with columns:" << list_extra_record;
}


void BackendWnd::initSelectPage()
{
    // connect
    QList<QPushButton*> btnList;
    btnList << ui->btn_page_record << ui->btn_page_advance;

    // ***index从1开始 0为select page***
    for (int i = 0; i < btnList.size(); i++)
        btnGpBackendSel->addButton(btnList.at(i), i+1);

    // Qt 6 只支持 idClicked, Qt 5.15+ 也支持 idClicked
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
    connect(btnGpBackendSel, &QButtonGroup::idClicked, this, &BackendWnd::event_backend_select_btn_clicked);
#else
    // Qt 5.15 以下版本使用 buttonClicked(int)
    connect(btnGpBackendSel, static_cast<void(QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked), 
            this, &BackendWnd::event_backend_select_btn_clicked);
#endif
}

void BackendWnd::init_record_page()
{
    btnGpRecordSel = new QButtonGroup(this);
    btnGpRecordSel->addButton(ui->btn_navi_sim, 0);
    btnGpRecordSel->addButton(ui->btn_navi_iot, 1);
    btnGpRecordSel->addButton(ui->btn_navi_serial, 2);
    btnGpRecordSel->addButton(ui->btn_navi_extra, 3);

    // Qt 6 只支持 idClicked, Qt 5.15+ 也支持 idClicked
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
    connect(btnGpRecordSel, &QButtonGroup::idClicked, this, &BackendWnd::event_navi_btn_clicked);
#else
    // Qt 5.15 以下版本使用 buttonClicked(int)
    connect(btnGpRecordSel, static_cast<void(QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked), 
            this, &BackendWnd::event_navi_btn_clicked);
#endif
    
    qDebug() << "[DEBUG] Qt version:" << QT_VERSION_STR << ", using" 
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
             << "idClicked signal";
#else
             << "buttonClicked(int) signal";
#endif

    // Connect Back Button to request back to home
    connect(ui->btn_record_back, &QPushButton::clicked, [this](){
         emit requestBackToHome();
    });
    
    // 连接SIM表格单击事件，支持点击"查看报告"列
    connect(ui->tb_sim_record_result, &QTableView::clicked, this, [this](const QModelIndex& index) {
        // 第7列是"查看报告"（索引从0开始，第0列是复选框）
        if (index.isValid() && index.column() == 7) {
            on_tb_sim_record_result_doubleClicked(index);
        }
    });
    
    // 连接IOT表格单击事件，支持点击"查看报告"列
    connect(ui->tb_iot_record_result, &QTableView::clicked, this, [this](const QModelIndex& index) {
        // 第5列是"查看报告"（索引从0开始，第0列是复选框）
        if (index.isValid() && index.column() == 5) {
            on_tb_iot_record_result_doubleClicked(index);
        }
    });

    // 连接串口测试表格单击事件，支持点击"查看报告"列
    connect(ui->tb_serial_record_result, &QTableView::clicked, this, [this](const QModelIndex& index) {
        if (index.isValid() && index.column() == 5) {
            on_tb_serial_record_result_doubleClicked(index);
        }
    });

    // 连接新增测试表格单击事件，支持点击"查看报告"列
    connect(ui->tb_extra_record_result, &QTableView::clicked, this, [this](const QModelIndex& index) {
        if (index.isValid() && index.column() == 5) {
            on_tb_extra_record_result_doubleClicked(index);
        }
    });
    
    // Default
    ui->btn_navi_sim->click();

    init_sim_page_combo_normal();
    init_sim_page_combo_completer();

    init_iot_page_combo_normal();
    init_iot_page_combo_completer();

    init_sim_page_time_select();
    init_iot_page_time_select();
    init_serial_page_time_select();
    init_serial_page_combo_normal();
    init_extra_page_time_select();
    init_extra_page_combo_normal();
}


void BackendWnd::init_sim_page_time_select()
{
    // 默认查询今天的记录（从今天00:00到明天00:00）
    QDateTime today = QDateTime::currentDateTime();
    today.setTime(QTime(0, 0, 0));
    ui->edt_sim_record_begin_date->setDateTime(today);
    ui->edt_sim_record_end_date->setDateTime(today.addDays(1));
}

void BackendWnd::init_iot_page_time_select()
{
    // 默认查询今天到明天的记录
    QDateTime today = QDateTime::currentDateTime();
    today.setTime(QTime(0, 0, 0));
    ui->edt_iot_record_begin_date->setDateTime(today);
    ui->edt_iot_record_end_date->setDateTime(today.addDays(1));
}


void BackendWnd::init_sim_page_combo_normal()
{
    // Net Status - 网络连接状态（成功/失败）
    ui->combo_sim_record_net_status->clear();
    ui->combo_sim_record_net_status->addItem(tr("All"), -1);
    ui->combo_sim_record_net_status->addItem(tr("Connection Successful"), 1); // 对应ESimNetStatus_Success
    ui->combo_sim_record_net_status->addItem(tr("Connection Failed"), 0);      // 对应ESimNetStatus_Fail

    // Net Type - 网络类型（使用通配符匹配数据库中的各种格式）
    ui->combo_sim_record_net_type->clear();
    ui->combo_sim_record_net_type->addItem(tr("All"), "");
    ui->combo_sim_record_net_type->addItem(tr("GSM (2G)"), "GSM");     // 匹配 "GSM - xxx"
    ui->combo_sim_record_net_type->addItem(tr("UMTS (3G)"), "UMTS");   // 匹配 "UMTS - xxx"
    ui->combo_sim_record_net_type->addItem(tr("LTE (4G)"), "LTE");     // 匹配 "LTE - xxx"
    ui->combo_sim_record_net_type->addItem(tr("CAT1"), "CAT1");
    ui->combo_sim_record_net_type->addItem(tr("CAT4"), "CAT4");
    ui->combo_sim_record_net_type->addItem(tr("CAT-M1"), "CAT-M1");
    ui->combo_sim_record_net_type->addItem(tr("CAT-NB1"), "CAT-NB1");
    ui->combo_sim_record_net_type->addItem(tr("NBIoT"), "NBIoT");      // BG95 NB-IoT
    ui->combo_sim_record_net_type->addItem(tr("eMTC"), "eMTC");
    ui->combo_sim_record_net_type->addItem(tr("无服务"), "无服务");
}

void BackendWnd::init_sim_page_combo_completer()
{
    // 已禁用候选项弹窗功能
}

void BackendWnd::init_iot_page_combo_normal()
{
}

void BackendWnd::init_iot_page_combo_completer()
{
    // 已禁用候选项弹窗功能
}

void BackendWnd::init_advance_page()
{
    ui->cb_serialport->clear();
    ui->cb_baudrate->clear();

    ui->cb_serialport->addItem(tr(CB_DEFAULT_ITEM), "");
    ui->cb_baudrate->addItem(tr(CB_DEFAULT_ITEM), "");


    for (int i = 0; i < 30; i++)
    {
        QString port_name = QString("COM%1").arg(i+1);

        ui->cb_serialport->addItem(port_name, port_name);
    }

    QStringList baudrate;
    baudrate << "4800" << "9600" << "19200" << "38400" << "43000" << "56000" << "57600" << "115200";

    for (int i = 0; i < baudrate.size(); i++)
    {
        QString baud_rate = baudrate.at(i);

        ui->cb_baudrate->addItem(baud_rate, baud_rate);
    }
}

void BackendWnd::update_combo_completer()
{
    update_sim_page_combo_completer();

    update_iot_page_combo_completer();
}

void BackendWnd::update_sim_page_combo_completer()
{
    QStringList module_id_list, imei_list, iccid_list, version_list;

    auto ret = TestRecordManager::getInstance()->GetComboCompleterInfo(module_id_list, imei_list, iccid_list, version_list, ETestType_Sim);
    if (ret != zl::EResult_Success)
    {
        qDebug() << "[INFO]" << "get combo completer info failed: " << ret;
        return;
    }

    // sim page completer - using QStringListModel instead of combo model
    QStringListModel* sim_iccid_model = new QStringListModel(iccid_list, this);
    completer_sim_page_sim_iccid->setModel(sim_iccid_model);




    // BOM version completer removed from sim page in new design
}

void BackendWnd::update_iot_page_combo_completer()
{
    QStringList module_id_list, imei_list, iccid_list, version_list;

    auto ret = TestRecordManager::getInstance()->GetComboCompleterInfo(module_id_list, imei_list, iccid_list, version_list, ETestType_Iot);
    if (ret != zl::EResult_Success)
    {
        qDebug() << "[INFO]" << "get combo completer info failed: " << ret;
        return;
    }

    // IOT module ID completer removed from IOT page in new design



    // IOT firmware version completer - using QStringListModel instead of combo model
    QStringListModel* iot_version_model = new QStringListModel(version_list, this);
    completer_iot_page_firmware_version->setModel(iot_version_model);
}

void BackendWnd::update_advance_info()
{
    auto serial_config = APPMODEL()->Config().serial_config;

    if (serial_config.port.isEmpty())
    {
        ui->cb_serialport->setCurrentIndex(0);
    }
    else
    {
        int32_t index = ui->cb_serialport->findText(serial_config.port);
        ui->cb_serialport->setCurrentIndex(index==-1?0:index);
    }

    if (serial_config.baud_rate.isEmpty())
    {
        ui->cb_baudrate->setCurrentIndex(0);
    }
    else
    {
        int32_t index = ui->cb_baudrate->findText(serial_config.baud_rate);
        ui->cb_baudrate->setCurrentIndex(index==-1?0:index);
    }
}

void BackendWnd::event_backend_select_btn_clicked(int index)
{
    if (index <= 0) return;

    emit GlobalSignal::getInstance()->backend_page_change(zl::EBP(index));
}

void BackendWnd::event_navi_btn_clicked(int index)
{
    qDebug() << "[DEBUG] Switching to page" << index << (index == 0 ? "(SIM)" : "(IOT)");
    
    ui->stackedWidget->setCurrentIndex(index);
    
    // 输出当前页面信息
    if (index == 0) {
        qDebug() << "[DEBUG] SIM table column count:" << model_sim_record_result_->columnCount();
        qDebug() << "[DEBUG] SIM table row count:" << model_sim_record_result_->rowCount();
    } else if (index == 1) {
        qDebug() << "[DEBUG] IOT table column count:" << model_iot_record_result_->columnCount();
        qDebug() << "[DEBUG] IOT table row count:" << model_iot_record_result_->rowCount();
    } else if (index == 2) {
        qDebug() << "[DEBUG] Serial table column count:" << model_serial_record_result_->columnCount();
        qDebug() << "[DEBUG] Serial table row count:" << model_serial_record_result_->rowCount();
    } else if (index == 3) {
        qDebug() << "[DEBUG] Extra table column count:" << model_extra_record_result_->columnCount();
        qDebug() << "[DEBUG] Extra table row count:" << model_extra_record_result_->rowCount();
    }
}


void BackendWnd::on_btn_sim_record_query_clicked()
{
    QDate beginDate = ui->edt_sim_record_begin_date->date();
    QDate endDateExclusive = ui->edt_sim_record_end_date->date().addDays(1);
    QString begdt = beginDate.toString("yyyy-MM-dd") + " 00:00:00";
    QString enddt = endDateExclusive.toString("yyyy-MM-dd") + " 00:00:00";

    ETestType test_type = ETestType_Sim;
    int32_t network_state = ui->combo_sim_record_net_status->currentData().toInt();
    
    // 获取网络类型筛选条件（字符串）
    QString network_type = ui->combo_sim_record_net_type->currentData().toString();

    QString sim_iccid = ui->edt_sim_record_iccid->text();
    QString bom_version = ""; 

    sim_record_vec_.clear();

    int32_t ret = zl::TestRecordManager::getInstance()->GetAllRecord(
        sim_record_vec_,
        begdt,
        enddt,
        test_type,
        zl::EResultType_Unknow,
        ESimNetStatus(network_state),
        "", 
        "", 
        sim_iccid,
        bom_version,
        network_type);

    if (ret != zl::EResult_Success)
    {
        MsgWnd::ShowNormalInfo(QObject::tr("Query Sim Record Failed"));
        return;
    }

    auto pfconvert3 = [this](int32_t v) -> QString {
        switch (v)
        {
        case zl::ESimNetStatus_Fail:           return tr("Connection Failed");
        case zl::ESimNetStatus_Success:        return tr("Connection Successful");
        default: return tr("Not Connected");
        }
    };

    auto funcAddItem = [&](const zl::RecordInfo& record)
    {
        QList<QStandardItem*> row;

        // 0. 复选框
        QStandardItem* item_checkbox = new QStandardItem();
        item_checkbox->setCheckable(true);
        item_checkbox->setCheckState(Qt::Unchecked);
        item_checkbox->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        item_checkbox->setEditable(false); // 只允许复选框交互，不允许文本编辑
        row.append(item_checkbox);

        // 1. 流水号（时间戳格式：年月日时分秒毫秒）
        QStandardItem* item_seq = new QStandardItem(record.record_id);
        item_seq->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        item_seq->setEditable(false);
        row.append(item_seq);

        // 2. Sim ICCID
        QStandardItem* item_iccid = new QStandardItem(record.iccid == "测试失败" ? tr("测试失败") : record.iccid);
        item_iccid->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        item_iccid->setEditable(false);
        if (record.iccid == "测试失败") item_iccid->setForeground(Qt::red);
        row.append(item_iccid);

        // 3. Net Status
        QStandardItem* item_net_status = new QStandardItem(pfconvert3(record.net_status));
        item_net_status->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        if (record.net_status == zl::ESimNetStatus_Fail) item_net_status->setForeground(Qt::red);
        row.append(item_net_status);

        // 4. Network Type
        QStandardItem* item_net_type = new QStandardItem(record.network_type.isEmpty() ? "-" : (record.network_type == "测试失败" ? tr("测试失败") : record.network_type));
        item_net_type->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        if (record.network_type == "测试失败") item_net_type->setForeground(Qt::red);
        row.append(item_net_type);

        // 5. Signal Strength
        {
            bool isCn = (APPMODEL()->CabinetLanguage() == zl::ELanguageType_Cn);
            auto translateSignal = [&isCn](const QString& s) -> QString {
                if (s == "Test Failed" || s == "测试失败")
                    return isCn ? "测试失败" : "Test Failed";
                if (s == "Unknown/Not detectable" || s == "未知/不可测")
                    return isCn ? "未知/不可测" : "Unknown/Not detectable";
                return s;
            };
            QString signalDisplay = record.signal_strength.isEmpty() ? "-" : translateSignal(record.signal_strength);
            QStandardItem* item_signal = new QStandardItem(signalDisplay);
            item_signal->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            bool signalFailed = (record.signal_strength == "Test Failed" || record.signal_strength == "测试失败");
            if (signalFailed) item_signal->setForeground(Qt::red);
            row.append(item_signal);
        }

        // 6. Time
        QStandardItem* item_time = new QStandardItem(record.test_time);
        item_time->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        row.append(item_time);
        
        // 7. Operation
        QStandardItem* item_op = new QStandardItem(tr("View Report"));
        item_op->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        item_op->setBackground(QBrush(QColor(0, 95, 167)));
        item_op->setForeground(Qt::white);
        row.append(item_op);

        model_sim_record_result_->appendRow(row);
    };

    // update table order
    TableHelper::readyUpdate(model_sim_record_result_, ui->tb_sim_record_result);

    foreach(const zl::RecordInfo record, sim_record_vec_)
    {
        funcAddItem(record);
    }

    TableHelper::finishUpdate(model_sim_record_result_, ui->tb_sim_record_result);

}

void BackendWnd::on_btn_sim_record_export_excel_clicked()
{
    int rowCount = model_sim_record_result_ ? model_sim_record_result_->rowCount() : 0;
    if (rowCount < 1)
    {
        MsgWnd::ShowNormalInfo(QObject::tr("Please query data first"));
        return;
    }

    QList<int> selectedRows;
    for (int i = 0; i < rowCount; ++i)
    {
        QStandardItem* cbItem = model_sim_record_result_->item(i, 0);
        if (cbItem && cbItem->checkState() == Qt::Checked)
        {
            selectedRows.append(i);
        }
    }

    if (selectedRows.isEmpty())
    {
        MsgWnd::ShowNormalInfo(QObject::tr("Please select records to export"));
        return;
    }

    QString timestamp = QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss");
    QString defaultFileName = QString("sim_test_report_%1.csv").arg(timestamp);
    QString fileName = QFileDialog::getSaveFileName(
        this,
        tr("Export SIM Card Test Report"),
        QDir::homePath() + "/" + defaultFileName,
        tr("CSV Files (*.csv);;All Files (*)")
    );

    if (fileName.isEmpty())
    {
        return;
    }

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QMessageBox::warning(this, tr("Export Failed"), tr("Cannot create file: %1").arg(fileName));
        return;
    }

    QTextStream out(&file);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    out.setEncoding(QStringConverter::Utf8);
#else
    out.setCodec("UTF-8");
#endif

    // UTF-8 BOM for Excel/WPS compatibility with Chinese text.
    out << "\xEF\xBB\xBF";

    auto csvEscape = [](const QString& text) -> QString {
        QString escaped = text;
        escaped.replace("\"", "\"\"");
        return QString("\"%1\"").arg(escaped);
    };

    // Export exactly the highlighted header columns: Serial Number ~ Test Time.
    QStringList headers;
    for (int col = 1; col <= 6; ++col)
    {
        headers.append(csvEscape(model_sim_record_result_->headerData(col, Qt::Horizontal).toString()));
    }
    out << headers.join(",") << "\n";

    for (int row : selectedRows)
    {
        QStringList rowData;
        for (int col = 1; col <= 6; ++col)
        {
            rowData.append(csvEscape(model_sim_record_result_->index(row, col).data().toString()));
        }
        out << rowData.join(",") << "\n";
    }

    file.close();
    QMessageBox::information(this, tr("Export Success"), tr("Report exported successfully to: %1").arg(fileName));
}

void BackendWnd::on_btn_sim_record_reset_clicked()
{
    ui->edt_sim_record_iccid->clear();
    ui->combo_sim_record_net_status->setCurrentIndex(0);
    ui->combo_sim_record_net_type->setCurrentIndex(0);
    init_sim_page_time_select();
}

void BackendWnd::on_btn_sim_record_delete_clicked()
{
    QStringList record_list;
    int rowCount = model_sim_record_result_->rowCount();
    for (int i = 0; i < rowCount; ++i) {
        QStandardItem* cbItem = model_sim_record_result_->item(i, 0);
        if (cbItem && cbItem->checkState() == Qt::Checked) {
            QStandardItem* seqItem = model_sim_record_result_->item(i, 1);
            if (seqItem) record_list << seqItem->text();
        }
    }

    if (record_list.isEmpty()) return;

    pending_delete_records_ = record_list;
    pending_delete_type_ = "sim";
    MsgWnd::ShowDeleteConfirm(this);
}

void BackendWnd::on_btn_iot_record_query_clicked()
{
    QDate beginDate = ui->edt_iot_record_begin_date->date();
    QDate endDateExclusive = ui->edt_iot_record_end_date->date().addDays(1);
    QString begdt = beginDate.toString("yyyy-MM-dd") + " 00:00:00";
    QString enddt = endDateExclusive.toString("yyyy-MM-dd") + " 00:00:00";

    ETestType test_type = ETestType_Iot;
    
    // Filters
    QString iot_imei = ui->edt_iot_record_imei->text();
    QString bom_version = ui->edt_iot_record_version->text();

    iot_record_vec_.clear();

    int32_t ret = zl::TestRecordManager::getInstance()->GetAllRecord(
        iot_record_vec_,
        begdt,
        enddt,
        test_type,
        zl::EResultType_Unknow,
        zl::ESimNetStatus_Unknow,
        "", // iot_module_id
        iot_imei,
        "", // sim_iccid
        bom_version,
        ""); // network_type

    if (ret != zl::EResult_Success)
    {
        MsgWnd::ShowNormalInfo(QObject::tr("Query IOT Record Failed"));
        return;
    }

    auto funcAddItem = [&](const zl::RecordInfo& record)
    {
        QList<QStandardItem*> row;

        // 0. 复选框
        QStandardItem* item_checkbox = new QStandardItem();
        item_checkbox->setCheckable(true);
        item_checkbox->setCheckState(Qt::Unchecked);
        item_checkbox->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        item_checkbox->setEditable(false); // 只允许复选框交互，不允许文本编辑
        row.append(item_checkbox);

        // 1. 流水号（时间戳格式：年月日时分秒毫秒）
        QStandardItem* item_seq = new QStandardItem(record.record_id);
        item_seq->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        item_seq->setEditable(false);
        row.append(item_seq);

        // 2. IOT Version (应该显示IOT模块版本，而不是镜像版本)
        QStandardItem* item_version = new QStandardItem(record.iot_module_id.isEmpty() ? "-" : (record.iot_module_id == "测试失败" ? tr("测试失败") : record.iot_module_id));
        item_version->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        item_version->setEditable(false);
        if (record.iot_module_id == "测试失败") item_version->setForeground(Qt::red);
        row.append(item_version);

        // 3. IOT IMEI
        QStandardItem* item_imei = new QStandardItem(record.iot_imei.isEmpty() ? "-" : (record.iot_imei == "测试失败" ? tr("测试失败") : record.iot_imei));
        item_imei->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        item_imei->setEditable(false);
        item_imei->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        if (record.iot_imei == "测试失败") item_imei->setForeground(Qt::red);
        row.append(item_imei);

        // 4. Test Time
        QStandardItem* item_time = new QStandardItem(record.test_time);
        item_time->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        row.append(item_time);

        // 5. Operation
        QStandardItem* item_op = new QStandardItem(tr("View Report"));
        item_op->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        item_op->setBackground(QBrush(QColor(0, 95, 167)));
        item_op->setForeground(Qt::white);
        row.append(item_op);

        model_iot_record_result_->appendRow(row);
    };

    // update table order
    TableHelper::readyUpdate(model_iot_record_result_, ui->tb_iot_record_result);

    foreach(const zl::RecordInfo record, iot_record_vec_)
    {
        funcAddItem(record);
    }

    TableHelper::finishUpdate(model_iot_record_result_, ui->tb_iot_record_result);
}

void BackendWnd::on_btn_iot_record_export_excel_clicked()
{
    int rowCount = model_iot_record_result_ ? model_iot_record_result_->rowCount() : 0;
    if (rowCount < 1)
    {
        MsgWnd::ShowNormalInfo(QObject::tr("Please query data first"));
        return;
    }

    QList<int> selectedRows;
    for (int i = 0; i < rowCount; ++i)
    {
        QStandardItem* cbItem = model_iot_record_result_->item(i, 0);
        if (cbItem && cbItem->checkState() == Qt::Checked)
        {
            selectedRows.append(i);
        }
    }

    if (selectedRows.isEmpty())
    {
        MsgWnd::ShowNormalInfo(QObject::tr("Please select records to export"));
        return;
    }

    QString timestamp = QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss");
    QString defaultFileName = QString("iot_test_report_%1.csv").arg(timestamp);
    QString fileName = QFileDialog::getSaveFileName(
        this,
        tr("Export IOT Test Report"),
        QDir::homePath() + "/" + defaultFileName,
        tr("CSV Files (*.csv);;All Files (*)")
    );

    if (fileName.isEmpty())
    {
        return;
    }

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QMessageBox::warning(this, tr("Export Failed"), tr("Cannot create file: %1").arg(fileName));
        return;
    }

    QTextStream out(&file);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    out.setEncoding(QStringConverter::Utf8);
#else
    out.setCodec("UTF-8");
#endif

    // UTF-8 BOM for Excel/WPS compatibility with Chinese text.
    out << "\xEF\xBB\xBF";

    auto csvEscape = [](const QString& text) -> QString {
        QString escaped = text;
        escaped.replace("\"", "\"\"");
        return QString("\"%1\"").arg(escaped);
    };

    // Export exactly the highlighted header columns: Serial Number ~ Test Time.
    QStringList headers;
    for (int col = 1; col <= 4; ++col)
    {
        headers.append(csvEscape(model_iot_record_result_->headerData(col, Qt::Horizontal).toString()));
    }
    out << headers.join(",") << "\n";

    for (int row : selectedRows)
    {
        QStringList rowData;
        for (int col = 1; col <= 4; ++col)
        {
            rowData.append(csvEscape(model_iot_record_result_->index(row, col).data().toString()));
        }
        out << rowData.join(",") << "\n";
    }

    file.close();
    QMessageBox::information(this, tr("Export Success"), tr("Report exported successfully to: %1").arg(fileName));
}

void BackendWnd::on_btn_iot_record_reset_clicked()
{
    ui->edt_iot_record_version->clear();
    ui->edt_iot_record_imei->clear();
    init_iot_page_time_select();
}

void BackendWnd::on_btn_iot_record_delete_clicked()
{
    QStringList record_list;
    int rowCount = model_iot_record_result_->rowCount();
    for (int i = 0; i < rowCount; ++i) {
        QStandardItem* cbItem = model_iot_record_result_->item(i, 0);
        if (cbItem && cbItem->checkState() == Qt::Checked) {
            QStandardItem* seqItem = model_iot_record_result_->item(i, 1);
            if (seqItem) record_list << seqItem->text();
        }
    }

    if (record_list.isEmpty()) return;

    pending_delete_records_ = record_list;
    pending_delete_type_ = "iot";
    MsgWnd::ShowDeleteConfirm(this);
}

void BackendWnd::event_user_confirm(const QString& token)
{
    if (token != "DELETE_SELECTED_RECORDS") return;

    if (pending_delete_records_.isEmpty()) return;

    auto ret = TestRecordManager::getInstance()->RemoveTestRecords(pending_delete_records_);
    pending_delete_records_.clear();

    if (ret != zl::EResult_Success)
    {
        qDebug() << "[ERROR]" << "delete selected records failed, ret:" << ret;
        return;
    }

    QEventLoop loop1;
    QTimer::singleShot(200, &loop1, SLOT(quit()));
    loop1.exec();

    if (pending_delete_type_ == "sim")
        on_btn_sim_record_query_clicked();
    else if (pending_delete_type_ == "iot")
        on_btn_iot_record_query_clicked();
    else if (pending_delete_type_ == "extra")
        on_btn_extra_record_query_clicked();
    else
        on_btn_serial_record_query_clicked();

    pending_delete_type_.clear();
}

void BackendWnd::on_btn_lang_cn_clicked()
{
    zl::ELanguageType cur_lang = APPMODEL()->CabinetLanguage();

    if (cur_lang == zl::ELanguageType_Cn) return;

    emit GlobalSignal::getInstance()->select_language(zl::ELanguageType_Cn);
}

void BackendWnd::on_btn_lang_en_clicked()
{
    zl::ELanguageType cur_lang = APPMODEL()->CabinetLanguage();

    if (cur_lang == zl::ELanguageType_En) return;

    emit GlobalSignal::getInstance()->select_language(zl::ELanguageType_En);
}

void BackendWnd::showTestReportDialog(const QString& recordId, const QString& testLog)
{
    bool isCn = (APPMODEL()->CabinetLanguage() == zl::ELanguageType_Cn);

    // 创建对话框
    QDialog* dialog = new QDialog(this);
    dialog->setWindowTitle((isCn ? "测试报告" : "Test Report") + QString(" - ") + recordId);
    dialog->resize(800, 600);
    dialog->setModal(true);

    // 创建布局
    QVBoxLayout* mainLayout = new QVBoxLayout(dialog);

    // 添加标题标签
    QLabel* titleLabel = new QLabel((isCn ? "测试记录ID: " : "Test Record ID: ") + recordId, dialog);
    QFont titleFont = titleLabel->font();
    titleFont.setBold(true);
    titleFont.setPointSize(titleFont.pointSize() + 2);
    titleLabel->setFont(titleFont);
    mainLayout->addWidget(titleLabel);

    // 添加测试日志文本框
    QPlainTextEdit* logTextEdit = new QPlainTextEdit(dialog);
    logTextEdit->setPlainText(testLog);
    logTextEdit->setReadOnly(true);
    logTextEdit->setLineWrapMode(QPlainTextEdit::NoWrap);
    
    // 设置等宽字体以便更好地显示日志
    QFont logFont("Courier New");
    logFont.setPointSize(9);
    logTextEdit->setFont(logFont);
    
    mainLayout->addWidget(logTextEdit);

    // 添加关闭和导出按钮
    QPushButton* closeButton = new QPushButton(isCn ? "关闭" : "Close", dialog);
    closeButton->setFixedWidth(120);
    connect(closeButton, &QPushButton::clicked, dialog, &QDialog::accept);

    QPushButton* exportButton = new QPushButton(isCn ? "导出报告" : "Export Report", dialog);
    exportButton->setFixedWidth(140);
    connect(exportButton, &QPushButton::clicked, dialog, [dialog, recordId, testLog, isCn]() {
        QString timestamp = QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss");
        QString defaultFileName = QString("test_report_%1_%2.txt").arg(recordId).arg(timestamp);
        QString fileName = QFileDialog::getSaveFileName(
            dialog,
            isCn ? "导出测试报告" : "Export Test Report",
            QDir::homePath() + "/" + defaultFileName,
            isCn ? "文本文件 (*.txt);;所有文件 (*)" : "Text Files (*.txt);;All Files (*)"
        );
        if (fileName.isEmpty()) return;

        QFile file(fileName);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QMessageBox::warning(dialog,
                                 isCn ? "导出失败" : "Export Failed",
                                 (isCn ? "无法创建文件: " : "Cannot create file: ") + fileName);
            return;
        }
        QTextStream out(&file);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        out.setEncoding(QStringConverter::Utf8);
#else
        out.setCodec("UTF-8");
#endif
        out << testLog;
        file.close();
        QMessageBox::information(dialog,
                                 isCn ? "导出成功" : "Export Success",
                                 (isCn ? "报告已成功导出到: " : "Report exported successfully to: ") + fileName);
    });

    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    buttonLayout->addWidget(closeButton);
    buttonLayout->addWidget(exportButton);
    buttonLayout->addStretch();
    mainLayout->addLayout(buttonLayout);

    dialog->setLayout(mainLayout);
    dialog->exec();
    dialog->deleteLater();
}

void BackendWnd::on_tb_sim_record_result_doubleClicked(const QModelIndex& index)
{
    if (!index.isValid()) {
        return;
    }

    // 获取点击行的record_id（第1列，第0列是复选框）
    QModelIndex recordIdIndex = model_sim_record_result_->index(index.row(), 1);
    QString recordId = model_sim_record_result_->data(recordIdIndex).toString();

    qDebug() << "[INFO] Double clicked SIM record, ID:" << recordId;

    // 每次点击按 record_id 直查数据库，拿最新 TEST_LOG
    QString testLog;
    QSqlQuery query = zl::CDBConnector::Instance()->CreateQuery();
    query.prepare("SELECT TEST_LOG FROM T_RECORD WHERE RECORD_ID=:record_id");
    query.bindValue(":record_id", recordId);
    int32_t ret = zl::CDBConnector::Instance()->ExecQuery(query);
    if (ret == zl::CDBConnector::EDBError_Success && query.next()) {
        testLog = query.value("TEST_LOG").toString();
        qDebug() << "[INFO] Loaded latest test log from DB, length:" << testLog.length();
    }

    // 回退：数据库异常时仍尝试使用当前页面缓存
    if (testLog.isEmpty()) {
        foreach(const zl::RecordInfo& record, sim_record_vec_) {
            if (record.record_id == recordId) {
                testLog = record.test_log;
                qDebug() << "[WARN] Fallback to cached test log, length:" << testLog.length();
                break;
            }
        }
    }

    if (testLog.isEmpty()) {
        qDebug() << "[WARN] No test log found for record:" << recordId;
        QMessageBox::warning(this, tr("Warning"), tr("No test log found for this record"));
        return;
    }

    showTestReportDialog(recordId, testLog);
}

void BackendWnd::on_tb_iot_record_result_doubleClicked(const QModelIndex& index)
{
    if (!index.isValid()) {
        return;
    }

    // 获取点击行的record_id（第1列，第0列是复选框）
    QModelIndex recordIdIndex = model_iot_record_result_->index(index.row(), 1);
    QString recordId = model_iot_record_result_->data(recordIdIndex).toString();

    qDebug() << "[INFO] Double clicked IOT record, ID:" << recordId;

    // 每次点击按 record_id 直查数据库，拿最新 TEST_LOG
    QString testLog;
    QSqlQuery query = zl::CDBConnector::Instance()->CreateQuery();
    query.prepare("SELECT TEST_LOG FROM T_RECORD WHERE RECORD_ID=:record_id");
    query.bindValue(":record_id", recordId);
    int32_t ret = zl::CDBConnector::Instance()->ExecQuery(query);
    if (ret == zl::CDBConnector::EDBError_Success && query.next()) {
        testLog = query.value("TEST_LOG").toString();
        qDebug() << "[INFO] Loaded latest test log from DB, length:" << testLog.length();
    }

    // 回退：数据库异常时仍尝试使用当前页面缓存
    if (testLog.isEmpty()) {
        foreach(const zl::RecordInfo& record, iot_record_vec_) {
            if (record.record_id == recordId) {
                testLog = record.test_log;
                qDebug() << "[WARN] Fallback to cached test log, length:" << testLog.length();
                break;
            }
        }
    }

    if (testLog.isEmpty()) {
        qDebug() << "[WARN] No test log found for record:" << recordId;
        QMessageBox::warning(this, tr("Warning"), tr("No test log found for this record"));
        return;
    }

    showTestReportDialog(recordId, testLog);
}

void BackendWnd::on_btn_save_advance_clicked()
{
    auto config = APPMODEL()->Config();
    bool modified = false;

    QString cur_port_name = ui->cb_serialport->currentData().toString().trimmed();
    QString cur_baud_rate = ui->cb_baudrate->currentData().toString().trimmed();

    if (config.serial_config.port.compare(cur_port_name) != 0 || config.serial_config.baud_rate.compare(cur_baud_rate) != 0)
    {
        modified = true;
    }

    config.serial_config.port = cur_port_name;
    config.serial_config.baud_rate = cur_baud_rate;

    APPMODEL()->SetConfig(config);

    auto ret = APPMODEL()->SaveAppConfiguration();
    if (ret != zl::EResult_Success)
    {
        MsgWnd::ShowNormalInfo(QObject::tr("Save config info failed"));
    }
    else
    {
        if (modified)
            emit configChanged();
    }
}

// SIM表头复选框点击事件 - 全选/取消全选
void BackendWnd::on_sim_header_checkbox_clicked(bool checked)
{
    if (!model_sim_record_result_) return;
    
    int row_count = model_sim_record_result_->rowCount();
    for (int i = 0; i < row_count; ++i) {
        QStandardItem* item = model_sim_record_result_->item(i, 0);
        if (item && item->isCheckable()) {
            item->setCheckState(checked ? Qt::Checked : Qt::Unchecked);
        }
    }
}

// IOT表头复选框点击事件 - 全选/取消全选
void BackendWnd::on_iot_header_checkbox_clicked(bool checked)
{
    if (!model_iot_record_result_) return;
    
    int row_count = model_iot_record_result_->rowCount();
    for (int i = 0; i < row_count; ++i) {
        QStandardItem* item = model_iot_record_result_->item(i, 0);
        if (item && item->isCheckable()) {
            item->setCheckState(checked ? Qt::Checked : Qt::Unchecked);
        }
    }
}

void BackendWnd::init_serial_page_time_select()
{
    // 默认查询今天到明天的记录
    QDateTime today = QDateTime::currentDateTime();
    today.setTime(QTime(0, 0, 0));
    ui->edt_serial_record_begin_date->setDateTime(today);
    ui->edt_serial_record_end_date->setDateTime(today.addDays(1));
}

void BackendWnd::init_serial_page_combo_normal()
{
    const bool isCn = (APPMODEL()->CabinetLanguage() == zl::ELanguageType_Cn);

    ui->btn_navi_serial->setText(isCn ? QStringLiteral("串口测试") : QStringLiteral("Serial Port Test"));
    ui->lb_serial_record_cmd->setText(isCn ? QStringLiteral("命令字:") : QStringLiteral("Command:"));
    ui->lb_serial_record_result->setText(isCn ? QStringLiteral("测试结果:") : QStringLiteral("Test Result:"));

    ui->combo_serial_record_cmd->clear();
    ui->combo_serial_record_cmd->addItem(isCn ? QStringLiteral("全部") : QStringLiteral("All"), -1);

    struct CmdItem {
        quint8 cmd;
        const char *label;
    };
    static const CmdItem items[] = {
        { Protocol::CmdQueryVersion,      "0x01 Query Board Version" },
        { Protocol::CmdVccCn52Test,       "0x02 VCC 12/5/3.3 (CN52)" },
        { Protocol::CmdPrinterCn43Test,   "0x03 Printer Power (CN43)" },
        { Protocol::CmdVout5vCn39Test,    "0x04 5V Ctl Output (CN39)" },
        { Protocol::CmdVout12vCn47Test,   "0x05 12V Ctl Output (CN47)" },
        { Protocol::CmdProximityCn13Test, "0x06 5V Proximity (CN13)" },
        { Protocol::CmdStInputIoTest,     "0x07 ST_INPUT1/2 IO Test" },
    };
    for (const CmdItem &item : items)
        ui->combo_serial_record_cmd->addItem(QString::fromLatin1(item.label), static_cast<int>(item.cmd));

    ui->combo_serial_record_result->clear();
    ui->combo_serial_record_result->addItem(isCn ? QStringLiteral("全部") : QStringLiteral("All"), -1);
    ui->combo_serial_record_result->addItem(isCn ? QStringLiteral("成功") : QStringLiteral("Success"), 1);
    ui->combo_serial_record_result->addItem(isCn ? QStringLiteral("失败") : QStringLiteral("Failure"), 0);
}

void BackendWnd::on_btn_serial_record_query_clicked()
{
    QDate beginDate = ui->edt_serial_record_begin_date->date();
    QDate endDateExclusive = ui->edt_serial_record_end_date->date().addDays(1);
    QString begdt = beginDate.toString("yyyy-MM-dd") + " 00:00:00";
    QString enddt = endDateExclusive.toString("yyyy-MM-dd") + " 00:00:00";

    ETestType test_type = ETestType_Serial;

    const int selectedCmd = ui->combo_serial_record_cmd->currentData().toInt();
    const int selectedResult = ui->combo_serial_record_result->currentData().toInt(); // -1全部, 1成功, 0失败

    serial_record_vec_.clear();

    int32_t ret = zl::TestRecordManager::getInstance()->GetAllRecord(
        serial_record_vec_,
        begdt,
        enddt,
        test_type,
        zl::EResultType_Unknow,
        zl::ESimNetStatus_Unknow,
        "",
        "",
        "",
        "",
        "");

    if (ret != zl::EResult_Success)
    {
        MsgWnd::ShowNormalInfo(QObject::tr("Query Serial Record Failed"));
        return;
    }

    if (selectedCmd >= 0 || selectedResult >= 0) {
        zl::RecordVec filtered;
        const QString cmdKey = selectedCmd >= 0
            ? QString("0x%1").arg(selectedCmd, 2, 16, QChar('0')).toUpper()
            : QString();
        for (const zl::RecordInfo &record : serial_record_vec_) {
            if (selectedCmd >= 0) {
                const QString cmdInfo = record.cmd_ret_info.trimmed().toUpper();
                if (!(cmdInfo == cmdKey || cmdInfo.startsWith(cmdKey + " ") || cmdInfo.startsWith(cmdKey + "\t")))
                    continue;
            }
            if (selectedResult == 1) {
                if (record.result_type != zl::EResultType_Success)
                    continue;
            } else if (selectedResult == 0) {
                if (record.result_type == zl::EResultType_Success)
                    continue;
            }
            filtered.push_back(record);
        }
        serial_record_vec_ = filtered;
    }

    const bool isCn = (APPMODEL()->CabinetLanguage() == zl::ELanguageType_Cn);

    auto funcAddItem = [&](const zl::RecordInfo& record)
    {
        QList<QStandardItem*> row;

        QStandardItem* item_checkbox = new QStandardItem();
        item_checkbox->setCheckable(true);
        item_checkbox->setCheckState(Qt::Unchecked);
        item_checkbox->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        item_checkbox->setEditable(false);
        row.append(item_checkbox);

        QStandardItem* item_seq = new QStandardItem(record.record_id);
        item_seq->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        item_seq->setEditable(false);
        row.append(item_seq);

        QStandardItem* item_cmd = new QStandardItem(record.cmd_ret_info.isEmpty() ? "-" : record.cmd_ret_info);
        item_cmd->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        item_cmd->setEditable(false);
        row.append(item_cmd);

        const bool ok = (record.result_type == zl::EResultType_Success);
        QString resultText = ok
            ? (isCn ? QStringLiteral("成功") : QStringLiteral("Success"))
            : (isCn ? QStringLiteral("测试失败") : QStringLiteral("Failed"));
        // 兼容旧记录：result_info 可能是 OK / Success / 测试失败
        if (ok && (record.result_info == QStringLiteral("OK")
                   || record.result_info.contains(QStringLiteral("(OK)"))))
            resultText = isCn ? QStringLiteral("成功") : QStringLiteral("Success");

        QStandardItem* item_result = new QStandardItem(resultText);
        item_result->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        item_result->setEditable(false);
        if (!ok)
            item_result->setForeground(Qt::red);
        row.append(item_result);

        QStandardItem* item_time = new QStandardItem(record.test_time);
        item_time->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        row.append(item_time);

        QStandardItem* item_op = new QStandardItem(isCn ? QStringLiteral("查看报告") : tr("View Report"));
        item_op->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        item_op->setBackground(QBrush(QColor(0, 95, 167)));
        item_op->setForeground(Qt::white);
        row.append(item_op);

        model_serial_record_result_->appendRow(row);
    };

    TableHelper::readyUpdate(model_serial_record_result_, ui->tb_serial_record_result);

    foreach(const zl::RecordInfo record, serial_record_vec_)
    {
        funcAddItem(record);
    }

    TableHelper::finishUpdate(model_serial_record_result_, ui->tb_serial_record_result);
}

void BackendWnd::on_btn_serial_record_export_excel_clicked()
{
    int rowCount = model_serial_record_result_ ? model_serial_record_result_->rowCount() : 0;
    if (rowCount < 1)
    {
        MsgWnd::ShowNormalInfo(QObject::tr("Please query data first"));
        return;
    }

    QList<int> selectedRows;
    for (int i = 0; i < rowCount; ++i)
    {
        QStandardItem* cbItem = model_serial_record_result_->item(i, 0);
        if (cbItem && cbItem->checkState() == Qt::Checked)
        {
            selectedRows.append(i);
        }
    }

    if (selectedRows.isEmpty())
    {
        MsgWnd::ShowNormalInfo(QObject::tr("Please select records to export"));
        return;
    }

    QString timestamp = QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss");
    QString defaultFileName = QString("serial_test_report_%1.csv").arg(timestamp);
    QString fileName = QFileDialog::getSaveFileName(
        this,
        tr("Export Serial Port Test Report"),
        QDir::homePath() + "/" + defaultFileName,
        tr("CSV Files (*.csv);;All Files (*)")
    );

    if (fileName.isEmpty())
    {
        return;
    }

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QMessageBox::warning(this, tr("Export Failed"), tr("Cannot create file: %1").arg(fileName));
        return;
    }

    QTextStream out(&file);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    out.setEncoding(QStringConverter::Utf8);
#else
    out.setCodec("UTF-8");
#endif

    // UTF-8 BOM for Excel/WPS compatibility with Chinese text.
    out << "\xEF\xBB\xBF";

    auto csvEscape = [](const QString& text) -> QString {
        QString escaped = text;
        escaped.replace("\"", "\"\"");
        return QString("\"%1\"").arg(escaped);
    };

    // Export exactly the highlighted header columns: Serial Number ~ Test Time.
    QStringList headers;
    for (int col = 1; col <= 4; ++col)
    {
        headers.append(csvEscape(model_serial_record_result_->headerData(col, Qt::Horizontal).toString()));
    }
    out << headers.join(",") << "\n";

    for (int row : selectedRows)
    {
        QStringList rowData;
        for (int col = 1; col <= 4; ++col)
        {
            rowData.append(csvEscape(model_serial_record_result_->index(row, col).data().toString()));
        }
        out << rowData.join(",") << "\n";
    }

    file.close();
    QMessageBox::information(this, tr("Export Success"), tr("Report exported successfully to: %1").arg(fileName));
}

void BackendWnd::on_btn_serial_record_reset_clicked()
{
    ui->combo_serial_record_cmd->setCurrentIndex(0);
    ui->combo_serial_record_result->setCurrentIndex(0);
    init_serial_page_time_select();
}

void BackendWnd::on_btn_serial_record_delete_clicked()
{
    QStringList record_list;
    int rowCount = model_serial_record_result_->rowCount();
    for (int i = 0; i < rowCount; ++i) {
        QStandardItem* cbItem = model_serial_record_result_->item(i, 0);
        if (cbItem && cbItem->checkState() == Qt::Checked) {
            QStandardItem* seqItem = model_serial_record_result_->item(i, 1);
            if (seqItem) record_list << seqItem->text();
        }
    }

    if (record_list.isEmpty()) return;

    pending_delete_records_ = record_list;
    pending_delete_type_ = "serial";
    MsgWnd::ShowDeleteConfirm(this);
}

void BackendWnd::on_tb_serial_record_result_doubleClicked(const QModelIndex& index)
{
    if (!index.isValid()) {
        return;
    }

    // 获取点击行的record_id（第1列，第0列是复选框）
    QModelIndex recordIdIndex = model_serial_record_result_->index(index.row(), 1);
    QString recordId = model_serial_record_result_->data(recordIdIndex).toString();

    qDebug() << "[INFO] Double clicked Serial record, ID:" << recordId;

    // 每次点击按 record_id 直查数据库，拿最新 TEST_LOG
    QString testLog;
    QSqlQuery query = zl::CDBConnector::Instance()->CreateQuery();
    query.prepare("SELECT TEST_LOG FROM T_RECORD WHERE RECORD_ID=:record_id");
    query.bindValue(":record_id", recordId);
    int32_t ret = zl::CDBConnector::Instance()->ExecQuery(query);
    if (ret == zl::CDBConnector::EDBError_Success && query.next()) {
        testLog = query.value("TEST_LOG").toString();
        qDebug() << "[INFO] Loaded latest test log from DB, length:" << testLog.length();
    }

    // 回退：数据库异常时仍尝试使用当前页面缓存
    if (testLog.isEmpty()) {
        foreach(const zl::RecordInfo& record, serial_record_vec_) {
            if (record.record_id == recordId) {
                testLog = record.test_log;
                qDebug() << "[WARN] Fallback to cached test log, length:" << testLog.length();
                break;
            }
        }
    }

    if (testLog.isEmpty()) {
        qDebug() << "[WARN] No test log found for record:" << recordId;
        QMessageBox::warning(this, tr("Warning"), tr("No test log found for this record"));
        return;
    }

    showTestReportDialog(recordId, testLog);
}

void BackendWnd::on_serial_header_checkbox_clicked(bool checked)
{
    if (!model_serial_record_result_) return;
    
    int row_count = model_serial_record_result_->rowCount();
    for (int i = 0; i < row_count; ++i) {
        QStandardItem* item = model_serial_record_result_->item(i, 0);
        if (item && item->isCheckable()) {
            item->setCheckState(checked ? Qt::Checked : Qt::Unchecked);
        }
    }
}

void BackendWnd::init_extra_page_time_select()
{
    QDateTime today = QDateTime::currentDateTime();
    today.setTime(QTime(0, 0, 0));
    ui->edt_extra_record_begin_date->setDateTime(today);
    ui->edt_extra_record_end_date->setDateTime(today.addDays(1));
}

void BackendWnd::init_extra_page_combo_normal()
{
    const bool isCn = (APPMODEL()->CabinetLanguage() == zl::ELanguageType_Cn);

    ui->btn_navi_extra->setText(isCn ? QStringLiteral("新增测试") : QStringLiteral("Additional Test"));
    ui->lb_extra_record_type->setText(isCn ? QStringLiteral("测试类型:") : QStringLiteral("Test Type:"));
    ui->lb_extra_record_result->setText(isCn ? QStringLiteral("测试结果:") : QStringLiteral("Test Result:"));

    ui->combo_extra_record_type->clear();
    ui->combo_extra_record_type->addItem(isCn ? QStringLiteral("全部") : QStringLiteral("All"), QString());

    struct TypeItem {
        const char *key;
        const char *labelCn;
        const char *labelEn;
    };
    static const TypeItem items[] = {
        { "RS232 (CN35/CN36)", "RS232 (CN35/CN36)", "RS232 (CN35/CN36)" },
        { "RS232 (CN37/CN38)", "RS232 (CN37/CN38)", "RS232 (CN37/CN38)" },
        { "USB Port", "USB 口", "USB Port" },
        { "TF Card", "TF 卡", "TF Card" },
        { "Temp/Humidity (CN40)", "温湿度 (CN40)", "Temp/Humidity (CN40)" },
        { "Backlight", "背光调节", "Backlight" },
        { "Audio (CN22)", "音频 (CN22)", "Audio (CN22)" },
    };
    for (const TypeItem &item : items) {
        ui->combo_extra_record_type->addItem(
            isCn ? QString::fromUtf8(item.labelCn) : QString::fromLatin1(item.labelEn),
            QString::fromLatin1(item.key));
    }

    ui->combo_extra_record_result->clear();
    ui->combo_extra_record_result->addItem(isCn ? QStringLiteral("全部") : QStringLiteral("All"), -1);
    ui->combo_extra_record_result->addItem(isCn ? QStringLiteral("成功") : QStringLiteral("Success"), 1);
    ui->combo_extra_record_result->addItem(isCn ? QStringLiteral("失败") : QStringLiteral("Failure"), 0);
}

void BackendWnd::on_btn_extra_record_query_clicked()
{
    QDate beginDate = ui->edt_extra_record_begin_date->date();
    QDate endDateExclusive = ui->edt_extra_record_end_date->date().addDays(1);
    QString begdt = beginDate.toString("yyyy-MM-dd") + " 00:00:00";
    QString enddt = endDateExclusive.toString("yyyy-MM-dd") + " 00:00:00";

    ETestType test_type = ETestType_Extra;

    const QString selectedType = ui->combo_extra_record_type->currentData().toString();
    const int selectedResult = ui->combo_extra_record_result->currentData().toInt();

    extra_record_vec_.clear();

    int32_t ret = zl::TestRecordManager::getInstance()->GetAllRecord(
        extra_record_vec_,
        begdt,
        enddt,
        test_type,
        zl::EResultType_Unknow,
        zl::ESimNetStatus_Unknow,
        "",
        "",
        "",
        "",
        "");

    if (ret != zl::EResult_Success)
    {
        MsgWnd::ShowNormalInfo(QObject::tr("Query Additional Test Record Failed"));
        return;
    }

    if (!selectedType.isEmpty() || selectedResult >= 0) {
        zl::RecordVec filtered;
        for (const zl::RecordInfo &record : extra_record_vec_) {
            if (!selectedType.isEmpty()) {
                if (record.cmd_ret_info.trimmed() != selectedType)
                    continue;
            }
            if (selectedResult == 1) {
                if (record.result_type != zl::EResultType_Success)
                    continue;
            } else if (selectedResult == 0) {
                if (record.result_type == zl::EResultType_Success)
                    continue;
            }
            filtered.push_back(record);
        }
        extra_record_vec_ = filtered;
    }

    const bool isCn = (APPMODEL()->CabinetLanguage() == zl::ELanguageType_Cn);

    auto typeDisplay = [isCn](const QString &key) -> QString {
        if (key == QStringLiteral("USB Port"))
            return isCn ? QStringLiteral("USB 口") : key;
        if (key == QStringLiteral("TF Card"))
            return isCn ? QStringLiteral("TF 卡") : key;
        if (key == QStringLiteral("Temp/Humidity (CN40)"))
            return isCn ? QStringLiteral("温湿度 (CN40)") : key;
        if (key == QStringLiteral("Backlight"))
            return isCn ? QStringLiteral("背光调节") : key;
        if (key == QStringLiteral("Audio (CN22)"))
            return isCn ? QStringLiteral("音频 (CN22)") : key;
        return key.isEmpty() ? QStringLiteral("-") : key;
    };

    auto funcAddItem = [&](const zl::RecordInfo& record)
    {
        QList<QStandardItem*> row;

        QStandardItem* item_checkbox = new QStandardItem();
        item_checkbox->setCheckable(true);
        item_checkbox->setCheckState(Qt::Unchecked);
        item_checkbox->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        item_checkbox->setEditable(false);
        row.append(item_checkbox);

        QStandardItem* item_seq = new QStandardItem(record.record_id);
        item_seq->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        item_seq->setEditable(false);
        row.append(item_seq);

        QStandardItem* item_type = new QStandardItem(typeDisplay(record.cmd_ret_info));
        item_type->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        item_type->setEditable(false);
        row.append(item_type);

        const bool ok = (record.result_type == zl::EResultType_Success);
        QString resultText = ok
            ? (isCn ? QStringLiteral("成功") : QStringLiteral("Success"))
            : (isCn ? QStringLiteral("测试失败") : QStringLiteral("Failed"));
        if (ok && !record.result_info.trimmed().isEmpty()
            && record.result_info != QStringLiteral("Success")
            && record.result_info != QStringLiteral("成功"))
            resultText = record.result_info;

        QStandardItem* item_result = new QStandardItem(resultText);
        item_result->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        item_result->setEditable(false);
        if (!ok)
            item_result->setForeground(Qt::red);
        row.append(item_result);

        QStandardItem* item_time = new QStandardItem(record.test_time);
        item_time->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        row.append(item_time);

        QStandardItem* item_op = new QStandardItem(isCn ? QStringLiteral("查看报告") : tr("View Report"));
        item_op->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        item_op->setBackground(QBrush(QColor(0, 95, 167)));
        item_op->setForeground(Qt::white);
        row.append(item_op);

        model_extra_record_result_->appendRow(row);
    };

    TableHelper::readyUpdate(model_extra_record_result_, ui->tb_extra_record_result);

    foreach(const zl::RecordInfo record, extra_record_vec_)
    {
        funcAddItem(record);
    }

    TableHelper::finishUpdate(model_extra_record_result_, ui->tb_extra_record_result);
}

void BackendWnd::on_btn_extra_record_export_excel_clicked()
{
    int rowCount = model_extra_record_result_ ? model_extra_record_result_->rowCount() : 0;
    if (rowCount < 1)
    {
        MsgWnd::ShowNormalInfo(QObject::tr("Please query data first"));
        return;
    }

    QList<int> selectedRows;
    for (int i = 0; i < rowCount; ++i)
    {
        QStandardItem* cbItem = model_extra_record_result_->item(i, 0);
        if (cbItem && cbItem->checkState() == Qt::Checked)
            selectedRows.append(i);
    }

    if (selectedRows.isEmpty())
    {
        MsgWnd::ShowNormalInfo(QObject::tr("Please select records to export"));
        return;
    }

    QString timestamp = QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss");
    QString defaultFileName = QString("extra_test_report_%1.csv").arg(timestamp);
    QString fileName = QFileDialog::getSaveFileName(
        this,
        tr("Export Additional Test Report"),
        QDir::homePath() + "/" + defaultFileName,
        tr("CSV Files (*.csv);;All Files (*)")
    );

    if (fileName.isEmpty())
        return;

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QMessageBox::warning(this, tr("Export Failed"), tr("Cannot create file: %1").arg(fileName));
        return;
    }

    QTextStream out(&file);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    out.setEncoding(QStringConverter::Utf8);
#else
    out.setCodec("UTF-8");
#endif

    out << "\xEF\xBB\xBF";

    auto csvEscape = [](const QString& text) -> QString {
        QString escaped = text;
        escaped.replace("\"", "\"\"");
        return QString("\"%1\"").arg(escaped);
    };

    QStringList headers;
    for (int col = 1; col <= 4; ++col)
        headers.append(csvEscape(model_extra_record_result_->headerData(col, Qt::Horizontal).toString()));
    out << headers.join(",") << "\n";

    for (int row : selectedRows)
    {
        QStringList rowData;
        for (int col = 1; col <= 4; ++col)
            rowData.append(csvEscape(model_extra_record_result_->index(row, col).data().toString()));
        out << rowData.join(",") << "\n";
    }

    file.close();
    QMessageBox::information(this, tr("Export Success"), tr("Report exported successfully to: %1").arg(fileName));
}

void BackendWnd::on_btn_extra_record_reset_clicked()
{
    ui->combo_extra_record_type->setCurrentIndex(0);
    ui->combo_extra_record_result->setCurrentIndex(0);
    init_extra_page_time_select();
}

void BackendWnd::on_btn_extra_record_delete_clicked()
{
    QStringList record_list;
    int rowCount = model_extra_record_result_->rowCount();
    for (int i = 0; i < rowCount; ++i) {
        QStandardItem* cbItem = model_extra_record_result_->item(i, 0);
        if (cbItem && cbItem->checkState() == Qt::Checked) {
            QStandardItem* seqItem = model_extra_record_result_->item(i, 1);
            if (seqItem) record_list << seqItem->text();
        }
    }

    if (record_list.isEmpty()) return;

    pending_delete_records_ = record_list;
    pending_delete_type_ = "extra";
    MsgWnd::ShowDeleteConfirm(this);
}

void BackendWnd::on_tb_extra_record_result_doubleClicked(const QModelIndex& index)
{
    if (!index.isValid())
        return;

    QModelIndex recordIdIndex = model_extra_record_result_->index(index.row(), 1);
    QString recordId = model_extra_record_result_->data(recordIdIndex).toString();

    qDebug() << "[INFO] Double clicked Extra record, ID:" << recordId;

    QString testLog;
    QSqlQuery query = zl::CDBConnector::Instance()->CreateQuery();
    query.prepare("SELECT TEST_LOG FROM T_RECORD WHERE RECORD_ID=:record_id");
    query.bindValue(":record_id", recordId);
    int32_t ret = zl::CDBConnector::Instance()->ExecQuery(query);
    if (ret == zl::CDBConnector::EDBError_Success && query.next()) {
        testLog = query.value("TEST_LOG").toString();
    }

    if (testLog.isEmpty()) {
        foreach(const zl::RecordInfo& record, extra_record_vec_) {
            if (record.record_id == recordId) {
                testLog = record.test_log;
                break;
            }
        }
    }

    if (testLog.isEmpty()) {
        QMessageBox::warning(this, tr("Warning"), tr("No test log found for this record"));
        return;
    }

    showTestReportDialog(recordId, testLog);
}

void BackendWnd::on_extra_header_checkbox_clicked(bool checked)
{
    if (!model_extra_record_result_) return;

    int row_count = model_extra_record_result_->rowCount();
    for (int i = 0; i < row_count; ++i) {
        QStandardItem* item = model_extra_record_result_->item(i, 0);
        if (item)
            item->setCheckState(checked ? Qt::Checked : Qt::Unchecked);
    }
}
