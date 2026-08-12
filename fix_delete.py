import re

file_path = r'D:\My_work\QT_pro\ToolingBoardTest\client\src\BackendWnd.cpp'

with open(file_path, 'rb') as f:
    data = f.read()

# Check line endings
if b'\r\n' in data:
    print("Line endings: CRLF")
    nl = b'\r\n'
else:
    print("Line endings: LF")
    nl = b'\n'

# Find sim delete handler
idx = data.find(b'on_btn_sim_record_delete_clicked')
if idx >= 0:
    snippet = data[idx:idx+400]
    print("SIM delete snippet repr:", repr(snippet))
else:
    print("SIM delete handler NOT found")

idx2 = data.find(b'on_btn_iot_record_delete_clicked')
if idx2 >= 0:
    snippet2 = data[idx2:idx2+400]
    print("IOT delete snippet repr:", repr(snippet2))

# Replace sim delete handler
old_sim = (
    b'void BackendWnd::on_btn_sim_record_delete_clicked()' + nl +
    b'{' + nl +
    b'    QStringList record_list;' + nl +
    nl +
    b'    // \xe7\x94\xb1\xe4\xba\x8e\xe6\xb7\xbb\xe5\x8a\xa0\xe4\xba\x86\xe5\xa4\x8d\xe9\x80\x89\xe6\xa1\x86\xe5\x88\x97\xef\xbc\x8c\xe6\xb5\x81\xe6\xb0\xb4\xe5\x8f\xb7\xe5\x88\x97\xe7\xb4\xa2\xe5\xbc\x95\xe4\xbb\x8e0\xe5\x8f\x98\xe4\xb8\xba1' + nl +
    b'    TableHelper::getSelectedRowsData(ui->tb_sim_record_result, 1, record_list);' + nl +
    nl +
    b'    if (record_list.isEmpty()) return;' + nl +
    nl +
    b'    // \xe5\xad\x98\xe5\x82\xa8\xe5\xbe\x85\xe5\x88\xa0\xe8\xae\xb0\xe5\xbd\x95\xef\xbc\x8c\xe5\xbc\xb9\xe5\x87\xba\xe7\xa1\xae\xe8\xae\xa4\xe5\xaf\xb9\xe8\xaf\x9d\xe6\xa1\x86' + nl +
    b'    pending_delete_records_ = record_list;' + nl +
    b'    pending_delete_type_ = "sim";' + nl +
    b'    MsgWnd::ShowDeleteConfirm(this);' + nl +
    b'}'
)

new_sim = (
    b'void BackendWnd::on_btn_sim_record_delete_clicked()' + nl +
    b'{' + nl +
    b'    QStringList record_list;' + nl +
    nl +
    b'    // \xe6\x89\xab\xe6\x8f\x8f\xe5\xa4\x8d\xe9\x80\x89\xe6\xa1\x86\xe7\x8a\xb6\xe6\x80\x81\xe8\x8e\xb7\xe5\x8f\x96\xe5\x8b\xbe\xe9\x80\x89\xe8\xa1\x8c\xe7\x9a\x84\xe6\xb5\x81\xe6\xb0\xb4\xe5\x8f\xb7\xef\xbc\x88\xe5\x88\x97 1\xef\xbc\x89' + nl +
    b'    for (int i = 0; i < model_sim_record_result_->rowCount(); i++) {' + nl +
    b'        QStandardItem* chk = model_sim_record_result_->item(i, 0);' + nl +
    b'        if (chk && chk->checkState() == Qt::Checked) {' + nl +
    b'            QStandardItem* id = model_sim_record_result_->item(i, 1);' + nl +
    b'            if (id) record_list << id->text();' + nl +
    b'        }' + nl +
    b'    }' + nl +
    nl +
    b'    if (record_list.isEmpty()) return;' + nl +
    nl +
    b'    // \xe5\xad\x98\xe5\x82\xa8\xe5\xbe\x85\xe5\x88\xa0\xe8\xae\xb0\xe5\xbd\x95\xef\xbc\x8c\xe5\xbc\xb9\xe5\x87\xba\xe7\xa1\xae\xe8\xae\xa4\xe5\xaf\xb9\xe8\xaf\x9d\xe6\xa1\x86' + nl +
    b'    pending_delete_records_ = record_list;' + nl +
    b'    pending_delete_type_ = "sim";' + nl +
    b'    MsgWnd::ShowDeleteConfirm(this);' + nl +
    b'}'
)

old_iot = (
    b'void BackendWnd::on_btn_iot_record_delete_clicked()' + nl +
    b'{' + nl +
    b'    QStringList record_list;' + nl +
    nl +
    b'    // \xe7\x94\xb1\xe4\xba\x8e\xe6\xb7\xbb\xe5\x8a\xa0\xe4\xba\x86\xe5\xa4\x8d\xe9\x80\x89\xe6\xa1\x86\xe5\x88\x97\xef\xbc\x8c\xe6\xb5\x81\xe6\xb0\xb4\xe5\x8f\xb7\xe5\x88\x97\xe7\xb4\xa2\xe5\xbc\x95\xe4\xbb\x8e0\xe5\x8f\x98\xe4\xb8\xba1' + nl +
    b'    TableHelper::getSelectedRowsData(ui->tb_iot_record_result, 1, record_list);' + nl +
    nl +
    b'    if (record_list.isEmpty()) return;' + nl +
    nl +
    b'    // \xe5\xad\x98\xe5\x82\xa8\xe5\xbe\x85\xe5\x88\xa0\xe8\xae\xb0\xe5\xbd\x95\xef\xbc\x8c\xe5\xbc\xb9\xe5\x87\xba\xe7\xa1\xae\xe8\xae\xa4\xe5\xaf\xb9\xe8\xaf\x9d\xe6\xa1\x86' + nl +
    b'    pending_delete_records_ = record_list;' + nl +
    b'    pending_delete_type_ = "iot";' + nl +
    b'    MsgWnd::ShowDeleteConfirm(this);' + nl +
    b'}'
)

new_iot = (
    b'void BackendWnd::on_btn_iot_record_delete_clicked()' + nl +
    b'{' + nl +
    b'    QStringList record_list;' + nl +
    nl +
    b'    // \xe6\x89\xab\xe6\x8f\x8f\xe5\xa4\x8d\xe9\x80\x89\xe6\xa1\x86\xe7\x8a\xb6\xe6\x80\x81\xe8\x8e\xb7\xe5\x8f\x96\xe5\x8b\xbe\xe9\x80\x89\xe8\xa1\x8c\xe7\x9a\x84\xe6\xb5\x81\xe6\xb0\xb4\xe5\x8f\xb7\xef\xbc\x88\xe5\x88\x97 1\xef\xbc\x89' + nl +
    b'    for (int i = 0; i < model_iot_record_result_->rowCount(); i++) {' + nl +
    b'        QStandardItem* chk = model_iot_record_result_->item(i, 0);' + nl +
    b'        if (chk && chk->checkState() == Qt::Checked) {' + nl +
    b'            QStandardItem* id = model_iot_record_result_->item(i, 1);' + nl +
    b'            if (id) record_list << id->text();' + nl +
    b'        }' + nl +
    b'    }' + nl +
    nl +
    b'    if (record_list.isEmpty()) return;' + nl +
    nl +
    b'    // \xe5\xad\x98\xe5\x82\xa8\xe5\xbe\x85\xe5\x88\xa0\xe8\xae\xb0\xe5\xbd\x95\xef\xbc\x8c\xe5\xbc\xb9\xe5\x87\xba\xe7\xa1\xae\xe8\xae\xa4\xe5\xaf\xb9\xe8\xaf\x9d\xe6\xa1\x86' + nl +
    b'    pending_delete_records_ = record_list;' + nl +
    b'    pending_delete_type_ = "iot";' + nl +
    b'    MsgWnd::ShowDeleteConfirm(this);' + nl +
    b'}'
)

# Try replacement
if old_sim in data:
    data = data.replace(old_sim, new_sim)
    print("SIM replaced OK")
else:
    print("SIM old pattern NOT matched - trying simpler approach")
    # simpler: just replace the getSelectedRowsData line
    simple_old = b'    TableHelper::getSelectedRowsData(ui->tb_sim_record_result, 1, record_list);'
    simple_new = (
        b'    for (int i = 0; i < model_sim_record_result_->rowCount(); i++) {' + nl +
        b'        QStandardItem* chk = model_sim_record_result_->item(i, 0);' + nl +
        b'        if (chk && chk->checkState() == Qt::Checked) {' + nl +
        b'            QStandardItem* id = model_sim_record_result_->item(i, 1);' + nl +
        b'            if (id) record_list << id->text();' + nl +
        b'        }' + nl +
        b'    }'
    )
    if simple_old in data:
        data = data.replace(simple_old, simple_new)
        print("SIM simple replaced OK")
    else:
        print("SIM simple pattern also NOT matched!")

if old_iot in data:
    data = data.replace(old_iot, new_iot)
    print("IOT replaced OK")
else:
    print("IOT old pattern NOT matched - trying simpler approach")
    simple_old2 = b'    TableHelper::getSelectedRowsData(ui->tb_iot_record_result, 1, record_list);'
    simple_new2 = (
        b'    for (int i = 0; i < model_iot_record_result_->rowCount(); i++) {' + nl +
        b'        QStandardItem* chk = model_iot_record_result_->item(i, 0);' + nl +
        b'        if (chk && chk->checkState() == Qt::Checked) {' + nl +
        b'            QStandardItem* id = model_iot_record_result_->item(i, 1);' + nl +
        b'            if (id) record_list << id->text();' + nl +
        b'        }' + nl +
        b'    }'
    )
    if simple_old2 in data:
        data = data.replace(simple_old2, simple_new2)
        print("IOT simple replaced OK")
    else:
        print("IOT simple pattern also NOT matched!")

with open(file_path, 'wb') as f:
    f.write(data)

print("Done. Verifying...")
with open(file_path, 'r', encoding='utf-8') as f:
    lines = f.readlines()

for i, line in enumerate(lines):
    if 'getSelectedRowsData' in line and ('tb_sim_record' in line or 'tb_iot_record' in line):
        print(f"STILL FOUND at line {i+1}: {line.rstrip()}")
    if 'model_sim_record_result_->rowCount' in line or 'model_iot_record_result_->rowCount' in line:
        print(f"NEW code at line {i+1}: {line.rstrip()}")
