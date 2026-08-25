# 补全翻译文件脚本
$ErrorActionPreference = "Stop"

$translations = @{
    # MainWnd UI 标签
    "Module Type:" = "模块类型:"
    "APN:" = "APN:"
    "NET:" = "NET:"
    "Image Version" = "镜像版本"
    "IOT Test" = "IOT测试"
    "Clear Test Result" = "清除测试结果"
    "Image Version:" = "镜像版本:"
    "Network Type:" = "网络类型:"
    "Sim Network Status:" = "Sim卡网络状态:"
    "Sim Card ICCID:" = "Sim卡ICCID:"
    "Signal Strength:" = "信号强度:"
    "IOT Version:" = "IOT版本:"
    "IOT IMEI:" = "IOT IMEI:"
    "Test Execution Process:" = "测试执行过程:"
   
    # BackendWnd 关键文本
    "Connection Failed" = "连接失败"
    "Connection Successful" = "连接成功"
    "Not Connected" = "未连接"
    "View Report" = "查看报告"
    "Text Files (*.txt);;All Files (*)" = "文本文件 (*.txt);;所有文件 (*)"
    "Cannot create file: %1" = "无法创建文件: %1"
    "Record ID: " = "记录ID: "
    "Test Time: " = "测试时间: "
    "Sim ICCID: " = "Sim ICCID: "
    "Network Status: " = "网络状态: "
    "Network Type: " = "网络类型: "
    "Signal Strength: " = "信号强度: "
    "Test Log:" = "测试日志:"
    "Export Success" = "导出成功"
    "Report exported successfully to: %1" = "报告已成功导出到: %1"
    "Result: " = "结果: "
    "IOT Module ID: " = "IOT模块ID: "
    "IOT IMEI: " = "IOT IMEI: "
    "Eseye" = "Eseye"
    "1Nice" = "1Nice"
    
    # MsgWnd - 部分已翻译
    
    # BannerWnd样式
    "font: 16pt &quot;微软雅黑&quot;; color: #333333;" = "[样式]"
    
    # MainWnd 额外项
    "Windows test mode: Version query simulated" = "Windows测试模式：版本查询已模拟"
    "Failed to query image version" = "查询镜像版本失败"
    
    # 通用Form标签 - 保持不变或翻译为空
    "Form" = "窗体"
}

Write-Host "`n开始补全翻译..." -ForegroundColor Cyan

$content = Get-Content "ToolingBoardTest_cn.ts" -Raw -Encoding UTF8
$modified = $content

$count = 0
foreach ($key in $translations.Keys) {
    $escapedKey = [regex]::Escape($key)
    # 查找 <source>key</source> 后紧跟 <translation type="unfinished"></translation> 或 <translation></translation>
    $pattern = "(<source>$escapedKey</source>\s*<translation[^>]*>)</translation>"
    $replacement = "`${1}$($translations[$key])</translation>"
    
    $before = $modified
    $modified = $modified -replace $pattern, $replacement
    
    if ($modified -ne $before) {
        $count++
        Write-Host ("  ✓ 已翻译: {0,-40} → {1}" -f $key, $translations[$key]) -ForegroundColor Green
    }
}

$modified | Set-Content "ToolingBoardTest_cn.ts" -Encoding UTF8 -NoNewline

Write-Host "`n✅ 完成! 共补全 $count 项翻译" -ForegroundColor Green
Write-Host "`n统计剩余未翻译项..." -ForegroundColor Yellow
$empty = ([regex]::Matches($modified, '<translation[^>]*></translation>')).Count
Write-Host "剩余空翻译: $empty 个" -ForegroundColor $(if ($empty -gt 50) { "Red" } else { "Yellow" })
