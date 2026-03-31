$f = "D:\My_work\QT_pro\ToolingBoardTest\client\src\BackendWnd.cpp"
$lines = [System.IO.File]::ReadAllLines($f, [System.Text.Encoding]::UTF8)
Write-Host "Total lines: $($lines.Count)"
$simStart = -1; $iotStart = -1
for ($i = 0; $i -lt $lines.Count; $i++) {
    if ($lines[$i] -match "void BackendWnd::on_btn_sim_record_delete_clicked") { $simStart = $i }
    if ($lines[$i] -match "void BackendWnd::on_btn_iot_record_delete_clicked") { $iotStart = $i }
}
Write-Host "simStart=$simStart iotStart=$iotStart"

# 找到函数体结束（空行后跟 "}" 独行）
function FindFuncEnd($lines, $start) {
    $depth = 0
    for ($i = $start; $i -lt $lines.Count; $i++) {
        $l = $lines[$i]
        foreach ($c in $l.ToCharArray()) {
            if ($c -eq '{') { $depth++ }
            if ($c -eq '}') { $depth--; if ($depth -eq 0) { return $i } }
        }
    }
    return -1
}

$simEnd = FindFuncEnd $lines $simStart
$iotEnd = FindFuncEnd $lines $iotStart
Write-Host "simEnd=$simEnd iotEnd=$iotEnd"

# 新的 sim delete 函数体（行数组）
$newSim = @(
    "void BackendWnd::on_btn_sim_record_delete_clicked()",
    "{",
    "    QStringList record_list;",
    "",
    "    for (int i = 0; i < model_sim_record_result_->rowCount(); i++) {",
    "        QStandardItem* chk = model_sim_record_result_->item(i, 0);",
    "        if (chk && chk->checkState() == Qt::Checked) {",
    "            QStandardItem* id = model_sim_record_result_->item(i, 1);",
    "            if (id) record_list << id->text();",
    "        }",
    "    }",
    "",
    "    if (record_list.isEmpty()) return;",
    "",
    "    pending_delete_records_ = record_list;",
    "    pending_delete_type_ = `"sim`";",
    "    MsgWnd::ShowDeleteConfirm(this);",
    "}"
)

$newIot = @(
    "void BackendWnd::on_btn_iot_record_delete_clicked()",
    "{",
    "    QStringList record_list;",
    "",
    "    for (int i = 0; i < model_iot_record_result_->rowCount(); i++) {",
    "        QStandardItem* chk = model_iot_record_result_->item(i, 0);",
    "        if (chk && chk->checkState() == Qt::Checked) {",
    "            QStandardItem* id = model_iot_record_result_->item(i, 1);",
    "            if (id) record_list << id->text();",
    "        }",
    "    }",
    "",
    "    if (record_list.isEmpty()) return;",
    "",
    "    pending_delete_records_ = record_list;",
    "    pending_delete_type_ = `"iot`";",
    "    MsgWnd::ShowDeleteConfirm(this);",
    "}"
)

# 替换 iot 先（行号更大，从后往前替换不影响 sim 行号）
$result = New-Object System.Collections.ArrayList

for ($i = 0; $i -lt $lines.Count; $i++) {
    if ($i -eq $simStart -and $simStart -ge 0) {
        foreach ($l in $newSim) { [void]$result.Add($l) }
        $i = $simEnd
    } elseif ($i -eq $iotStart -and $iotStart -ge 0) {
        foreach ($l in $newIot) { [void]$result.Add($l) }
        $i = $iotEnd
    } else {
        [void]$result.Add($lines[$i])
    }
}

[System.IO.File]::WriteAllLines($f, $result, [System.Text.Encoding]::UTF8)
Write-Host "Written OK, total lines: $($result.Count)"

# 验证
$verify = [System.IO.File]::ReadAllLines($f, [System.Text.Encoding]::UTF8)
$found = $false
foreach ($l in $verify) {
    if ($l -match "getSelectedRowsData.*tb_sim_record|getSelectedRowsData.*tb_iot_record") {
        Write-Host "STILL HAS OLD: $l"
        $found = $true
    }
}
if (-not $found) { Write-Host "VERIFIED: No old getSelectedRowsData for sim/iot delete handlers" }
