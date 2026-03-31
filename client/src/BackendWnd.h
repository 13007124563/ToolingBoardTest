#ifndef BACKENDWND_H
#define BACKENDWND_H

#include <QWidget>
#include <QCheckBox>
#include "CommonType.h"
#include "GlobalSignal.h"

namespace Ui {
class BackendWnd;
}

class QButtonGroup;
class QAbstractButton;

class QStandardItemModel;

class QComboBox;
class QTableView;

class QCompleter;

using namespace zl;
class BackendWnd : public QWidget
{
    Q_OBJECT

public:
    explicit BackendWnd(QWidget *parent = 0);
    ~BackendWnd();

    void ShowBackendPage();
    void SetBackendPage(zl::EBP backendPage);

    void NotifyQuitBackend();

    void LoadData();
    void UpdatePanelAdvance();
protected:

    void init();

    void init_table_view();
    void initSelectPage();
    void init_record_page();

    void init_advance_page();

    void update_advance_info();

    void update_combo_completer();

private:

    void init_sim_page_time_select();
    void init_iot_page_time_select();

    void init_sim_page_combo_normal();
    void init_sim_page_combo_completer();

    void init_iot_page_combo_normal();
    void init_iot_page_combo_completer();

    void update_sim_page_combo_completer();
    void update_iot_page_combo_completer();
protected slots:
    void lang_change();

    void event_backend_select_btn_clicked(int index);

    void event_navi_btn_clicked(int index);

    void on_btn_sim_record_query_clicked();
    void on_btn_sim_record_export_excel_clicked();

    void on_btn_sim_record_reset_clicked();
    void on_btn_sim_record_delete_clicked();

    void on_btn_iot_record_query_clicked();
    void on_btn_iot_record_export_excel_clicked();

    void on_btn_iot_record_reset_clicked();
    void on_btn_iot_record_delete_clicked();

    void on_sim_header_checkbox_clicked(bool checked);
    void on_iot_header_checkbox_clicked(bool checked);

    void on_tb_sim_record_result_doubleClicked(const QModelIndex &index);
    void on_tb_iot_record_result_doubleClicked(const QModelIndex &index);

    void showTestReportDialog(const QString& recordId, const QString& testType);

    void on_btn_lang_cn_clicked();

    void on_btn_lang_en_clicked();

    void event_user_confirm(const QString& token);

    void on_btn_save_advance_clicked();
signals:
    void configChanged();
    void requestBackToHome();

private:
    Ui::BackendWnd *ui;

    QButtonGroup*           btnGpBackendSel;            // 组合按键 选择后台模块
    QButtonGroup*           btnGpRecordSel;             // 组合按键 选择记录类型按钮

    QStandardItemModel*		model_sim_record_result_;
    QStandardItemModel*		model_iot_record_result_;

    QCheckBox*              sim_header_checkbox_;       // SIM表头全选复选框
    QCheckBox*              iot_header_checkbox_;       // IOT表头全选复选框


    QCompleter*             completer_sim_page_sim_iccid;
    QCompleter*             completer_sim_page_firmware_version;

    QCompleter*             completer_iot_page_iot_module_id;
    QCompleter*             completer_iot_page_iot_imei;
    QCompleter*             completer_iot_page_firmware_version;

    zl::RecordVec           sim_record_vec_;                 // 当前显示的sim卡测试记录
    zl::RecordVec           iot_record_vec_;                 // 当前显示的IOT测试记录

    QStringList             pending_delete_records_;         // 待删除记录ID列表
    QString                 pending_delete_type_;            // 待删除记录类型："sim" 或 "iot"
};

#endif // BACKENDWND_H
