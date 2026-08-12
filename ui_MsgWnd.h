/********************************************************************************
** Form generated from reading UI file 'MsgWnd.ui'
**
** Created by: Qt User Interface Compiler version 5.10.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MSGWND_H
#define UI_MSGWND_H

#include <QtCore/QVariant>
#include <QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MsgWnd
{
public:
    QWidget *wnd_bk;
    QWidget *wnd_container;
    QStackedWidget *wnd_msg_content;
    QWidget *normal;
    QVBoxLayout *verticalLayout_8;
    QWidget *wnd_normal_top;
    QVBoxLayout *verticalLayout_2;
    QLabel *lb_normal_title;
    QWidget *wnd_normal_icon;
    QHBoxLayout *horizontalLayout_7;
    QSpacerItem *horizontalSpacer_16;
    QLabel *lb_normal_icon;
    QSpacerItem *horizontalSpacer_17;
    QWidget *wnd_normal_tip;
    QVBoxLayout *verticalLayout;
    QSpacerItem *verticalSpacer;
    QLabel *lb_normal_tip1;
    QLabel *lb_normal_tip2;
    QLabel *lb_normal_tip3;
    QSpacerItem *verticalSpacer_2;
    QSpacerItem *horizontalSpacer_3;
    QWidget *wnd_normal_btn;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QPushButton *btn_nor_normal_cancel;
    QPushButton *btn_nor_normal_ok;
    QSpacerItem *horizontalSpacer_2;
    QWidget *page_confirm;
    QVBoxLayout *verticalLayout_3;
    QWidget *wnd_confirm_top;
    QHBoxLayout *horizontalLayout_13;
    QSpacerItem *horizontalSpacer_27;
    QLabel *lb_title_confirm;
    QSpacerItem *horizontalSpacer_28;
    QWidget *wnd_confirm_info;
    QVBoxLayout *verticalLayout_12;
    QSpacerItem *verticalSpacer_7;
    QWidget *wnd_confirm_icon;
    QHBoxLayout *horizontalLayout_14;
    QSpacerItem *horizontalSpacer_12;
    QLabel *lb_confirm_icon;
    QSpacerItem *horizontalSpacer_29;
    QLabel *lb_tips_receive_info;
    QWidget *wnd_input_info;
    QHBoxLayout *horizontalLayout_2;
    QSpacerItem *horizontalSpacer_5;
    QLabel *lb_tips_input_info;
    QLineEdit *edt_input_info;
    QSpacerItem *horizontalSpacer_4;
    QSpacerItem *verticalSpacer_8;
    QWidget *wnd_confirm_btn;
    QHBoxLayout *horizontalLayout_5;
    QSpacerItem *horizontalSpacer_13;
    QPushButton *btn_nor_save_input;
    QPushButton *btn_nor_save_receive;
    QSpacerItem *horizontalSpacer_14;
    QPushButton *btn_top_left_back;
    QLabel *lb_fetch_timer;

    void setupUi(QWidget *MsgWnd)
    {
        if (MsgWnd->objectName().isEmpty())
            MsgWnd->setObjectName(QStringLiteral("MsgWnd"));
        MsgWnd->resize(816, 696);
        MsgWnd->setStyleSheet(QLatin1String("#MsgWnd\n"
"{\n"
"	background:transparent;\n"
"}\n"
"\n"
"#wnd_bk\n"
"{\n"
"	background:transparent;\n"
"}\n"
"\n"
"QLabel\n"
"{\n"
"	background:transparent;\n"
"}"));
        wnd_bk = new QWidget(MsgWnd);
        wnd_bk->setObjectName(QStringLiteral("wnd_bk"));
        wnd_bk->setGeometry(QRect(0, 0, 816, 696));
        wnd_container = new QWidget(wnd_bk);
        wnd_container->setObjectName(QStringLiteral("wnd_container"));
        wnd_container->setGeometry(QRect(0, 0, 816, 696));
        wnd_container->setStyleSheet(QString::fromUtf8("#wnd_container\n"
"{\n"
"	background:transparent;\n"
"}\n"
"\n"
"#wnd_container\n"
"{\n"
"	background-image: url(:/img/msg/bg.png);\n"
"}\n"
"\n"
"#wnd_msg_content\n"
",#normal\n"
",#page_confirm\n"
"{\n"
"	background:transparent;\n"
"}\n"
"\n"
"#wnd_normal_btn\n"
",#wnd_normal_tip\n"
",#wnd_normal_top\n"
",#wnd_normal_icon\n"
"{\n"
"	background:transparent;\n"
"}\n"
"\n"
"\n"
"#wnd_confirm_top\n"
",#wnd_confirm_info\n"
",#wnd_confirm_icon\n"
",#wnd_input_info\n"
",#wnd_confirm_btn\n"
"{\n"
"	background:transparent;\n"
"}\n"
"\n"
"#btn_top_left_back\n"
"{\n"
"	background:transparent;\n"
"	font: 75 28pt \"\345\276\256\350\275\257\351\233\205\351\273\221\";\n"
"\n"
"	padding-bottom:5px;\n"
"	color: #7ecef4;\n"
"	border:0px;\n"
"}\n"
"\n"
"#btn_top_left_back:pressed\n"
"{\n"
"	color: #036DB7;\n"
"}\n"
"\n"
"\n"
"#btn_top_left_back\n"
"{\n"
"	font: 75 24pt \"\345\276\256\350\275\257\351\233\205\351\273\221\";\n"
"}\n"
"\n"
"#lb_fetch_timer\n"
"{\n"
"	font: 75 28pt \"\345\276\256\350\275\257\351\233\205\351\273\221"
                        "\";\n"
"	color: #333333;\n"
"}\n"
"\n"
"QPushButton[objectName^=btn_nor_]\n"
"{\n"
"	border: none;\n"
"	border-radius:3px;\n"
"	background-color: #005FA7;\n"
"	\n"
"	font: 24pt \"\345\276\256\350\275\257\351\233\205\351\273\221\";\n"
"	color:#FFFFFF;\n"
"}\n"
"\n"
"QPushButton[objectName^=btn_nor_]:pressed\n"
"{\n"
"	background-color:#74a8cf;\n"
"}\n"
"\n"
"QPushButton[objectName^=btn_nor_]:disabled\n"
"{\n"
"	background-color:#999999;\n"
"}\n"
"\n"
"QLabel[objectName^=lb_title_]\n"
"{\n"
"	background-color: #005FA7;\n"
"	font: 75 28pt \"\345\276\256\350\275\257\351\233\205\351\273\221\";\n"
"	color: #FFFFFF;\n"
"}\n"
"\n"
"QLabel\n"
"{\n"
"	font: 75 24pt \"\345\276\256\350\275\257\351\233\205\351\273\221\";\n"
"	color:#333333;\n"
"}"));
        wnd_msg_content = new QStackedWidget(wnd_container);
        wnd_msg_content->setObjectName(QStringLiteral("wnd_msg_content"));
        wnd_msg_content->setGeometry(QRect(8, 6, 800, 680));
        wnd_msg_content->setMinimumSize(QSize(800, 680));
        wnd_msg_content->setMaximumSize(QSize(800, 680));
        wnd_msg_content->setStyleSheet(QStringLiteral(""));
        normal = new QWidget();
        normal->setObjectName(QStringLiteral("normal"));
        normal->setStyleSheet(QString::fromUtf8("#lb_normal_title\n"
"{\n"
"	font: 75 28pt \"\345\276\256\350\275\257\351\233\205\351\273\221\";\n"
"	color: #333333;\n"
"}\n"
"\n"
""));
        verticalLayout_8 = new QVBoxLayout(normal);
        verticalLayout_8->setSpacing(0);
        verticalLayout_8->setObjectName(QStringLiteral("verticalLayout_8"));
        verticalLayout_8->setContentsMargins(0, 20, 0, 20);
        wnd_normal_top = new QWidget(normal);
        wnd_normal_top->setObjectName(QStringLiteral("wnd_normal_top"));
        verticalLayout_2 = new QVBoxLayout(wnd_normal_top);
        verticalLayout_2->setSpacing(12);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        lb_normal_title = new QLabel(wnd_normal_top);
        lb_normal_title->setObjectName(QStringLiteral("lb_normal_title"));
        lb_normal_title->setMinimumSize(QSize(0, 60));
        lb_normal_title->setMaximumSize(QSize(16777215, 60));
        lb_normal_title->setText(QStringLiteral("Tips"));
        lb_normal_title->setAlignment(Qt::AlignCenter);

        verticalLayout_2->addWidget(lb_normal_title);

        wnd_normal_icon = new QWidget(wnd_normal_top);
        wnd_normal_icon->setObjectName(QStringLiteral("wnd_normal_icon"));
        wnd_normal_icon->setMinimumSize(QSize(0, 180));
        wnd_normal_icon->setMaximumSize(QSize(16777215, 180));
        horizontalLayout_7 = new QHBoxLayout(wnd_normal_icon);
        horizontalLayout_7->setSpacing(0);
        horizontalLayout_7->setObjectName(QStringLiteral("horizontalLayout_7"));
        horizontalLayout_7->setContentsMargins(0, 0, 0, 0);
        horizontalSpacer_16 = new QSpacerItem(337, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_7->addItem(horizontalSpacer_16);

        lb_normal_icon = new QLabel(wnd_normal_icon);
        lb_normal_icon->setObjectName(QStringLiteral("lb_normal_icon"));
        lb_normal_icon->setMinimumSize(QSize(180, 180));
        lb_normal_icon->setMaximumSize(QSize(180, 180));
        lb_normal_icon->setAlignment(Qt::AlignCenter);
        lb_normal_icon->setPixmap(QPixmap(QString::fromUtf8(":/img/msg/ico_error.png")));

        horizontalLayout_7->addWidget(lb_normal_icon);

        horizontalSpacer_17 = new QSpacerItem(337, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_7->addItem(horizontalSpacer_17);


        verticalLayout_2->addWidget(wnd_normal_icon);


        verticalLayout_8->addWidget(wnd_normal_top);

        wnd_normal_tip = new QWidget(normal);
        wnd_normal_tip->setObjectName(QStringLiteral("wnd_normal_tip"));
        verticalLayout = new QVBoxLayout(wnd_normal_tip);
        verticalLayout->setSpacing(3);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        verticalSpacer = new QSpacerItem(20, 10, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);

        lb_normal_tip1 = new QLabel(wnd_normal_tip);
        lb_normal_tip1->setObjectName(QStringLiteral("lb_normal_tip1"));
        lb_normal_tip1->setMinimumSize(QSize(0, 60));
        lb_normal_tip1->setMaximumSize(QSize(16777215, 60));
        lb_normal_tip1->setText(QStringLiteral("Info1"));
        lb_normal_tip1->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(lb_normal_tip1);

        lb_normal_tip2 = new QLabel(wnd_normal_tip);
        lb_normal_tip2->setObjectName(QStringLiteral("lb_normal_tip2"));
        lb_normal_tip2->setMinimumSize(QSize(0, 60));
        lb_normal_tip2->setMaximumSize(QSize(16777215, 60));
        lb_normal_tip2->setText(QStringLiteral("Info2"));
        lb_normal_tip2->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(lb_normal_tip2);

        lb_normal_tip3 = new QLabel(wnd_normal_tip);
        lb_normal_tip3->setObjectName(QStringLiteral("lb_normal_tip3"));
        lb_normal_tip3->setMinimumSize(QSize(0, 60));
        lb_normal_tip3->setMaximumSize(QSize(16777215, 60));
        lb_normal_tip3->setText(QStringLiteral("Info3"));
        lb_normal_tip3->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(lb_normal_tip3);

        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer_2);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        verticalLayout->addItem(horizontalSpacer_3);


        verticalLayout_8->addWidget(wnd_normal_tip);

        wnd_normal_btn = new QWidget(normal);
        wnd_normal_btn->setObjectName(QStringLiteral("wnd_normal_btn"));
        wnd_normal_btn->setMinimumSize(QSize(0, 80));
        wnd_normal_btn->setMaximumSize(QSize(16777215, 80));
        horizontalLayout = new QHBoxLayout(wnd_normal_btn);
        horizontalLayout->setSpacing(40);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        horizontalSpacer = new QSpacerItem(214, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        btn_nor_normal_cancel = new QPushButton(wnd_normal_btn);
        btn_nor_normal_cancel->setObjectName(QStringLiteral("btn_nor_normal_cancel"));
        btn_nor_normal_cancel->setMinimumSize(QSize(280, 80));
        btn_nor_normal_cancel->setMaximumSize(QSize(280, 80));

        horizontalLayout->addWidget(btn_nor_normal_cancel);

        btn_nor_normal_ok = new QPushButton(wnd_normal_btn);
        btn_nor_normal_ok->setObjectName(QStringLiteral("btn_nor_normal_ok"));
        btn_nor_normal_ok->setMinimumSize(QSize(280, 80));
        btn_nor_normal_ok->setMaximumSize(QSize(280, 80));

        horizontalLayout->addWidget(btn_nor_normal_ok);

        horizontalSpacer_2 = new QSpacerItem(214, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_2);


        verticalLayout_8->addWidget(wnd_normal_btn);

        wnd_msg_content->addWidget(normal);
        page_confirm = new QWidget();
        page_confirm->setObjectName(QStringLiteral("page_confirm"));
        page_confirm->setStyleSheet(QString::fromUtf8("#edt_input_info\n"
"{\n"
"	border: 1px solid #B3B3B3;\n"
"\n"
"	font: 20pt \"\345\276\256\350\275\257\351\233\205\351\273\221\";\n"
"	color:#333333;\n"
"	background:transparent;\n"
"}"));
        verticalLayout_3 = new QVBoxLayout(page_confirm);
        verticalLayout_3->setSpacing(0);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        verticalLayout_3->setContentsMargins(0, 40, 0, 50);
        wnd_confirm_top = new QWidget(page_confirm);
        wnd_confirm_top->setObjectName(QStringLiteral("wnd_confirm_top"));
        wnd_confirm_top->setMinimumSize(QSize(0, 80));
        wnd_confirm_top->setMaximumSize(QSize(16777215, 80));
        horizontalLayout_13 = new QHBoxLayout(wnd_confirm_top);
        horizontalLayout_13->setSpacing(0);
        horizontalLayout_13->setObjectName(QStringLiteral("horizontalLayout_13"));
        horizontalLayout_13->setContentsMargins(0, 0, 0, 0);
        horizontalSpacer_27 = new QSpacerItem(337, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_13->addItem(horizontalSpacer_27);

        lb_title_confirm = new QLabel(wnd_confirm_top);
        lb_title_confirm->setObjectName(QStringLiteral("lb_title_confirm"));
        lb_title_confirm->setMinimumSize(QSize(600, 80));
        lb_title_confirm->setMaximumSize(QSize(600, 80));
        lb_title_confirm->setAlignment(Qt::AlignCenter);

        horizontalLayout_13->addWidget(lb_title_confirm);

        horizontalSpacer_28 = new QSpacerItem(337, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_13->addItem(horizontalSpacer_28);


        verticalLayout_3->addWidget(wnd_confirm_top);

        wnd_confirm_info = new QWidget(page_confirm);
        wnd_confirm_info->setObjectName(QStringLiteral("wnd_confirm_info"));
        wnd_confirm_info->setStyleSheet(QStringLiteral(""));
        verticalLayout_12 = new QVBoxLayout(wnd_confirm_info);
        verticalLayout_12->setSpacing(10);
        verticalLayout_12->setObjectName(QStringLiteral("verticalLayout_12"));
        verticalLayout_12->setContentsMargins(0, 0, 0, 0);
        verticalSpacer_7 = new QSpacerItem(20, 53, QSizePolicy::Minimum, QSizePolicy::Fixed);

        verticalLayout_12->addItem(verticalSpacer_7);

        wnd_confirm_icon = new QWidget(wnd_confirm_info);
        wnd_confirm_icon->setObjectName(QStringLiteral("wnd_confirm_icon"));
        horizontalLayout_14 = new QHBoxLayout(wnd_confirm_icon);
        horizontalLayout_14->setSpacing(0);
        horizontalLayout_14->setObjectName(QStringLiteral("horizontalLayout_14"));
        horizontalLayout_14->setContentsMargins(0, 0, 0, 0);
        horizontalSpacer_12 = new QSpacerItem(103, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_14->addItem(horizontalSpacer_12);

        lb_confirm_icon = new QLabel(wnd_confirm_icon);
        lb_confirm_icon->setObjectName(QStringLiteral("lb_confirm_icon"));
        lb_confirm_icon->setMinimumSize(QSize(120, 120));
        lb_confirm_icon->setMaximumSize(QSize(120, 120));
        lb_confirm_icon->setText(QStringLiteral(""));
        lb_confirm_icon->setPixmap(QPixmap(QString::fromUtf8(":/img/msg/ico_error.png")));
        lb_confirm_icon->setAlignment(Qt::AlignCenter);

        horizontalLayout_14->addWidget(lb_confirm_icon);

        horizontalSpacer_29 = new QSpacerItem(102, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_14->addItem(horizontalSpacer_29);


        verticalLayout_12->addWidget(wnd_confirm_icon);

        lb_tips_receive_info = new QLabel(wnd_confirm_info);
        lb_tips_receive_info->setObjectName(QStringLiteral("lb_tips_receive_info"));
        lb_tips_receive_info->setMinimumSize(QSize(0, 60));
        lb_tips_receive_info->setMaximumSize(QSize(16777215, 60));
        lb_tips_receive_info->setAlignment(Qt::AlignCenter);

        verticalLayout_12->addWidget(lb_tips_receive_info);

        wnd_input_info = new QWidget(wnd_confirm_info);
        wnd_input_info->setObjectName(QStringLiteral("wnd_input_info"));
        wnd_input_info->setMinimumSize(QSize(0, 60));
        wnd_input_info->setMaximumSize(QSize(16777215, 60));
        horizontalLayout_2 = new QHBoxLayout(wnd_input_info);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(0, 0, 0, 0);
        horizontalSpacer_5 = new QSpacerItem(78, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_5);

        lb_tips_input_info = new QLabel(wnd_input_info);
        lb_tips_input_info->setObjectName(QStringLiteral("lb_tips_input_info"));
        lb_tips_input_info->setMinimumSize(QSize(300, 60));
        lb_tips_input_info->setMaximumSize(QSize(300, 60));
        lb_tips_input_info->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout_2->addWidget(lb_tips_input_info);

        edt_input_info = new QLineEdit(wnd_input_info);
        edt_input_info->setObjectName(QStringLiteral("edt_input_info"));
        edt_input_info->setMinimumSize(QSize(320, 60));
        edt_input_info->setMaximumSize(QSize(320, 60));
        edt_input_info->setMaxLength(50);

        horizontalLayout_2->addWidget(edt_input_info);

        horizontalSpacer_4 = new QSpacerItem(78, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_4);


        verticalLayout_12->addWidget(wnd_input_info);

        verticalSpacer_8 = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_12->addItem(verticalSpacer_8);


        verticalLayout_3->addWidget(wnd_confirm_info);

        wnd_confirm_btn = new QWidget(page_confirm);
        wnd_confirm_btn->setObjectName(QStringLiteral("wnd_confirm_btn"));
        wnd_confirm_btn->setMinimumSize(QSize(0, 80));
        wnd_confirm_btn->setMaximumSize(QSize(16777215, 80));
        horizontalLayout_5 = new QHBoxLayout(wnd_confirm_btn);
        horizontalLayout_5->setSpacing(40);
        horizontalLayout_5->setObjectName(QStringLiteral("horizontalLayout_5"));
        horizontalLayout_5->setContentsMargins(0, 0, 0, 0);
        horizontalSpacer_13 = new QSpacerItem(214, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_5->addItem(horizontalSpacer_13);

        btn_nor_save_input = new QPushButton(wnd_confirm_btn);
        btn_nor_save_input->setObjectName(QStringLiteral("btn_nor_save_input"));
        btn_nor_save_input->setMinimumSize(QSize(280, 80));
        btn_nor_save_input->setMaximumSize(QSize(280, 80));

        horizontalLayout_5->addWidget(btn_nor_save_input);

        btn_nor_save_receive = new QPushButton(wnd_confirm_btn);
        btn_nor_save_receive->setObjectName(QStringLiteral("btn_nor_save_receive"));
        btn_nor_save_receive->setMinimumSize(QSize(280, 80));
        btn_nor_save_receive->setMaximumSize(QSize(280, 80));

        horizontalLayout_5->addWidget(btn_nor_save_receive);

        horizontalSpacer_14 = new QSpacerItem(214, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_5->addItem(horizontalSpacer_14);


        verticalLayout_3->addWidget(wnd_confirm_btn);

        wnd_msg_content->addWidget(page_confirm);
        btn_top_left_back = new QPushButton(wnd_container);
        btn_top_left_back->setObjectName(QStringLiteral("btn_top_left_back"));
        btn_top_left_back->setGeometry(QRect(15, 15, 80, 40));
        lb_fetch_timer = new QLabel(wnd_container);
        lb_fetch_timer->setObjectName(QStringLiteral("lb_fetch_timer"));
        lb_fetch_timer->setGeometry(QRect(735, 15, 50, 50));
        lb_fetch_timer->setText(QStringLiteral("10"));
        lb_fetch_timer->setAlignment(Qt::AlignCenter);

        retranslateUi(MsgWnd);

        QMetaObject::connectSlotsByName(MsgWnd);
    } // setupUi

    void retranslateUi(QWidget *MsgWnd)
    {
        MsgWnd->setWindowTitle(QApplication::translate("MsgWnd", "Form", nullptr));
        lb_normal_icon->setText(QString());
        btn_nor_normal_cancel->setText(QApplication::translate("MsgWnd", "Cancel", nullptr));
        btn_nor_normal_ok->setText(QApplication::translate("MsgWnd", "OK", nullptr));
        lb_title_confirm->setText(QString());
        lb_tips_receive_info->setText(QString());
        lb_tips_input_info->setText(QApplication::translate("MsgWnd", "Input new one:", nullptr));
        edt_input_info->setPlaceholderText(QApplication::translate("MsgWnd", "Sim Card ICCID", nullptr));
        btn_nor_save_input->setText(QApplication::translate("MsgWnd", "Save Input", nullptr));
        btn_nor_save_receive->setText(QApplication::translate("MsgWnd", "Save Received", nullptr));
        btn_top_left_back->setText(QApplication::translate("MsgWnd", "Exit", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MsgWnd: public Ui_MsgWnd {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MSGWND_H
