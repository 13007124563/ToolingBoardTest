/********************************************************************************
** Form generated from reading UI file 'BannerWnd.ui'
**
** Created by: Qt User Interface Compiler version 5.10.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_BANNERWND_H
#define UI_BANNERWND_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_BannerWnd
{
public:
    QWidget *widget_bk;
    QVBoxLayout *verticalLayout_2;
    QWidget *wnd_top_info;
    QHBoxLayout *horizontalLayout_2;
    QLabel *lb_logo;
    QLabel *lb_title;
    QWidget *wnd_time;
    QVBoxLayout *verticalLayout;
    QLabel *lb_day_time;
    QLabel *lb_date_time;
    QStackedWidget *stackedWidget_banner;
    QWidget *page_back_sel;
    QHBoxLayout *horizontalLayout_4;
    QPushButton *btn_quit_bk;
    QLabel *lb_back_sel_title;
    QPushButton *btn_exit;
    QWidget *page_back;
    QHBoxLayout *horizontalLayout_3;
    QPushButton *btn_bk_back;
    QLabel *lb_backend_title;
    QLabel *lb_pos;

    void setupUi(QWidget *BannerWnd)
    {
        if (BannerWnd->objectName().isEmpty())
            BannerWnd->setObjectName(QStringLiteral("BannerWnd"));
        BannerWnd->resize(1280, 90);
        BannerWnd->setStyleSheet(QString::fromUtf8("#BannerWnd\n"
",#widget_bk \n"
"{\n"
"	background:transparent;\n"
"}\n"
"\n"
"QLabel\n"
"{\n"
"	background:transparent;\n"
"}\n"
"\n"
"#wnd_ad\n"
",#wnd_top_info\n"
",#wnd_time\n"
"{\n"
"	background:transparent;\n"
"}\n"
"\n"
"#wnd_top_info\n"
"{\n"
"	background-color:#FFFFFF;\n"
"    border-bottom: 1px solid #D5D5D5;\n"
"}\n"
"\n"
"#stackedWidget_banner\n"
"#page_back_sel\n"
",#page_back\n"
"{\n"
"	background-color:#FFFFFF;\n"
"}\n"
"\n"
"#lb_logo\n"
"{\n"
"    /* Logo styling if needed */\n"
"}\n"
"\n"
"#lb_title\n"
"{\n"
"	font: 24pt \"\345\276\256\350\275\257\351\233\205\351\273\221\";\n"
"	color:#333333;\n"
"}\n"
"\n"
"#lb_date_time\n"
",#lb_day_time\n"
"{\n"
"	font: 12pt \"\345\276\256\350\275\257\351\233\205\351\273\221\";\n"
"	color:#333333;\n"
"}\n"
"\n"
"#lb_main_title\n"
",#lb_backend_title\n"
",#lb_back_sel_title\n"
"{\n"
"	font: 30pt \"\345\276\256\350\275\257\351\233\205\351\273\221\";\n"
"	color: #FFFFFF;\n"
"}\n"
"\n"
"#lb_timer\n"
"{\n"
"	font: 24pt \"\345\276\256\350\275\257\351\233\205\351\273"
                        "\221\";\n"
"	color: #FFFFFF;\n"
"}\n"
"\n"
"#wnd_timer{\n"
"	background:transparent;\n"
"}"));
        widget_bk = new QWidget(BannerWnd);
        widget_bk->setObjectName(QStringLiteral("widget_bk"));
        widget_bk->setGeometry(QRect(0, 0, 1280, 90));
        widget_bk->setMinimumSize(QSize(1280, 90));
        widget_bk->setMaximumSize(QSize(1280, 90));
        verticalLayout_2 = new QVBoxLayout(widget_bk);
        verticalLayout_2->setSpacing(0);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        wnd_top_info = new QWidget(widget_bk);
        wnd_top_info->setObjectName(QStringLiteral("wnd_top_info"));
        wnd_top_info->setMinimumSize(QSize(1280, 90));
        wnd_top_info->setMaximumSize(QSize(1280, 90));
        wnd_top_info->setStyleSheet(QStringLiteral(""));
        horizontalLayout_2 = new QHBoxLayout(wnd_top_info);
        horizontalLayout_2->setSpacing(0);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(20, 10, 20, 10);
        lb_logo = new QLabel(wnd_top_info);
        lb_logo->setObjectName(QStringLiteral("lb_logo"));
        lb_logo->setMinimumSize(QSize(230, 70));
        lb_logo->setMaximumSize(QSize(230, 70));
        lb_logo->setPixmap(QPixmap(QString::fromUtf8(":/img/banner/logo0001.png")));
        lb_logo->setScaledContents(true);

        horizontalLayout_2->addWidget(lb_logo);

        lb_title = new QLabel(wnd_top_info);
        lb_title->setObjectName(QStringLiteral("lb_title"));
        lb_title->setMinimumSize(QSize(0, 60));
        lb_title->setMaximumSize(QSize(550, 60));
        lb_title->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

        horizontalLayout_2->addWidget(lb_title);

        wnd_time = new QWidget(wnd_top_info);
        wnd_time->setObjectName(QStringLiteral("wnd_time"));
        verticalLayout = new QVBoxLayout(wnd_time);
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        lb_day_time = new QLabel(wnd_time);
        lb_day_time->setObjectName(QStringLiteral("lb_day_time"));
        lb_day_time->setMinimumSize(QSize(0, 40));
        lb_day_time->setText(QStringLiteral("2026-01-01"));
        lb_day_time->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        verticalLayout->addWidget(lb_day_time);

        lb_date_time = new QLabel(wnd_time);
        lb_date_time->setObjectName(QStringLiteral("lb_date_time"));
        lb_date_time->setMinimumSize(QSize(0, 40));
        lb_date_time->setMaximumSize(QSize(16777215, 60));
        lb_date_time->setText(QStringLiteral("00:00:00"));
        lb_date_time->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        verticalLayout->addWidget(lb_date_time);


        horizontalLayout_2->addWidget(wnd_time);


        verticalLayout_2->addWidget(wnd_top_info);

        stackedWidget_banner = new QStackedWidget(widget_bk);
        stackedWidget_banner->setObjectName(QStringLiteral("stackedWidget_banner"));
        stackedWidget_banner->setMinimumSize(QSize(1024, 90));
        stackedWidget_banner->setMaximumSize(QSize(1024, 90));
        stackedWidget_banner->setStyleSheet(QString::fromUtf8("QPushButton\n"
"{\n"
"	border: none;\n"
"	border-radius:6px;\n"
"	background-color: #FFFFFF;\n"
"	\n"
"	font: 22pt \"\345\276\256\350\275\257\351\233\205\351\273\221\";\n"
"	color:#005FA7;\n"
"}\n"
"\n"
"QPushButton:pressed\n"
"{\n"
"	background-color:#E4E4E4;\n"
"}\n"
"\n"
"QPushButton[objectName^=btn_nor_]:disabled\n"
"{\n"
"	background-color:#999999;\n"
"}\n"
""));
        page_back_sel = new QWidget();
        page_back_sel->setObjectName(QStringLiteral("page_back_sel"));
        horizontalLayout_4 = new QHBoxLayout(page_back_sel);
        horizontalLayout_4->setSpacing(6);
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        horizontalLayout_4->setContentsMargins(20, 20, 20, 20);
        btn_quit_bk = new QPushButton(page_back_sel);
        btn_quit_bk->setObjectName(QStringLiteral("btn_quit_bk"));
        btn_quit_bk->setMinimumSize(QSize(120, 50));
        btn_quit_bk->setMaximumSize(QSize(120, 50));

        horizontalLayout_4->addWidget(btn_quit_bk);

        lb_back_sel_title = new QLabel(page_back_sel);
        lb_back_sel_title->setObjectName(QStringLiteral("lb_back_sel_title"));
        lb_back_sel_title->setMinimumSize(QSize(400, 40));
        lb_back_sel_title->setMaximumSize(QSize(16777215, 40));
        lb_back_sel_title->setAlignment(Qt::AlignCenter);

        horizontalLayout_4->addWidget(lb_back_sel_title);

        btn_exit = new QPushButton(page_back_sel);
        btn_exit->setObjectName(QStringLiteral("btn_exit"));
        btn_exit->setMinimumSize(QSize(120, 50));
        btn_exit->setMaximumSize(QSize(120, 50));

        horizontalLayout_4->addWidget(btn_exit);

        stackedWidget_banner->addWidget(page_back_sel);
        page_back = new QWidget();
        page_back->setObjectName(QStringLiteral("page_back"));
        page_back->setStyleSheet(QStringLiteral(""));
        horizontalLayout_3 = new QHBoxLayout(page_back);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        horizontalLayout_3->setContentsMargins(20, 20, 20, 20);
        btn_bk_back = new QPushButton(page_back);
        btn_bk_back->setObjectName(QStringLiteral("btn_bk_back"));
        btn_bk_back->setMinimumSize(QSize(120, 50));
        btn_bk_back->setMaximumSize(QSize(120, 50));

        horizontalLayout_3->addWidget(btn_bk_back);

        lb_backend_title = new QLabel(page_back);
        lb_backend_title->setObjectName(QStringLiteral("lb_backend_title"));
        lb_backend_title->setMinimumSize(QSize(400, 40));
        lb_backend_title->setMaximumSize(QSize(16777215, 40));
        lb_backend_title->setAlignment(Qt::AlignCenter);

        horizontalLayout_3->addWidget(lb_backend_title);

        lb_pos = new QLabel(page_back);
        lb_pos->setObjectName(QStringLiteral("lb_pos"));
        lb_pos->setMinimumSize(QSize(162, 82));
        lb_pos->setMaximumSize(QSize(162, 82));

        horizontalLayout_3->addWidget(lb_pos);

        stackedWidget_banner->addWidget(page_back);

        verticalLayout_2->addWidget(stackedWidget_banner);


        retranslateUi(BannerWnd);

        QMetaObject::connectSlotsByName(BannerWnd);
    } // setupUi

    void retranslateUi(QWidget *BannerWnd)
    {
        BannerWnd->setWindowTitle(QApplication::translate("BannerWnd", "Form", nullptr));
        lb_logo->setText(QString());
        lb_title->setStyleSheet(QApplication::translate("BannerWnd", "font: 16pt \"\345\276\256\350\275\257\351\233\205\351\273\221\"; color: #333333;", nullptr));
        lb_title->setText(QApplication::translate("BannerWnd", "IOT\346\250\241\345\235\227\346\265\213\350\257\225\345\267\245\345\205\267", nullptr));
        btn_quit_bk->setText(QApplication::translate("BannerWnd", "Home", nullptr));
        lb_back_sel_title->setText(QApplication::translate("BannerWnd", "Select Module", nullptr));
        btn_exit->setText(QApplication::translate("BannerWnd", "Exit", nullptr));
        btn_bk_back->setText(QApplication::translate("BannerWnd", "Back", nullptr));
        lb_backend_title->setText(QApplication::translate("BannerWnd", "None", nullptr));
        lb_pos->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class BannerWnd: public Ui_BannerWnd {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_BANNERWND_H
