/********************************************************************************
** Form generated from reading UI file 'MainWnd.ui'
**
** Created by: Qt User Interface Compiler version 5.10.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWND_H
#define UI_MAINWND_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWnd
{
public:
    QWidget *wnd_main_bk;
    QStackedWidget *sw_main;
    QWidget *page_main;
    QHBoxLayout *main_layout;
    QWidget *wnd_left_panel;
    QVBoxLayout *verticalLayout_Left;
    QLabel *lb_module_type;
    QComboBox *cb_module_type;
    QLabel *lb_apn_info;
    QComboBox *le_apn;
    QLabel *lb_net_info;
    QComboBox *le_net;
    QSpacerItem *verticalSpacer_Buttons;
    QPushButton *btn_nor_version;
    QPushButton *btn_nor_all_test;
    QPushButton *btn_nor_record;
    QSpacerItem *verticalSpacer_Bottom;
    QPushButton *btn_nor_clear;
    QWidget *wnd_right_panel;
    QVBoxLayout *verticalLayout_Right;
    QGridLayout *gridLayout_Info;
    QLabel *lb_version_info;
    QLineEdit *lb_test_version;
    QLabel *lb_network_info;
    QLabel *lb_sim_network_info_label;
    QLineEdit *lb_test_network_type;
    QLineEdit *lb_test_sim_network;
    QLabel *lb_iccid_info;
    QLabel *lb_rssi_info;
    QLineEdit *lb_test_iccid;
    QLineEdit *lb_test_rssi;
    QLabel *lb_iot_module_info;
    QLabel *lb_iot_imei_info;
    QLineEdit *lb_test_iot_module_ver;
    QLineEdit *lb_test_iot_imei;
    QLabel *lb_process_info;
    QPlainTextEdit *lb_test_cmd_excute_return_msg;

    void setupUi(QWidget *MainWnd)
    {
        if (MainWnd->objectName().isEmpty())
            MainWnd->setObjectName(QStringLiteral("MainWnd"));
        MainWnd->resize(1280, 710);
        MainWnd->setStyleSheet(QLatin1String("#MainWnd\n"
",#wnd_main_bk\n"
"{\n"
"	background:transparent;\n"
"	background-color: #ffffff;\n"
"}\n"
"\n"
"QLabel\n"
"{\n"
"	background:transparent;\n"
"}\n"
"\n"
"#wnd_main_bk\n"
"{\n"
"	background-color: #E9F2F8;\n"
"}\n"
"\n"
"#sw_main\n"
",#page_main\n"
"{\n"
"	background:transparent;\n"
"}"));
        wnd_main_bk = new QWidget(MainWnd);
        wnd_main_bk->setObjectName(QStringLiteral("wnd_main_bk"));
        wnd_main_bk->setGeometry(QRect(0, 0, 1280, 710));
        sw_main = new QStackedWidget(wnd_main_bk);
        sw_main->setObjectName(QStringLiteral("sw_main"));
        sw_main->setGeometry(QRect(0, 0, 1280, 710));
        sw_main->setStyleSheet(QString::fromUtf8("\n"
"QPushButton\n"
"{\n"
"	border: none;\n"
"	border-radius:6px;\n"
"	font: 20pt \"\345\276\256\350\275\257\351\233\205\351\273\221\";\n"
"	color:#FFFFFF;\n"
"}\n"
"\n"
"/* \346\251\231\350\211\262\346\214\211\351\222\256 - \351\225\234\345\203\217\347\211\210\346\234\254 */\n"
"QPushButton[objectName=\"btn_nor_version\"]\n"
"{\n"
"	background-color: #FF8C00;\n"
"}\n"
"QPushButton[objectName=\"btn_nor_version\"]:pressed\n"
"{\n"
"	background-color: #E67E00;\n"
"}\n"
"\n"
"/* \350\223\235\350\211\262\346\214\211\351\222\256 - \344\270\200\351\224\256\346\265\213\350\257\225 */\n"
"QPushButton[objectName=\"btn_nor_all_test\"]\n"
"{\n"
"	background-color: #005FA7;\n"
"}\n"
"QPushButton[objectName=\"btn_nor_all_test\"]:pressed\n"
"{\n"
"	background-color:#74a8cf;\n"
"}\n"
"\n"
"/* \347\273\277\350\211\262\346\214\211\351\222\256 - \346\265\213\350\257\225\350\256\260\345\275\225 */\n"
"QPushButton[objectName=\"btn_nor_record\"]\n"
"{\n"
"	background-color: #00C853;\n"
"}\n"
"QPushButton[objectName=\"btn_nor_recor"
                        "d\"]:pressed\n"
"{\n"
"	background-color: #00A042;\n"
"}\n"
"\n"
"/* \347\231\275\350\211\262\346\214\211\351\222\256 - \346\270\205\347\251\272\346\265\213\350\257\225\347\273\223\346\236\234 */\n"
"QPushButton[objectName=\"btn_nor_clear\"]\n"
"{\n"
"	background-color: #FFFFFF;\n"
"	color: #FF0000;\n"
"	border: 2px solid #FF0000;\n"
"}\n"
"QPushButton[objectName=\"btn_nor_clear\"]:pressed\n"
"{\n"
"	background-color: #FFF0F0;\n"
"}\n"
"\n"
"QPushButton:disabled\n"
"{\n"
"	background-color:#999999;\n"
"	color: #DDDDDD;\n"
"	border: none;\n"
"}\n"
""));
        page_main = new QWidget();
        page_main->setObjectName(QStringLiteral("page_main"));
        page_main->setStyleSheet(QString::fromUtf8("#wnd_left_panel {\n"
"    background-color: #ffffff;\n"
"    border-radius: 10px;\n"
"}\n"
"#wnd_right_panel {\n"
"    background-color: #ffffff;\n"
"    border-radius: 10px;\n"
"}\n"
"\n"
"#lb_module_type, #lb_apn_info, #lb_net_info {\n"
"    font: 75 12pt \"\345\276\256\350\275\257\351\233\205\351\273\221\";\n"
"    color: #333333;\n"
"}\n"
"\n"
"/* \350\276\223\345\205\245\346\241\206\346\240\267\345\274\217 */\n"
"QComboBox, QLineEdit {\n"
"    border: 1px solid #CCCCCC;\n"
"    border-radius: 4px;\n"
"    padding: 2px;\n"
"    background: #FFFFFF;\n"
"    font: 12pt \"\345\276\256\350\275\257\351\233\205\351\273\221\";\n"
"    color: #333333;\n"
"    min-height: 35px;\n"
"}\n"
"QComboBox::drop-down {\n"
"    subcontrol-origin: padding;\n"
"    subcontrol-position: top right;\n"
"    width: 30px;\n"
"    border-left-width: 0px;\n"
"    border-left-color: transparent;\n"
"    border-left-style: none;\n"
"    border-top-right-radius: 3px;\n"
"    border-bottom-right-radius: 3px;\n"
"    background: transpare"
                        "nt;\n"
"}\n"
"QComboBox::down-arrow {\n"
"    image: url(:/img/backend/icon-up.png);\n"
"    width: 20px;\n"
"    height: 13px;\n"
"}\n"
"\n"
"/* \345\217\263\344\276\247\344\277\241\346\201\257\346\240\207\347\255\276\346\240\267\345\274\217 */\n"
"QLabel[objectName^=\"lb_\"] {\n"
"    font: 12pt \"\345\276\256\350\275\257\351\233\205\351\273\221\";\n"
"    color: #666666;\n"
"}\n"
"\n"
"QLineEdit[objectName^=\"lb_test_\"] {\n"
"     background-color: #F8F8F8;\n"
"     border: 1px solid #D5D5D5;\n"
"     border-radius: 6px;\n"
"     padding: 2px 5px;\n"
"     color: #333333;\n"
"     font: 12pt \"\345\276\256\350\275\257\351\233\205\351\273\221\";\n"
"}"));
        main_layout = new QHBoxLayout(page_main);
        main_layout->setSpacing(20);
        main_layout->setObjectName(QStringLiteral("main_layout"));
        main_layout->setContentsMargins(20, 20, 20, 10);
        wnd_left_panel = new QWidget(page_main);
        wnd_left_panel->setObjectName(QStringLiteral("wnd_left_panel"));
        wnd_left_panel->setMinimumSize(QSize(280, 0));
        wnd_left_panel->setMaximumSize(QSize(280, 16777215));
        verticalLayout_Left = new QVBoxLayout(wnd_left_panel);
        verticalLayout_Left->setSpacing(10);
        verticalLayout_Left->setObjectName(QStringLiteral("verticalLayout_Left"));
        verticalLayout_Left->setContentsMargins(20, 20, 20, 20);
        lb_module_type = new QLabel(wnd_left_panel);
        lb_module_type->setObjectName(QStringLiteral("lb_module_type"));

        verticalLayout_Left->addWidget(lb_module_type);

        cb_module_type = new QComboBox(wnd_left_panel);
        cb_module_type->setObjectName(QStringLiteral("cb_module_type"));
        cb_module_type->setMinimumSize(QSize(0, 40));

        verticalLayout_Left->addWidget(cb_module_type);

        lb_apn_info = new QLabel(wnd_left_panel);
        lb_apn_info->setObjectName(QStringLiteral("lb_apn_info"));

        verticalLayout_Left->addWidget(lb_apn_info);

        le_apn = new QComboBox(wnd_left_panel);
        le_apn->setObjectName(QStringLiteral("le_apn"));
        le_apn->setEditable(true);
        le_apn->setMinimumSize(QSize(0, 40));

        verticalLayout_Left->addWidget(le_apn);

        lb_net_info = new QLabel(wnd_left_panel);
        lb_net_info->setObjectName(QStringLiteral("lb_net_info"));

        verticalLayout_Left->addWidget(lb_net_info);

        le_net = new QComboBox(wnd_left_panel);
        le_net->setObjectName(QStringLiteral("le_net"));
        le_net->setEditable(true);
        le_net->setMinimumSize(QSize(0, 40));

        verticalLayout_Left->addWidget(le_net);

        verticalSpacer_Buttons = new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Fixed);

        verticalLayout_Left->addItem(verticalSpacer_Buttons);

        btn_nor_version = new QPushButton(wnd_left_panel);
        btn_nor_version->setObjectName(QStringLiteral("btn_nor_version"));
        btn_nor_version->setMinimumSize(QSize(0, 50));

        verticalLayout_Left->addWidget(btn_nor_version);

        btn_nor_all_test = new QPushButton(wnd_left_panel);
        btn_nor_all_test->setObjectName(QStringLiteral("btn_nor_all_test"));
        btn_nor_all_test->setMinimumSize(QSize(0, 50));

        verticalLayout_Left->addWidget(btn_nor_all_test);

        btn_nor_record = new QPushButton(wnd_left_panel);
        btn_nor_record->setObjectName(QStringLiteral("btn_nor_record"));
        btn_nor_record->setMinimumSize(QSize(0, 50));

        verticalLayout_Left->addWidget(btn_nor_record);

        verticalSpacer_Bottom = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_Left->addItem(verticalSpacer_Bottom);

        btn_nor_clear = new QPushButton(wnd_left_panel);
        btn_nor_clear->setObjectName(QStringLiteral("btn_nor_clear"));
        btn_nor_clear->setMinimumSize(QSize(0, 50));

        verticalLayout_Left->addWidget(btn_nor_clear);


        main_layout->addWidget(wnd_left_panel);

        wnd_right_panel = new QWidget(page_main);
        wnd_right_panel->setObjectName(QStringLiteral("wnd_right_panel"));
        verticalLayout_Right = new QVBoxLayout(wnd_right_panel);
        verticalLayout_Right->setSpacing(10);
        verticalLayout_Right->setObjectName(QStringLiteral("verticalLayout_Right"));
        verticalLayout_Right->setContentsMargins(20, 20, 20, 20);
        gridLayout_Info = new QGridLayout();
        gridLayout_Info->setObjectName(QStringLiteral("gridLayout_Info"));
        gridLayout_Info->setVerticalSpacing(10);
        gridLayout_Info->setHorizontalSpacing(20);
        lb_version_info = new QLabel(wnd_right_panel);
        lb_version_info->setObjectName(QStringLiteral("lb_version_info"));

        gridLayout_Info->addWidget(lb_version_info, 0, 0, 1, 1);

        lb_test_version = new QLineEdit(wnd_right_panel);
        lb_test_version->setObjectName(QStringLiteral("lb_test_version"));
        lb_test_version->setReadOnly(true);
        lb_test_version->setMinimumSize(QSize(0, 35));

        gridLayout_Info->addWidget(lb_test_version, 1, 0, 1, 1);

        lb_network_info = new QLabel(wnd_right_panel);
        lb_network_info->setObjectName(QStringLiteral("lb_network_info"));

        gridLayout_Info->addWidget(lb_network_info, 2, 0, 1, 1);

        lb_sim_network_info_label = new QLabel(wnd_right_panel);
        lb_sim_network_info_label->setObjectName(QStringLiteral("lb_sim_network_info_label"));

        gridLayout_Info->addWidget(lb_sim_network_info_label, 2, 1, 1, 1);

        lb_test_network_type = new QLineEdit(wnd_right_panel);
        lb_test_network_type->setObjectName(QStringLiteral("lb_test_network_type"));
        lb_test_network_type->setReadOnly(true);
        lb_test_network_type->setMinimumSize(QSize(0, 35));

        gridLayout_Info->addWidget(lb_test_network_type, 3, 0, 1, 1);

        lb_test_sim_network = new QLineEdit(wnd_right_panel);
        lb_test_sim_network->setObjectName(QStringLiteral("lb_test_sim_network"));
        lb_test_sim_network->setReadOnly(true);
        lb_test_sim_network->setMinimumSize(QSize(0, 35));

        gridLayout_Info->addWidget(lb_test_sim_network, 3, 1, 1, 1);

        lb_iccid_info = new QLabel(wnd_right_panel);
        lb_iccid_info->setObjectName(QStringLiteral("lb_iccid_info"));

        gridLayout_Info->addWidget(lb_iccid_info, 4, 0, 1, 1);

        lb_rssi_info = new QLabel(wnd_right_panel);
        lb_rssi_info->setObjectName(QStringLiteral("lb_rssi_info"));

        gridLayout_Info->addWidget(lb_rssi_info, 4, 1, 1, 1);

        lb_test_iccid = new QLineEdit(wnd_right_panel);
        lb_test_iccid->setObjectName(QStringLiteral("lb_test_iccid"));
        lb_test_iccid->setReadOnly(true);
        lb_test_iccid->setMinimumSize(QSize(0, 35));

        gridLayout_Info->addWidget(lb_test_iccid, 5, 0, 1, 1);

        lb_test_rssi = new QLineEdit(wnd_right_panel);
        lb_test_rssi->setObjectName(QStringLiteral("lb_test_rssi"));
        lb_test_rssi->setReadOnly(true);
        lb_test_rssi->setMinimumSize(QSize(0, 35));

        gridLayout_Info->addWidget(lb_test_rssi, 5, 1, 1, 1);

        lb_iot_module_info = new QLabel(wnd_right_panel);
        lb_iot_module_info->setObjectName(QStringLiteral("lb_iot_module_info"));

        gridLayout_Info->addWidget(lb_iot_module_info, 6, 0, 1, 1);

        lb_iot_imei_info = new QLabel(wnd_right_panel);
        lb_iot_imei_info->setObjectName(QStringLiteral("lb_iot_imei_info"));

        gridLayout_Info->addWidget(lb_iot_imei_info, 6, 1, 1, 1);

        lb_test_iot_module_ver = new QLineEdit(wnd_right_panel);
        lb_test_iot_module_ver->setObjectName(QStringLiteral("lb_test_iot_module_ver"));
        lb_test_iot_module_ver->setReadOnly(true);
        lb_test_iot_module_ver->setMinimumSize(QSize(0, 35));

        gridLayout_Info->addWidget(lb_test_iot_module_ver, 7, 0, 1, 1);

        lb_test_iot_imei = new QLineEdit(wnd_right_panel);
        lb_test_iot_imei->setObjectName(QStringLiteral("lb_test_iot_imei"));
        lb_test_iot_imei->setReadOnly(true);
        lb_test_iot_imei->setMinimumSize(QSize(0, 35));

        gridLayout_Info->addWidget(lb_test_iot_imei, 7, 1, 1, 1);


        verticalLayout_Right->addLayout(gridLayout_Info);

        lb_process_info = new QLabel(wnd_right_panel);
        lb_process_info->setObjectName(QStringLiteral("lb_process_info"));

        verticalLayout_Right->addWidget(lb_process_info);

        lb_test_cmd_excute_return_msg = new QPlainTextEdit(wnd_right_panel);
        lb_test_cmd_excute_return_msg->setObjectName(QStringLiteral("lb_test_cmd_excute_return_msg"));
        lb_test_cmd_excute_return_msg->setReadOnly(true);

        verticalLayout_Right->addWidget(lb_test_cmd_excute_return_msg);


        main_layout->addWidget(wnd_right_panel);

        sw_main->addWidget(page_main);

        retranslateUi(MainWnd);

        QMetaObject::connectSlotsByName(MainWnd);
    } // setupUi

    void retranslateUi(QWidget *MainWnd)
    {
        MainWnd->setWindowTitle(QApplication::translate("MainWnd", "Form", nullptr));
        lb_module_type->setText(QApplication::translate("MainWnd", "Module Type:", nullptr));
        lb_apn_info->setText(QApplication::translate("MainWnd", "APN:", nullptr));
        le_apn->setCurrentText(QString());
        lb_net_info->setText(QApplication::translate("MainWnd", "NET:", nullptr));
        le_net->setCurrentText(QString());
        btn_nor_version->setText(QApplication::translate("MainWnd", "Image Version", nullptr));
        btn_nor_all_test->setText(QApplication::translate("MainWnd", "One-key Test", nullptr));
        btn_nor_record->setText(QApplication::translate("MainWnd", "Test Record", nullptr));
        btn_nor_clear->setText(QApplication::translate("MainWnd", "Clear Test Result", nullptr));
        lb_version_info->setText(QApplication::translate("MainWnd", "Image Version:", nullptr));
        lb_network_info->setText(QApplication::translate("MainWnd", "Network Type:", nullptr));
        lb_sim_network_info_label->setText(QApplication::translate("MainWnd", "Sim Network Status:", nullptr));
        lb_iccid_info->setText(QApplication::translate("MainWnd", "Sim Card ICCID:", nullptr));
        lb_rssi_info->setText(QApplication::translate("MainWnd", "Signal Strength:", nullptr));
        lb_iot_module_info->setText(QApplication::translate("MainWnd", "IOT Version:", nullptr));
        lb_iot_imei_info->setText(QApplication::translate("MainWnd", "IOT IMEI:", nullptr));
        lb_process_info->setText(QApplication::translate("MainWnd", "Test Execution Process:", nullptr));
        lb_process_info->setStyleSheet(QApplication::translate("MainWnd", "font: 75 12pt \"\345\276\256\350\275\257\351\233\205\351\273\221\"; color: #333333; background: transparent; border: none;", nullptr));
        lb_test_cmd_excute_return_msg->setStyleSheet(QApplication::translate("MainWnd", "border: 1px solid #D5D5D5; border-radius: 5px; background: #F8F8F8; color: #333333; font: 10pt \"\345\276\256\350\275\257\351\233\205\351\273\221\";", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWnd: public Ui_MainWnd {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWND_H
