/********************************************************************************
** Form generated from reading UI file 'BackendWnd.ui'
**
** Created by: Qt User Interface Compiler version 5.10.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_BACKENDWND_H
#define UI_BACKENDWND_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDateEdit>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QTableView>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_BackendWnd
{
public:
    QWidget *widget_bk;
    QStackedWidget *stackedWidget_bk;
    QWidget *page_sel;
    QWidget *wnd_sel;
    QGridLayout *gridLayout;
    QPushButton *btn_page_record;
    QPushButton *btn_page_advance;
    QWidget *page_record;
    QWidget *wnd_record;
    QVBoxLayout *verticalLayout_4;
    QWidget *wnd_record_header;
    QHBoxLayout *horizontalLayout_header;
    QPushButton *btn_record_back;
    QSpacerItem *header_spacer_left;
    QLabel *lb_record_title;
    QSpacerItem *header_spacer_right;
    QWidget *header_placeholder_right;
    QWidget *wnd_navi_type;
    QHBoxLayout *horizontalLayout_2;
    QToolButton *btn_navi_sim;
    QToolButton *btn_navi_iot;
    QSpacerItem *navi_spacer;
    QStackedWidget *stackedWidget;
    QWidget *page_sim;
    QVBoxLayout *verticalLayout_5;
    QWidget *wnd_sim_filters;
    QGridLayout *gridLayout_sim_filters;
    QLabel *lb_sim_record_iccid;
    QLineEdit *edt_sim_record_iccid;
    QLabel *lb_sim_record_net_status;
    QComboBox *combo_sim_record_net_status;
    QLabel *lb_sim_record_net_type;
    QComboBox *combo_sim_record_net_type;
    QLabel *lb_sim_record_test_time;
    QDateEdit *edt_sim_record_begin_date;
    QLabel *lb_sim_to;
    QDateEdit *edt_sim_record_end_date;
    QPushButton *btn_sim_record_query;
    QPushButton *btn_sim_record_reset;
    QSpacerItem *spacer_sim_filters;
    QWidget *wnd_sim_actions;
    QHBoxLayout *hbox_sim_actions;
    QPushButton *btn_sim_record_export_excel;
    QPushButton *btn_sim_record_delete;
    QSpacerItem *spacer_sim_actions;
    QTableView *tb_sim_record_result;
    QWidget *page_iot;
    QVBoxLayout *verticalLayout_7;
    QWidget *wnd_iot_filters;
    QGridLayout *gridLayout_iot_filters;
    QLabel *lb_iot_record_version;
    QLineEdit *edt_iot_record_version;
    QLabel *lb_iot_record_imei;
    QLineEdit *edt_iot_record_imei;
    QLabel *lb_iot_record_test_time;
    QDateEdit *edt_iot_record_begin_date;
    QLabel *lb_iot_to;
    QDateEdit *edt_iot_record_end_date;
    QPushButton *btn_iot_record_query;
    QPushButton *btn_iot_record_reset;
    QSpacerItem *spacer_iot_filters;
    QWidget *wnd_iot_actions;
    QHBoxLayout *hbox_iot_actions;
    QPushButton *btn_iot_record_export_excel;
    QPushButton *btn_iot_record_delete;
    QSpacerItem *spacer_iot_actions;
    QTableView *tb_iot_record_result;
    QWidget *page_advance;
    QWidget *wnd_advance;
    QVBoxLayout *verticalLayout_3;
    QWidget *wnd_serial_config;
    QVBoxLayout *verticalLayout_26;
    QLabel *lb_serial_info;
    QWidget *wnd_serial_config_container;
    QHBoxLayout *horizontalLayout_57;
    QLabel *lb_scaner_serialport;
    QComboBox *cb_serialport;
    QLabel *lb_scaner_baudrate;
    QComboBox *cb_baudrate;
    QSpacerItem *horizontalSpacer_35;
    QWidget *wnd_lang_select;
    QVBoxLayout *verticalLayout_27;
    QLabel *lb_lang_info;
    QWidget *wnd_select_lang_container;
    QHBoxLayout *horizontalLayout_58;
    QPushButton *btn_lang_cn;
    QPushButton *btn_lang_en;
    QSpacerItem *horizontalSpacer_36;
    QSpacerItem *verticalSpacer_4;
    QWidget *wnd_advance_bottom_btn;
    QHBoxLayout *horizontalLayout_30;
    QSpacerItem *horizontalSpacer_27;
    QPushButton *btn_save_advance;

    void setupUi(QWidget *BackendWnd)
    {
        if (BackendWnd->objectName().isEmpty())
            BackendWnd->setObjectName(QStringLiteral("BackendWnd"));
        BackendWnd->resize(1280, 710);
        BackendWnd->setStyleSheet(QLatin1String("#BackendWnd\n"
"{\n"
"	background: transparent;\n"
"	background-color: #FFFFFF;\n"
"}\n"
"\n"
"QLable\n"
"{\n"
"	background: transparent;\n"
"}\n"
"\n"
"#widget_bk\n"
"{\n"
"	background-color: #E9F2F8;\n"
"}\n"
"\n"
"#stackedWidget_bk\n"
",#page_sel\n"
",#page_record\n"
",#page_advance\n"
"\n"
"{\n"
"	background: transparent;\n"
"/*	background-color: #ffffff;*/\n"
"}"));
        widget_bk = new QWidget(BackendWnd);
        widget_bk->setObjectName(QStringLiteral("widget_bk"));
        widget_bk->setGeometry(QRect(0, 10, 1280, 668));
        stackedWidget_bk = new QStackedWidget(widget_bk);
        stackedWidget_bk->setObjectName(QStringLiteral("stackedWidget_bk"));
        stackedWidget_bk->setGeometry(QRect(0, 0, 1280, 668));
        stackedWidget_bk->setMinimumSize(QSize(1280, 668));
        stackedWidget_bk->setMaximumSize(QSize(1280, 668));
        stackedWidget_bk->setStyleSheet(QString::fromUtf8("QLabel\n"
"{\n"
"	font: 18pt \"\345\276\256\350\275\257\351\233\205\351\273\221\";\n"
"	color: #005FA7;\n"
"}\n"
"\n"
"QLabel:disabled\n"
"{\n"
"	color: #CDCDCD;\n"
"}\n"
"\n"
"QComboBox {\n"
"	border: 1px solid #BBBBBB;\n"
"	border-radius: 5px;\n"
"	padding-left: 10px;\n"
"	\n"
"	font: 16pt \"\345\276\256\350\275\257\351\233\205\351\273\221\";\n"
"	color: #005FA7;\n"
"	background: transparent;\n"
"}\n"
"\n"
"QComboBox::disabled \n"
"{\n"
"	border: 1px solid #999999;\n"
"	border-radius: 5px;\n"
"	padding: 2px;\n"
"	font: 16pt \"\345\276\256\350\275\257\351\233\205\351\273\221\";\n"
"	color: #999999;\n"
"	background: transparent;\n"
"}\n"
"\n"
"QComboBox:focus, QComboBox:hover {\n"
"	border: 1px solid #008bcc;\n"
"}\n"
"\n"
"QComboBox::drop-down {\n"
"	width: 32px;\n"
"	border: none;\n"
"	background: transparent;\n"
"}\n"
"\n"
"QComboBox::down-arrow {\n"
"	width: 14px;\n"
"	height: 8px;\n"
"	border-image: url(:/img/backend/icon-up.png);\n"
"}\n"
"\n"
"QComboBox QAbstractItemView\n"
"{\n"
"     border: 1px solid"
                        " rgb(161,161,161);\n"
"}\n"
" \n"
"QComboBox QAbstractItemView::item\n"
"{\n"
"    height: 60px;\n"
"}\n"
" \n"
"QComboBox QAbstractItemView::item:selected\n"
"{	\n"
"    background-color: rgba(54, 98, 180);\n"
"}\n"
"\n"
"\n"
"\n"
"QDateEdit {\n"
"	border: 1px solid #BBBBBB;\n"
"	border-radius: 5px;\n"
"	font: 14pt \"\345\276\256\350\275\257\351\233\205\351\273\221\";\n"
"	color: #005FA7;\n"
"	padding: 2px;\n"
"	background: transparent;\n"
"}\n"
"\n"
"QDateEdit:focus, QDateEdit:hover {\n"
"	border: 1px solid #008bcc;\n"
"}\n"
"\n"
"QDateEdit::drop-down {\n"
"	width: 32px;\n"
"	border: none;\n"
"	background: transparent;\n"
"}\n"
"\n"
"QDateEdit:down-arrow {\n"
"	width: 14px;\n"
"	height: 8px;\n"
"	border-image: url(:/img/backend/icon-up.png);    \n"
"}\n"
"\n"
"QDateEdit\n"
"{\n"
"	padding-left: 10px;\n"
"}\n"
"\n"
"#scrollArea{\n"
"	border: 1px solid #999999;\n"
"	background:transparent;\n"
"}\n"
"\n"
"QPushButton\n"
"{\n"
"	border: none;\n"
"	border-radius:5px;\n"
"	background-color:#005FA7;\n"
"	\n"
"	font"
                        ": 16pt \"\345\276\256\350\275\257\351\233\205\351\273\221\";\n"
"	color:#FFFFFF;\n"
"}\n"
"\n"
"QPushButton:pressed\n"
"{\n"
"	background-color:#74a8cf;\n"
"}\n"
"\n"
"QPushButton:disabled\n"
"{\n"
"	background-color:#999999;\n"
"}\n"
"\n"
"QTableView {\n"
"	padding: 3px;\n"
"	border: 1px solid #999999;\n"
"	border-radius: 1px;\n"
"	background: transparent;\n"
"/*	background: #FFF;*/\n"
"\n"
"	font: 16pt \"\345\276\256\350\275\257\351\233\205\351\273\221\";\n"
"	color: #333333;\n"
"\n"
"	selection-color: #333333; \n"
"	selection-background-color:#7FAFD3;\n"
"}\n"
"\n"
"QTableView:focus, QTableView:hover {\n"
"	border: 1px solid #008bcc;\n"
"}\n"
"\n"
"QTableView QAbstractItemView::item {\n"
"	padding-left: 6px;\n"
"}\n"
"\n"
"QTableView QHeaderView {\n"
"	border: 1px solid #999999;\n"
"	background: transparent;\n"
"}\n"
"\n"
"QTableView QHeaderView::section{\n"
"	border: 1px solid #999999;\n"
"	height:40px;\n"
"	color: #FFFFFF;\n"
"        font: 12pt \"\345\276\256\350\275\257\351\233\205\351\273\221\";\n"
"  "
                        "      background-color:#9fa9b6;\n"
"        border-bottom: 1px solid #eaeaea;\n"
"        padding-left: 6px;\n"
"}\n"
"\n"
"QLineEdit:focus, QLineEdit:hover {\n"
"	border: 1px solid #008bcc;\n"
"}\n"
"\n"
"QLineEdit:disabled\n"
"{\n"
"	color: #CDCDCD;\n"
"}\n"
"\n"
"/* combobox \346\273\221\345\212\250\346\235\241*/\n"
"QComboBox QScrollBar:vertical\n"
"{\n"
"    width:60px;\n"
"    margin:0px,0px,0px,0px;\n"
"	border-image: url(:/img/scroll_bar_large/v/bg.png);\n"
"    padding-top:55px;\n"
"    padding-bottom:55px;\n"
"}\n"
"\n"
"QComboBox QScrollBar::handle:vertical\n"
"{\n"
"    width:40px;\n"
"\n"
"    min-height:20px;\n"
"	margin:0px 10px 0px 10px;\n"
"	\n"
"	border-image: url(:/img/scroll_bar_large/v/thumb_up.png);\n"
"}\n"
"\n"
"QComboBox QScrollBar::handle:vertical:hover\n"
"{\n"
"	border-image: url(:/img/scroll_bar_large/v/thumb_click.png);\n"
"}\n"
"\n"
"QComboBox QScrollBar::handle:vertical:pressed\n"
"{\n"
"	border-image: url(:/img/scroll_bar_large/v/thumb_down.png);\n"
"}\n"
"\n"
"QComboBox QScrol"
                        "lBar::sub-line:vertical\n"
"{\n"
"	border-radius: 2px;\n"
"	width:55px;\n"
"    height:55px;\n"
"	\n"
"	border-image: url(:/img/scroll_bar_large/v/btn_down_up.png);\n"
"	subcontrol-position: top;\n"
"}\n"
"\n"
"QComboBox QScrollBar::sub-line:vertical:hover\n"
"{\n"
"	border-image: url(:/img/scroll_bar_large/v/btn_down_click.png);\n"
"}\n"
"\n"
"QComboBox QScrollBar::sub-line:vertical:pressed\n"
"{\n"
"	border-image: url(:/img/scroll_bar_large/v/btn_down_down.png);\n"
"}\n"
"\n"
"QComboBox QScrollBar::add-line:vertical\n"
"{\n"
"	border-radius: 2px;\n"
"\n"
"	width:55px;\n"
"    height:55px;\n"
"	\n"
"	border-image: url(:/img/scroll_bar_large/v/btn_up_up.png);\n"
"    subcontrol-position:bottom;\n"
"}\n"
"\n"
"QComboBox QScrollBar::add-line:vertical:hover\n"
"{\n"
"	border-image: url(:/img/scroll_bar_large/v/btn_up_click.png);\n"
"}\n"
"\n"
"QComboBox QScrollBar::add-line:vertical:pressed\n"
"{\n"
"	border-image: url(:/img/scroll_bar_large/v/btn_up_down.png);\n"
"}\n"
"\n"
"QComboBox QScrollBar::add-page:vertic"
                        "al,QScrollBar::sub-page:vertical\n"
"{\n"
"    border-width:1px;\n"
"}\n"
"\n"
"\n"
"QComboBox QScrollBar:horizontal \n"
"{\n"
"    height:60px;\n"
"    margin:0px,0px,0px,0px;\n"
"	border-image: url(:/img/scroll_bar_large/h/bg.png);\n"
"    background:rgba(0,0,0,0%);\n"
"    padding-left:55px;\n"
"    padding-right:55px;\n"
"}\n"
"\n"
"QComboBox QScrollBar::handle:horizontal \n"
"{\n"
"    height:40px;\n"
"    min-width:20px;\n"
"	margin:10px 0px 10px 0px;\n"
"	border-width:2px;\n"
"	\n"
"	border-image: url(:/img/scroll_bar_large/h/thumb_up.png);\n"
"}\n"
"\n"
"QComboBox QScrollBar::handle:horizontal:hover\n"
"{\n"
"	border-image: url(:/img/scroll_bar_large/h/thumb_click.png);\n"
"}\n"
"\n"
"QComboBox QScrollBar::handle:horizontal:pressed\n"
"{\n"
"	border-image: url(:/img/scroll_bar_large/h/thumb_down.png);\n"
"}\n"
"\n"
"QComboBox QScrollBar::sub-line:horizontal\n"
"{\n"
"	border-radius: 2px;\n"
"	width:55px;\n"
"    height:55px;\n"
"	\n"
"	border-image: url(:/img/scroll_bar_large/h/btn_left_up.png);\n"
"	s"
                        "ubcontrol-position: left;\n"
"}\n"
"\n"
"QComboBox QScrollBar::sub-line:horizontal:hover\n"
"{\n"
"	border-image: url(:/img/scroll_bar_large/h/btn_left_click.png);\n"
"}\n"
"\n"
"QComboBox QScrollBar::sub-line:horizontal:pressed\n"
"{\n"
"	border-image: url(:/img/scroll_bar_large/h/btn_left_down.png);\n"
"}\n"
"\n"
"QComboBox QScrollBar::add-line:horizontal\n"
"{\n"
"	border-radius: 2px;\n"
"\n"
"	width:55px;\n"
"    height:55px;\n"
"	\n"
"	border-image: url(:/img/scroll_bar_large/h/btn_right_up.png);\n"
"    subcontrol-position:right;\n"
"}\n"
"\n"
"QComboBox QScrollBar::add-line:horizontal:hover\n"
"{\n"
"	border-image: url(:/img/scroll_bar_large/h/btn_right_click.png);\n"
"}\n"
"\n"
"QComboBox QScrollBar::add-line:horizontal:pressed\n"
"{\n"
"	border-image: url(:/img/scroll_bar_large/h/btn_right_down.png);\n"
"}\n"
"\n"
"QComboBox QScrollBar::add-page:horizontal,QScrollBar::sub-page:horizontal\n"
"{\n"
"    border-width:1px;\n"
"}\n"
"\n"
"/* \346\227\245\346\234\237\350\241\250 */\n"
"/*QDateEdit,QDateTime"
                        "Edit\n"
"{\n"
"  min-width:190px;\n"
"  height:50px;\n"
"  font-size: 26px;\n"
"  background-color: white;\n"
"  border: 1px solid #999999;\n"
"  border-radius:2px;\n"
"}\n"
"\n"
"#dateTimeEdit{\n"
"  min-width:240px;\n"
"}\n"
"\n"
"QDateEdit::drop-down,QDateTimeEdit::drop-down{\n"
"  width: 50px;\n"
"  border:none;     \n"
"  image: url(:/resource/image/backstage/icon_down.png);\n"
"}*/\n"
"\n"
"QCalendarWidget {\n"
"  min-width:440px;\n"
"  min-height:300px;\n"
"}\n"
"\n"
"QCalendarWidget QWidget#qt_calendar_navigationbar {\n"
"  background-color: #0494DB;\n"
"}\n"
"QToolButton#qt_calendar_monthbutton{\n"
"  width:120px;\n"
"}\n"
"QToolButton#qt_calendar_yearbutton{\n"
"  width:200px;\n"
"}\n"
"QCalendarWidget QToolButton {\n"
"  height: 50px;\n"
"  color:white;\n"
"  font-size:26px;\n"
"    icon-size: 50px, 50px;\n"
"    background-color: #0494DB;\n"
"}\n"
"QCalendarWidget QMenu {\n"
"    width: 160px;\n"
"    left: 20px;\n"
"    color: #333333;\n"
"    font-size: 30px;\n"
"    background-color: white;\n"
""
                        "}\n"
"QCalendarWidget QMenu::item:selected {\n"
"  border-color: darkblue;\n"
"  background: #0494DB;\n"
"}\n"
"\n"
"QCalendarWidget QSpinBox,QTimeEdit { \n"
"    width: 130px; \n"
"    font-size:26px; \n"
"    color: white; \n"
"    background-color: transparent; \n"
"    selection-background-color: transparent;\n"
"    selection-color: white;\n"
"}\n"
"QTimeEdit { \n"
"    width: 130px; \n"
"    font-size:26px; \n"
"    color: #333333; \n"
"    background-color: transparent; \n"
"    selection-background-color: transparent;\n"
"    selection-color: #0494DB;\n"
"}\n"
"QCalendarWidget QSpinBox::up-button ,QTimeEdit::up-button{ \n"
"  subcontrol-origin: border;  \n"
"  subcontrol-position: center right;  \n"
"  width:70px; \n"
"}\n"
"QCalendarWidget QSpinBox::down-button,QTimeEdit::down-button {\n"
"  subcontrol-origin: border; \n"
"  subcontrol-position: center left;  \n"
"  width:70px;\n"
"}\n"
"\n"
" \n"
"/* header row */\n"
"QCalendarWidget QWidget { \n"
"  alternate-background-color: white; \n"
"}\n"
"\n"
""
                        "QCalendarWidget QAbstractItemView\n"
"{\n"
"  border:none;\n"
"    font-size:23px;  \n"
"  color:#333333;\n"
"    background-color: white;  \n"
"    selection-background-color: #0494DB; \n"
"    selection-color: white; \n"
"}\n"
" \n"
"/* days in other months */\n"
"QCalendarWidget QAbstractItemView:disabled { \n"
"  color: #999999; \n"
"}\n"
"\n"
"QTimeEdit \n"
"{ \n"
"	font: 24pt \"\345\276\256\350\275\257\351\233\205\351\273\221\";\n"
"	color: #333333;\n"
"\n"
"	min-height: 50px;\n"
"	max-height: 50px;\n"
"\n"
"    background-color: transparent; \n"
"\n"
"    selection-background-color: transparent;\n"
"    selection-color: #0494DB;\n"
"}\n"
"\n"
"QTimeEdit:disabled \n"
"{\n"
"	font: 24pt \"\345\276\256\350\275\257\351\233\205\351\273\221\"; \n"
"	color: #999999;\n"
"\n"
"    background-color: #cdcdcd;\n"
"\n"
"    selection-background-color: transparent;\n"
"    selection-color: #0494DB;\n"
"\n"
"		\n"
"}\n"
"\n"
"/*spinbox \346\212\254\350\265\267\346\240\267\345\274\217*/\n"
"QTimeEdit::up-button,QDouble"
                        "SpinBox::up-button,QSpinBox::up-button {subcontrol-origin:border;\n"
"    subcontrol-position:right;\n"
"    image: url(:/img/icon/btn_on_right.png);\n"
"    width: 42px;\n"
"    height: 70px;       \n"
"}\n"
"QTimeEdit::down-button,QDoubleSpinBox::down-button,QSpinBox::down-button {subcontrol-origin:border;\n"
"    subcontrol-position:left;\n"
"    border-image: url(:/img/icon/btn_on_left.png);\n"
"    width: 42px;\n"
"    height: 70px;\n"
"}\n"
"/*\346\214\211\351\222\256\346\214\211\344\270\213\346\240\267\345\274\217*/\n"
"QTimeEdit::up-button:pressed,QDoubleSpinBox::up-button:pressed,QSpinBox::up-button:pressed{subcontrol-origin:border;\n"
"    subcontrol-position:right;\n"
"    image: url(:/img/icon/btn_off_right.png);\n"
"    width: 42px;\n"
"    height: 70px;       \n"
"}\n"
"  \n"
"QTimeEdit::down-button:pressed,QDoubleSpinBox::down-button:pressed,QSpinBox::down-button:pressed{\n"
"    subcontrol-position:left;\n"
"    image: url(:/img/icon/btn_off_left.png);\n"
"    width: 42px;\n"
"    height: 70px;"
                        "\n"
"}\n"
"\n"
"/*\346\214\211\351\222\256\347\246\201\347\224\250\346\240\267\345\274\217*/\n"
"QTimeEdit::up-button:disabled,QDoubleSpinBox::up-button:disabled,QSpinBox::up-button:disabled{subcontrol-origin:border;\n"
"    subcontrol-position:right;\n"
"    image: url(:/img/icon/btn_off_right.png);\n"
"    width: 42px;\n"
"    height: 70px;       \n"
"}\n"
"  \n"
"QTimeEdit::down-button:disabled,QDoubleSpinBox::down-button:disabled,QSpinBox::down-button:disabled{\n"
"    subcontrol-position:left;\n"
"    image: url(:/img/icon/btn_off_left.png);\n"
"    width: 42px;\n"
"    height: 70px;\n"
"}\n"
"\n"
"/*check box \346\240\267\345\274\217*/\n"
"QCheckBox {\n"
"    spacing: 5px;\n"
"}\n"
"\n"
"QCheckBox::indicator {\n"
"    width: 42px;\n"
"    height: 42px;\n"
"}\n"
"\n"
"QCheckBox::indicator:unchecked {\n"
"    image: url(:/img/icon/ico_Choice_off.png);\n"
"}\n"
"\n"
"QCheckBox::indicator:unchecked:pressed {\n"
"    image: url(:/img/icon/ico_Choice_down.png);\n"
"}\n"
"\n"
"QCheckBox::indicator:checked {\n"
""
                        "    image: url(:/img/icon/ico_Choice_on.png);\n"
"}\n"
"\n"
"QCheckBox::indicator:checked:pressed {\n"
"    image: url(:/img/icon/ico_Choice_Close.png);\n"
"}\n"
""));
        page_sel = new QWidget();
        page_sel->setObjectName(QStringLiteral("page_sel"));
        page_sel->setStyleSheet(QString::fromUtf8("#wnd_sel\n"
"{\n"
"	background:transparent;\n"
"}\n"
"\n"
"#btn_page_advance\n"
"{\n"
"	font: 75 22pt \"\345\276\256\350\275\257\351\233\205\351\273\221\";\n"
"	color:#FFFFFF;\n"
"\n"
"	padding-top:120px;\n"
"\n"
"	background:transparent;\n"
"	border-image: url(:/img/backend/ico_record.png);\n"
"}\n"
"\n"
"#btn_page_advance:pressed\n"
"{\n"
"	background:transparent;\n"
"	border-image: url(:/img/backend/ico_record_on.png);\n"
"}\n"
"\n"
"#btn_page_record\n"
"{\n"
"	font: 75 22pt \"\345\276\256\350\275\257\351\233\205\351\273\221\";\n"
"	color:#FFFFFF;\n"
"\n"
"	padding-top:120px;\n"
"\n"
"	background:transparent;\n"
"	border-image: url(:/img/backend/ico_set.png);\n"
"}\n"
"\n"
"#btn_page_record:pressed\n"
"{\n"
"	background:transparent;\n"
"	border-image: url(:/img/backend/ico_set_on.png);\n"
"}"));
        wnd_sel = new QWidget(page_sel);
        wnd_sel->setObjectName(QStringLiteral("wnd_sel"));
        wnd_sel->setGeometry(QRect(0, 0, 1280, 678));
        gridLayout = new QGridLayout(wnd_sel);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        gridLayout->setHorizontalSpacing(82);
        gridLayout->setContentsMargins(252, 107, 252, 361);
        btn_page_record = new QPushButton(wnd_sel);
        btn_page_record->setObjectName(QStringLiteral("btn_page_record"));
        btn_page_record->setMinimumSize(QSize(200, 200));
        btn_page_record->setMaximumSize(QSize(200, 200));

        gridLayout->addWidget(btn_page_record, 0, 1, 1, 1);

        btn_page_advance = new QPushButton(wnd_sel);
        btn_page_advance->setObjectName(QStringLiteral("btn_page_advance"));
        btn_page_advance->setMinimumSize(QSize(200, 200));
        btn_page_advance->setMaximumSize(QSize(200, 200));

        gridLayout->addWidget(btn_page_advance, 0, 2, 1, 1);

        stackedWidget_bk->addWidget(page_sel);
        page_record = new QWidget();
        page_record->setObjectName(QStringLiteral("page_record"));
        page_record->setStyleSheet(QString::fromUtf8("#wnd_record { background-color: #F5F5F5; }\n"
"#wnd_record_header { background-color: #FFFFFF; border-bottom: 1px solid #E0E0E0; }\n"
"#btn_record_back { border: none; background: transparent; font: bold 16pt \"\345\276\256\350\275\257\351\233\205\351\273\221\"; color: #333333; }\n"
"#lb_record_title { font: bold 18pt \"\345\276\256\350\275\257\351\233\205\351\273\221\"; color: #333333; }\n"
"#wnd_navi_type { background-color: #FFFFFF; padding-left: 20px; }\n"
"#wnd_navi_type QToolButton { border: none; background: transparent; font: 16pt \"\345\276\256\350\275\257\351\233\205\351\273\221\"; color: #666666; padding-bottom: 5px; }\n"
"#wnd_navi_type QToolButton:hover { color: #005FA7; }\n"
"#wnd_navi_type QToolButton:checked { color: #005FA7; border-bottom: 3px solid #005FA7; font-weight: bold; }\n"
""));
        wnd_record = new QWidget(page_record);
        wnd_record->setObjectName(QStringLiteral("wnd_record"));
        wnd_record->setGeometry(QRect(0, 0, 1280, 668));
        wnd_record->setMinimumSize(QSize(1280, 668));
        wnd_record->setMaximumSize(QSize(1280, 668));
        verticalLayout_4 = new QVBoxLayout(wnd_record);
        verticalLayout_4->setSpacing(0);
        verticalLayout_4->setObjectName(QStringLiteral("verticalLayout_4"));
        verticalLayout_4->setContentsMargins(0, 0, 0, 0);
        wnd_record_header = new QWidget(wnd_record);
        wnd_record_header->setObjectName(QStringLiteral("wnd_record_header"));
        wnd_record_header->setMinimumSize(QSize(0, 60));
        wnd_record_header->setMaximumSize(QSize(16777215, 60));
        horizontalLayout_header = new QHBoxLayout(wnd_record_header);
        horizontalLayout_header->setObjectName(QStringLiteral("horizontalLayout_header"));
        horizontalLayout_header->setContentsMargins(10, -1, 10, -1);
        btn_record_back = new QPushButton(wnd_record_header);
        btn_record_back->setObjectName(QStringLiteral("btn_record_back"));
        btn_record_back->setMaximumSize(QSize(50, 16777215));

        horizontalLayout_header->addWidget(btn_record_back);

        header_spacer_left = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_header->addItem(header_spacer_left);

        lb_record_title = new QLabel(wnd_record_header);
        lb_record_title->setObjectName(QStringLiteral("lb_record_title"));
        lb_record_title->setAlignment(Qt::AlignCenter);

        horizontalLayout_header->addWidget(lb_record_title);

        header_spacer_right = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_header->addItem(header_spacer_right);

        header_placeholder_right = new QWidget(wnd_record_header);
        header_placeholder_right->setObjectName(QStringLiteral("header_placeholder_right"));
        header_placeholder_right->setMaximumSize(QSize(50, 1));

        horizontalLayout_header->addWidget(header_placeholder_right);


        verticalLayout_4->addWidget(wnd_record_header);

        wnd_navi_type = new QWidget(wnd_record);
        wnd_navi_type->setObjectName(QStringLiteral("wnd_navi_type"));
        wnd_navi_type->setMinimumSize(QSize(0, 50));
        wnd_navi_type->setMaximumSize(QSize(16777215, 50));
        horizontalLayout_2 = new QHBoxLayout(wnd_navi_type);
        horizontalLayout_2->setSpacing(30);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(20, 0, 0, 0);
        btn_navi_sim = new QToolButton(wnd_navi_type);
        btn_navi_sim->setObjectName(QStringLiteral("btn_navi_sim"));
        btn_navi_sim->setMinimumSize(QSize(160, 40));
        btn_navi_sim->setMaximumSize(QSize(160, 40));
        btn_navi_sim->setCheckable(true);
        btn_navi_sim->setChecked(true);
        btn_navi_sim->setAutoExclusive(true);
        btn_navi_sim->setToolButtonStyle(Qt::ToolButtonTextOnly);

        horizontalLayout_2->addWidget(btn_navi_sim);

        btn_navi_iot = new QToolButton(wnd_navi_type);
        btn_navi_iot->setObjectName(QStringLiteral("btn_navi_iot"));
        btn_navi_iot->setEnabled(true);
        btn_navi_iot->setMinimumSize(QSize(120, 40));
        btn_navi_iot->setMaximumSize(QSize(120, 40));
        btn_navi_iot->setCheckable(true);
        btn_navi_iot->setAutoExclusive(true);
        btn_navi_iot->setToolButtonStyle(Qt::ToolButtonTextOnly);

        horizontalLayout_2->addWidget(btn_navi_iot);

        navi_spacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(navi_spacer);


        verticalLayout_4->addWidget(wnd_navi_type);

        stackedWidget = new QStackedWidget(wnd_record);
        stackedWidget->setObjectName(QStringLiteral("stackedWidget"));
        page_sim = new QWidget();
        page_sim->setObjectName(QStringLiteral("page_sim"));
        page_sim->setStyleSheet(QString::fromUtf8("\n"
"#wnd_sim_filters QLabel { font: 12pt \"\345\276\256\350\275\257\351\233\205\351\273\221\"; color: #333333; }\n"
"#wnd_sim_filters QLineEdit { border: 1px solid #CCCCCC; border-radius: 4px; padding: 5px; font: 12pt \"\345\276\256\350\275\257\351\233\205\351\273\221\"; background: #FFFFFF; }\n"
"#wnd_sim_filters QComboBox { border: 1px solid #CCCCCC; border-radius: 4px; padding: 5px; font: 12pt \"\345\276\256\350\275\257\351\233\205\351\273\221\"; background: #FFFFFF; }\n"
"#wnd_sim_filters QDateEdit { border: 1px solid #CCCCCC; border-radius: 4px; padding: 5px; font: 12pt \"\345\276\256\350\275\257\351\233\205\351\273\221\"; background: #FFFFFF; }\n"
"#btn_sim_record_query { background-color: #005FA7; color: #FFFFFF; border-radius: 4px; font: 12pt \"\345\276\256\350\275\257\351\233\205\351\273\221\"; border: none; }\n"
"#btn_sim_record_reset { background-color: #FFFFFF; color: #005FA7; border: 1px solid #005FA7; border-radius: 4px; font: 12pt \"\345\276\256\350\275\257\351\233\205\351\273\221\"; }\n"
"#btn"
                        "_sim_record_export_excel { background-color: #005FA7; color: #FFFFFF; border-radius: 4px; font: 12pt \"\345\276\256\350\275\257\351\233\205\351\273\221\"; border: none; }\n"
"#btn_sim_record_delete { background-color: #FF4D4F; color: #FFFFFF; border-radius: 4px; font: 12pt \"\345\276\256\350\275\257\351\233\205\351\273\221\"; border: none; }\n"
"             "));
        verticalLayout_5 = new QVBoxLayout(page_sim);
        verticalLayout_5->setSpacing(15);
        verticalLayout_5->setObjectName(QStringLiteral("verticalLayout_5"));
        verticalLayout_5->setContentsMargins(20, 20, 20, 20);
        wnd_sim_filters = new QWidget(page_sim);
        wnd_sim_filters->setObjectName(QStringLiteral("wnd_sim_filters"));
        gridLayout_sim_filters = new QGridLayout(wnd_sim_filters);
        gridLayout_sim_filters->setSpacing(15);
        gridLayout_sim_filters->setContentsMargins(0, 0, 0, 0);
        gridLayout_sim_filters->setObjectName(QStringLiteral("gridLayout_sim_filters"));
        lb_sim_record_iccid = new QLabel(wnd_sim_filters);
        lb_sim_record_iccid->setObjectName(QStringLiteral("lb_sim_record_iccid"));

        gridLayout_sim_filters->addWidget(lb_sim_record_iccid, 0, 0, 1, 1);

        edt_sim_record_iccid = new QLineEdit(wnd_sim_filters);
        edt_sim_record_iccid->setObjectName(QStringLiteral("edt_sim_record_iccid"));
        edt_sim_record_iccid->setMinimumSize(QSize(200, 40));

        gridLayout_sim_filters->addWidget(edt_sim_record_iccid, 1, 0, 1, 1);

        lb_sim_record_net_status = new QLabel(wnd_sim_filters);
        lb_sim_record_net_status->setObjectName(QStringLiteral("lb_sim_record_net_status"));

        gridLayout_sim_filters->addWidget(lb_sim_record_net_status, 0, 1, 1, 1);

        combo_sim_record_net_status = new QComboBox(wnd_sim_filters);
        combo_sim_record_net_status->setObjectName(QStringLiteral("combo_sim_record_net_status"));
        combo_sim_record_net_status->setMinimumSize(QSize(150, 40));

        gridLayout_sim_filters->addWidget(combo_sim_record_net_status, 1, 1, 1, 1);

        lb_sim_record_net_type = new QLabel(wnd_sim_filters);
        lb_sim_record_net_type->setObjectName(QStringLiteral("lb_sim_record_net_type"));

        gridLayout_sim_filters->addWidget(lb_sim_record_net_type, 0, 2, 1, 1);

        combo_sim_record_net_type = new QComboBox(wnd_sim_filters);
        combo_sim_record_net_type->setObjectName(QStringLiteral("combo_sim_record_net_type"));
        combo_sim_record_net_type->setMinimumSize(QSize(150, 40));

        gridLayout_sim_filters->addWidget(combo_sim_record_net_type, 1, 2, 1, 1);

        lb_sim_record_test_time = new QLabel(wnd_sim_filters);
        lb_sim_record_test_time->setObjectName(QStringLiteral("lb_sim_record_test_time"));

        gridLayout_sim_filters->addWidget(lb_sim_record_test_time, 0, 3, 1, 3);

        edt_sim_record_begin_date = new QDateEdit(wnd_sim_filters);
        edt_sim_record_begin_date->setObjectName(QStringLiteral("edt_sim_record_begin_date"));
        edt_sim_record_begin_date->setCalendarPopup(true);
        edt_sim_record_begin_date->setMinimumSize(QSize(180, 40));

        gridLayout_sim_filters->addWidget(edt_sim_record_begin_date, 1, 3, 1, 1);

        lb_sim_to = new QLabel(wnd_sim_filters);
        lb_sim_to->setObjectName(QStringLiteral("lb_sim_to"));
        lb_sim_to->setAlignment(Qt::AlignCenter);
        lb_sim_to->setMinimumSize(QSize(20, 40));

        gridLayout_sim_filters->addWidget(lb_sim_to, 1, 4, 1, 1);

        edt_sim_record_end_date = new QDateEdit(wnd_sim_filters);
        edt_sim_record_end_date->setObjectName(QStringLiteral("edt_sim_record_end_date"));
        edt_sim_record_end_date->setCalendarPopup(true);
        edt_sim_record_end_date->setMinimumSize(QSize(180, 40));

        gridLayout_sim_filters->addWidget(edt_sim_record_end_date, 1, 5, 1, 1);

        btn_sim_record_query = new QPushButton(wnd_sim_filters);
        btn_sim_record_query->setObjectName(QStringLiteral("btn_sim_record_query"));
        btn_sim_record_query->setMinimumSize(QSize(80, 40));

        gridLayout_sim_filters->addWidget(btn_sim_record_query, 1, 6, 1, 1);

        btn_sim_record_reset = new QPushButton(wnd_sim_filters);
        btn_sim_record_reset->setObjectName(QStringLiteral("btn_sim_record_reset"));
        btn_sim_record_reset->setMinimumSize(QSize(80, 40));

        gridLayout_sim_filters->addWidget(btn_sim_record_reset, 1, 7, 1, 1);

        spacer_sim_filters = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_sim_filters->addItem(spacer_sim_filters, 1, 8, 1, 1);


        verticalLayout_5->addWidget(wnd_sim_filters);

        wnd_sim_actions = new QWidget(page_sim);
        wnd_sim_actions->setObjectName(QStringLiteral("wnd_sim_actions"));
        hbox_sim_actions = new QHBoxLayout(wnd_sim_actions);
        hbox_sim_actions->setSpacing(10);
        hbox_sim_actions->setContentsMargins(0, 0, 0, 0);
        hbox_sim_actions->setObjectName(QStringLiteral("hbox_sim_actions"));
        btn_sim_record_export_excel = new QPushButton(wnd_sim_actions);
        btn_sim_record_export_excel->setObjectName(QStringLiteral("btn_sim_record_export_excel"));
        btn_sim_record_export_excel->setMinimumSize(QSize(80, 40));

        hbox_sim_actions->addWidget(btn_sim_record_export_excel);

        btn_sim_record_delete = new QPushButton(wnd_sim_actions);
        btn_sim_record_delete->setObjectName(QStringLiteral("btn_sim_record_delete"));
        btn_sim_record_delete->setMinimumSize(QSize(80, 40));

        hbox_sim_actions->addWidget(btn_sim_record_delete);

        spacer_sim_actions = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);

        hbox_sim_actions->addItem(spacer_sim_actions);


        verticalLayout_5->addWidget(wnd_sim_actions);

        tb_sim_record_result = new QTableView(page_sim);
        tb_sim_record_result->setObjectName(QStringLiteral("tb_sim_record_result"));
        tb_sim_record_result->setSelectionBehavior(QAbstractItemView::SelectRows);
        tb_sim_record_result->setSortingEnabled(true);
        tb_sim_record_result->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        tb_sim_record_result->setAlternatingRowColors(true);
        tb_sim_record_result->horizontalHeader()->setStretchLastSection(true);

        verticalLayout_5->addWidget(tb_sim_record_result);

        stackedWidget->addWidget(page_sim);
        page_iot = new QWidget();
        page_iot->setObjectName(QStringLiteral("page_iot"));
        page_iot->setStyleSheet(QString::fromUtf8("\n"
"#wnd_iot_filters QLabel { font: 12pt \"\345\276\256\350\275\257\351\233\205\351\273\221\"; color: #333333; }\n"
"#wnd_iot_filters QLineEdit { border: 1px solid #CCCCCC; border-radius: 4px; padding: 5px; font: 12pt \"\345\276\256\350\275\257\351\233\205\351\273\221\"; background: #FFFFFF; }\n"
"#wnd_iot_filters QComboBox { border: 1px solid #CCCCCC; border-radius: 4px; padding: 5px; font: 12pt \"\345\276\256\350\275\257\351\233\205\351\273\221\"; background: #FFFFFF; }\n"
"#wnd_iot_filters QDateEdit { border: 1px solid #CCCCCC; border-radius: 4px; padding: 5px; font: 12pt \"\345\276\256\350\275\257\351\233\205\351\273\221\"; background: #FFFFFF; }\n"
"#btn_iot_record_query { background-color: #005FA7; color: #FFFFFF; border-radius: 4px; font: 12pt \"\345\276\256\350\275\257\351\233\205\351\273\221\"; border: none; }\n"
"#btn_iot_record_reset { background-color: #FFFFFF; color: #005FA7; border: 1px solid #005FA7; border-radius: 4px; font: 12pt \"\345\276\256\350\275\257\351\233\205\351\273\221\"; }\n"
"#btn"
                        "_iot_record_export_excel { background-color: #005FA7; color: #FFFFFF; border-radius: 4px; font: 12pt \"\345\276\256\350\275\257\351\233\205\351\273\221\"; border: none; }\n"
"#btn_iot_record_delete { background-color: #FF4D4F; color: #FFFFFF; border-radius: 4px; font: 12pt \"\345\276\256\350\275\257\351\233\205\351\273\221\"; border: none; }\n"
"             "));
        verticalLayout_7 = new QVBoxLayout(page_iot);
        verticalLayout_7->setSpacing(15);
        verticalLayout_7->setObjectName(QStringLiteral("verticalLayout_7"));
        verticalLayout_7->setContentsMargins(20, 20, 20, 20);
        wnd_iot_filters = new QWidget(page_iot);
        wnd_iot_filters->setObjectName(QStringLiteral("wnd_iot_filters"));
        gridLayout_iot_filters = new QGridLayout(wnd_iot_filters);
        gridLayout_iot_filters->setSpacing(15);
        gridLayout_iot_filters->setContentsMargins(0, 0, 0, 0);
        gridLayout_iot_filters->setObjectName(QStringLiteral("gridLayout_iot_filters"));
        lb_iot_record_version = new QLabel(wnd_iot_filters);
        lb_iot_record_version->setObjectName(QStringLiteral("lb_iot_record_version"));

        gridLayout_iot_filters->addWidget(lb_iot_record_version, 0, 0, 1, 1);

        edt_iot_record_version = new QLineEdit(wnd_iot_filters);
        edt_iot_record_version->setObjectName(QStringLiteral("edt_iot_record_version"));
        edt_iot_record_version->setMinimumSize(QSize(200, 40));

        gridLayout_iot_filters->addWidget(edt_iot_record_version, 1, 0, 1, 1);

        lb_iot_record_imei = new QLabel(wnd_iot_filters);
        lb_iot_record_imei->setObjectName(QStringLiteral("lb_iot_record_imei"));

        gridLayout_iot_filters->addWidget(lb_iot_record_imei, 0, 1, 1, 1);

        edt_iot_record_imei = new QLineEdit(wnd_iot_filters);
        edt_iot_record_imei->setObjectName(QStringLiteral("edt_iot_record_imei"));
        edt_iot_record_imei->setMinimumSize(QSize(200, 40));

        gridLayout_iot_filters->addWidget(edt_iot_record_imei, 1, 1, 1, 1);

        lb_iot_record_test_time = new QLabel(wnd_iot_filters);
        lb_iot_record_test_time->setObjectName(QStringLiteral("lb_iot_record_test_time"));

        gridLayout_iot_filters->addWidget(lb_iot_record_test_time, 0, 2, 1, 3);

        edt_iot_record_begin_date = new QDateEdit(wnd_iot_filters);
        edt_iot_record_begin_date->setObjectName(QStringLiteral("edt_iot_record_begin_date"));
        edt_iot_record_begin_date->setCalendarPopup(true);
        edt_iot_record_begin_date->setMinimumSize(QSize(180, 40));

        gridLayout_iot_filters->addWidget(edt_iot_record_begin_date, 1, 2, 1, 1);

        lb_iot_to = new QLabel(wnd_iot_filters);
        lb_iot_to->setObjectName(QStringLiteral("lb_iot_to"));
        lb_iot_to->setAlignment(Qt::AlignCenter);
        lb_iot_to->setMinimumSize(QSize(20, 40));

        gridLayout_iot_filters->addWidget(lb_iot_to, 1, 3, 1, 1);

        edt_iot_record_end_date = new QDateEdit(wnd_iot_filters);
        edt_iot_record_end_date->setObjectName(QStringLiteral("edt_iot_record_end_date"));
        edt_iot_record_end_date->setCalendarPopup(true);
        edt_iot_record_end_date->setMinimumSize(QSize(180, 40));

        gridLayout_iot_filters->addWidget(edt_iot_record_end_date, 1, 4, 1, 1);

        btn_iot_record_query = new QPushButton(wnd_iot_filters);
        btn_iot_record_query->setObjectName(QStringLiteral("btn_iot_record_query"));
        btn_iot_record_query->setMinimumSize(QSize(80, 40));

        gridLayout_iot_filters->addWidget(btn_iot_record_query, 1, 5, 1, 1);

        btn_iot_record_reset = new QPushButton(wnd_iot_filters);
        btn_iot_record_reset->setObjectName(QStringLiteral("btn_iot_record_reset"));
        btn_iot_record_reset->setMinimumSize(QSize(80, 40));

        gridLayout_iot_filters->addWidget(btn_iot_record_reset, 1, 6, 1, 1);

        spacer_iot_filters = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_iot_filters->addItem(spacer_iot_filters, 1, 7, 1, 1);


        verticalLayout_7->addWidget(wnd_iot_filters);

        wnd_iot_actions = new QWidget(page_iot);
        wnd_iot_actions->setObjectName(QStringLiteral("wnd_iot_actions"));
        hbox_iot_actions = new QHBoxLayout(wnd_iot_actions);
        hbox_iot_actions->setSpacing(10);
        hbox_iot_actions->setContentsMargins(0, 0, 0, 0);
        hbox_iot_actions->setObjectName(QStringLiteral("hbox_iot_actions"));
        btn_iot_record_export_excel = new QPushButton(wnd_iot_actions);
        btn_iot_record_export_excel->setObjectName(QStringLiteral("btn_iot_record_export_excel"));
        btn_iot_record_export_excel->setMinimumSize(QSize(80, 40));

        hbox_iot_actions->addWidget(btn_iot_record_export_excel);

        btn_iot_record_delete = new QPushButton(wnd_iot_actions);
        btn_iot_record_delete->setObjectName(QStringLiteral("btn_iot_record_delete"));
        btn_iot_record_delete->setMinimumSize(QSize(80, 40));

        hbox_iot_actions->addWidget(btn_iot_record_delete);

        spacer_iot_actions = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);

        hbox_iot_actions->addItem(spacer_iot_actions);


        verticalLayout_7->addWidget(wnd_iot_actions);

        tb_iot_record_result = new QTableView(page_iot);
        tb_iot_record_result->setObjectName(QStringLiteral("tb_iot_record_result"));
        tb_iot_record_result->setSelectionBehavior(QAbstractItemView::SelectRows);
        tb_iot_record_result->setSortingEnabled(true);
        tb_iot_record_result->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        tb_iot_record_result->setAlternatingRowColors(true);
        tb_iot_record_result->horizontalHeader()->setStretchLastSection(true);

        verticalLayout_7->addWidget(tb_iot_record_result);

        stackedWidget->addWidget(page_iot);

        verticalLayout_4->addWidget(stackedWidget);

        stackedWidget_bk->addWidget(page_record);
        page_advance = new QWidget();
        page_advance->setObjectName(QStringLiteral("page_advance"));
        wnd_advance = new QWidget(page_advance);
        wnd_advance->setObjectName(QStringLiteral("wnd_advance"));
        wnd_advance->setGeometry(QRect(0, 0, 1280, 668));
        wnd_advance->setStyleSheet(QString::fromUtf8("#lb_serial_info\n"
",#lb_lang_info\n"
"{\n"
"	color: #005FA7;\n"
"	font: 20pt \"\345\276\256\350\275\257\351\233\205\351\273\221\";\n"
"	background:transparent;\n"
"}\n"
"\n"
"#wnd_advance\n"
"{\n"
"	background: transparent;\n"
"}\n"
"\n"
"#wnd_serial_config\n"
",#wnd_serial_config_container\n"
"{\n"
"	background: transparent;\n"
"}\n"
"\n"
"#wnd_lang_select\n"
",#wnd_select_lang_container\n"
"{\n"
"	background: transparent;\n"
"}\n"
"\n"
"#wnd_advance_bottom_btn\n"
"{\n"
"	background: transparent;\n"
"}"));
        verticalLayout_3 = new QVBoxLayout(wnd_advance);
        verticalLayout_3->setSpacing(20);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        verticalLayout_3->setContentsMargins(10, 20, 10, 10);
        wnd_serial_config = new QWidget(wnd_advance);
        wnd_serial_config->setObjectName(QStringLiteral("wnd_serial_config"));
        wnd_serial_config->setMaximumSize(QSize(16777215, 100));
        verticalLayout_26 = new QVBoxLayout(wnd_serial_config);
        verticalLayout_26->setSpacing(10);
        verticalLayout_26->setObjectName(QStringLiteral("verticalLayout_26"));
        verticalLayout_26->setContentsMargins(0, 0, 0, 0);
        lb_serial_info = new QLabel(wnd_serial_config);
        lb_serial_info->setObjectName(QStringLiteral("lb_serial_info"));
        lb_serial_info->setMinimumSize(QSize(0, 40));
        lb_serial_info->setMaximumSize(QSize(16777215, 40));

        verticalLayout_26->addWidget(lb_serial_info);

        wnd_serial_config_container = new QWidget(wnd_serial_config);
        wnd_serial_config_container->setObjectName(QStringLiteral("wnd_serial_config_container"));
        horizontalLayout_57 = new QHBoxLayout(wnd_serial_config_container);
        horizontalLayout_57->setSpacing(10);
        horizontalLayout_57->setObjectName(QStringLiteral("horizontalLayout_57"));
        horizontalLayout_57->setContentsMargins(0, 0, 0, 0);
        lb_scaner_serialport = new QLabel(wnd_serial_config_container);
        lb_scaner_serialport->setObjectName(QStringLiteral("lb_scaner_serialport"));
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(lb_scaner_serialport->sizePolicy().hasHeightForWidth());
        lb_scaner_serialport->setSizePolicy(sizePolicy);
        lb_scaner_serialport->setMinimumSize(QSize(130, 40));
        lb_scaner_serialport->setMaximumSize(QSize(130, 40));
        lb_scaner_serialport->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout_57->addWidget(lb_scaner_serialport);

        cb_serialport = new QComboBox(wnd_serial_config_container);
        cb_serialport->setObjectName(QStringLiteral("cb_serialport"));
        cb_serialport->setMinimumSize(QSize(160, 40));
        cb_serialport->setMaximumSize(QSize(160, 40));

        horizontalLayout_57->addWidget(cb_serialport);

        lb_scaner_baudrate = new QLabel(wnd_serial_config_container);
        lb_scaner_baudrate->setObjectName(QStringLiteral("lb_scaner_baudrate"));
        sizePolicy.setHeightForWidth(lb_scaner_baudrate->sizePolicy().hasHeightForWidth());
        lb_scaner_baudrate->setSizePolicy(sizePolicy);
        lb_scaner_baudrate->setMinimumSize(QSize(130, 40));
        lb_scaner_baudrate->setMaximumSize(QSize(130, 40));
        lb_scaner_baudrate->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout_57->addWidget(lb_scaner_baudrate);

        cb_baudrate = new QComboBox(wnd_serial_config_container);
        cb_baudrate->setObjectName(QStringLiteral("cb_baudrate"));
        cb_baudrate->setMinimumSize(QSize(160, 40));
        cb_baudrate->setMaximumSize(QSize(160, 40));

        horizontalLayout_57->addWidget(cb_baudrate);

        horizontalSpacer_35 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_57->addItem(horizontalSpacer_35);


        verticalLayout_26->addWidget(wnd_serial_config_container);


        verticalLayout_3->addWidget(wnd_serial_config);

        wnd_lang_select = new QWidget(wnd_advance);
        wnd_lang_select->setObjectName(QStringLiteral("wnd_lang_select"));
        wnd_lang_select->setMaximumSize(QSize(16777215, 100));
        verticalLayout_27 = new QVBoxLayout(wnd_lang_select);
        verticalLayout_27->setSpacing(10);
        verticalLayout_27->setObjectName(QStringLiteral("verticalLayout_27"));
        verticalLayout_27->setContentsMargins(0, 0, 0, 0);
        lb_lang_info = new QLabel(wnd_lang_select);
        lb_lang_info->setObjectName(QStringLiteral("lb_lang_info"));
        lb_lang_info->setMinimumSize(QSize(0, 40));
        lb_lang_info->setMaximumSize(QSize(16777215, 40));

        verticalLayout_27->addWidget(lb_lang_info);

        wnd_select_lang_container = new QWidget(wnd_lang_select);
        wnd_select_lang_container->setObjectName(QStringLiteral("wnd_select_lang_container"));
        horizontalLayout_58 = new QHBoxLayout(wnd_select_lang_container);
        horizontalLayout_58->setSpacing(50);
        horizontalLayout_58->setObjectName(QStringLiteral("horizontalLayout_58"));
        horizontalLayout_58->setContentsMargins(20, 0, 0, 0);
        btn_lang_cn = new QPushButton(wnd_select_lang_container);
        btn_lang_cn->setObjectName(QStringLiteral("btn_lang_cn"));
        btn_lang_cn->setMinimumSize(QSize(150, 40));
        btn_lang_cn->setMaximumSize(QSize(150, 40));

        horizontalLayout_58->addWidget(btn_lang_cn);

        btn_lang_en = new QPushButton(wnd_select_lang_container);
        btn_lang_en->setObjectName(QStringLiteral("btn_lang_en"));
        btn_lang_en->setMinimumSize(QSize(150, 40));
        btn_lang_en->setMaximumSize(QSize(150, 40));

        horizontalLayout_58->addWidget(btn_lang_en);

        horizontalSpacer_36 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_58->addItem(horizontalSpacer_36);


        verticalLayout_27->addWidget(wnd_select_lang_container);


        verticalLayout_3->addWidget(wnd_lang_select);

        verticalSpacer_4 = new QSpacerItem(0, 325, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_3->addItem(verticalSpacer_4);

        wnd_advance_bottom_btn = new QWidget(wnd_advance);
        wnd_advance_bottom_btn->setObjectName(QStringLiteral("wnd_advance_bottom_btn"));
        horizontalLayout_30 = new QHBoxLayout(wnd_advance_bottom_btn);
        horizontalLayout_30->setObjectName(QStringLiteral("horizontalLayout_30"));
        horizontalLayout_30->setContentsMargins(0, 0, 0, 0);
        horizontalSpacer_27 = new QSpacerItem(863, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_30->addItem(horizontalSpacer_27);

        btn_save_advance = new QPushButton(wnd_advance_bottom_btn);
        btn_save_advance->setObjectName(QStringLiteral("btn_save_advance"));
        btn_save_advance->setMinimumSize(QSize(90, 40));
        btn_save_advance->setMaximumSize(QSize(90, 40));

        horizontalLayout_30->addWidget(btn_save_advance);


        verticalLayout_3->addWidget(wnd_advance_bottom_btn);

        stackedWidget_bk->addWidget(page_advance);

        retranslateUi(BackendWnd);

        QMetaObject::connectSlotsByName(BackendWnd);
    } // setupUi

    void retranslateUi(QWidget *BackendWnd)
    {
        BackendWnd->setWindowTitle(QApplication::translate("BackendWnd", "Form", nullptr));
        btn_page_record->setText(QApplication::translate("BackendWnd", "Record", nullptr));
        btn_page_advance->setText(QApplication::translate("BackendWnd", "Setting", nullptr));
        btn_record_back->setText(QApplication::translate("BackendWnd", "<", nullptr));
        lb_record_title->setText(QApplication::translate("BackendWnd", "Test Record", nullptr));
        btn_navi_sim->setText(QApplication::translate("BackendWnd", "Sim Card Test", nullptr));
        btn_navi_iot->setText(QApplication::translate("BackendWnd", "IOT Test", nullptr));
        lb_sim_record_iccid->setText(QApplication::translate("BackendWnd", "Sim ICCID:", nullptr));
        edt_sim_record_iccid->setPlaceholderText(QApplication::translate("BackendWnd", "Please enter", nullptr));
        lb_sim_record_net_status->setText(QApplication::translate("BackendWnd", "Sim Network Status:", nullptr));
        lb_sim_record_net_type->setText(QApplication::translate("BackendWnd", "Network Type:", nullptr));
        lb_sim_record_test_time->setText(QApplication::translate("BackendWnd", "Test Time:", nullptr));
        edt_sim_record_begin_date->setDisplayFormat(QApplication::translate("BackendWnd", "yyyy-MM-dd", nullptr));
        lb_sim_to->setText(QApplication::translate("BackendWnd", "-", nullptr));
        edt_sim_record_end_date->setDisplayFormat(QApplication::translate("BackendWnd", "yyyy-MM-dd", nullptr));
        btn_sim_record_query->setText(QApplication::translate("BackendWnd", "Search", nullptr));
        btn_sim_record_reset->setText(QApplication::translate("BackendWnd", "Reset", nullptr));
        btn_sim_record_export_excel->setText(QApplication::translate("BackendWnd", "Export", nullptr));
        btn_sim_record_delete->setText(QApplication::translate("BackendWnd", "Delete", nullptr));
        lb_iot_record_version->setText(QApplication::translate("BackendWnd", "IOT Version:", nullptr));
        edt_iot_record_version->setPlaceholderText(QApplication::translate("BackendWnd", "Please enter", nullptr));
        lb_iot_record_imei->setText(QApplication::translate("BackendWnd", "IOT IMEI:", nullptr));
        edt_iot_record_imei->setPlaceholderText(QApplication::translate("BackendWnd", "Please enter", nullptr));
        lb_iot_record_test_time->setText(QApplication::translate("BackendWnd", "Test Time:", nullptr));
        edt_iot_record_begin_date->setDisplayFormat(QApplication::translate("BackendWnd", "yyyy-MM-dd", nullptr));
        lb_iot_to->setText(QApplication::translate("BackendWnd", "-", nullptr));
        edt_iot_record_end_date->setDisplayFormat(QApplication::translate("BackendWnd", "yyyy-MM-dd", nullptr));
        btn_iot_record_query->setText(QApplication::translate("BackendWnd", "Search", nullptr));
        btn_iot_record_reset->setText(QApplication::translate("BackendWnd", "Reset", nullptr));
        btn_iot_record_export_excel->setText(QApplication::translate("BackendWnd", "Export", nullptr));
        btn_iot_record_delete->setText(QApplication::translate("BackendWnd", "Delete", nullptr));
        lb_serial_info->setText(QApplication::translate("BackendWnd", "Serial Connect Config", nullptr));
        lb_scaner_serialport->setText(QApplication::translate("BackendWnd", "Serial Port:", nullptr));
        lb_scaner_baudrate->setText(QApplication::translate("BackendWnd", "Baudrate:", nullptr));
        lb_lang_info->setText(QApplication::translate("BackendWnd", "Select Language", nullptr));
        btn_lang_cn->setText(QApplication::translate("BackendWnd", "\344\270\255\346\226\207", nullptr));
        btn_lang_en->setText(QApplication::translate("BackendWnd", "English", nullptr));
        btn_save_advance->setText(QApplication::translate("BackendWnd", "Save", nullptr));
    } // retranslateUi

};

namespace Ui {
    class BackendWnd: public Ui_BackendWnd {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_BACKENDWND_H
