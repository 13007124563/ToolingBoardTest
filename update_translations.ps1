# =============================================================================
# update_translations.ps1
# 更新并发布 ToolingBoardTest 翻译文件（Windows）
#
# 功能：
#   1. 自动查找 Qt 的 lrelease 工具
#   2. 运行 lrelease 将 .ts 编译为 .qm 文件
#   3. 将 .qm 文件同步到项目所有需要的目录
#
# 用法：
#   .\update_translations.ps1
#   .\update_translations.ps1 -QtBinPath "C:\Qt\6.5.0\msvc2019_64\bin"
# =============================================================================

param(
    [string]$QtBinPath = ""
)

$ErrorActionPreference = "Stop"

# ── 颜色输出 ──────────────────────────────────────────────────────────────────
function Write-Info($msg) {
    Write-Host "[INFO]  " -ForegroundColor Blue -NoNewline
    Write-Host $msg
}

function Write-Success($msg) {
    Write-Host "[OK]    " -ForegroundColor Green -NoNewline
    Write-Host $msg
}

function Write-Warn($msg) {
    Write-Host "[WARN]  " -ForegroundColor Yellow -NoNewline
    Write-Host $msg
}

function Write-Error($msg) {
    Write-Host "[ERROR] " -ForegroundColor Red -NoNewline
    Write-Host $msg
}

# ── 脚本所在目录（项目根目录）──────────────────────────────────────────────────
$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
Set-Location $ScriptDir

Write-Info "项目根目录: $ScriptDir"
Write-Host ""

# ── 查找 lrelease ─────────────────────────────────────────────────────────────
function Find-LRelease {
    param([string]$QtBinHint)
    
    # 优先使用用户指定路径
    if ($QtBinHint) {
        $candidate = Join-Path $QtBinHint "lrelease.exe"
        if (Test-Path $candidate) {
            return $candidate
        }
        Write-Warn "指定的 Qt bin 目录中未找到 lrelease.exe: $QtBinHint"
    }
    
    # 检查环境变量 PATH
    $lrelease = Get-Command lrelease.exe -ErrorAction SilentlyContinue
    if ($lrelease) {
        return $lrelease.Source
    }
    
    # 搜索常见 Qt 安装位置
    $drives = @("C:\", "D:\")
    $qtPaths = @()
    
    foreach ($drive in $drives) {
        if (Test-Path $drive) {
            # 搜索 C:\Qt, D:\Qt
            $qtDir = Join-Path $drive "Qt"
            if (Test-Path $qtDir) {
                $qtPaths += Get-ChildItem -Path $qtDir -Filter "lrelease.exe" -Recurse -ErrorAction SilentlyContinue | 
                            Select-Object -ExpandProperty FullName
            }
        }
    }
    
    # 返回第一个找到的（通常选择最新版本）
    if ($qtPaths.Count -gt 0) {
        $qtPaths = $qtPaths | Sort-Object -Descending
        return $qtPaths[0]
    }
    
    return $null
}

$LRelease = Find-LRelease -QtBinHint $QtBinPath

if (-not $LRelease) {
    Write-Error "未找到 lrelease.exe。请执行以下操作之一："
    Write-Error "  1. 确保 Qt bin 目录在 PATH 环境变量中"
    Write-Error "  2. 使用 -QtBinPath 参数指定 Qt bin 目录"
    Write-Error "     例如: .\update_translations.ps1 -QtBinPath 'C:\Qt\6.5.0\msvc2019_64\bin'"
    exit 1
}

Write-Info "lrelease → $LRelease"
Write-Host ""

# ── 翻译文件定义 ──────────────────────────────────────────────────────────────
$TsCn = "ToolingBoardTest_cn.ts"
$TsEn = "ToolingBoardTest_en.ts"
$QmCn = "ToolingBoardTest_cn.qm"
$QmEn = "ToolingBoardTest_en.qm"

# .qm 需要同步的目标目录
$QmDestDirs = @(
    "client\res",
    "Win32\Debug",
    "Win32\Release"
)

# ── 步骤 1：lrelease ──────────────────────────────────────────────────────────
Write-Host "════════════════════════════════════════"
Write-Info "步骤 1/2  编译 .ts → .qm"
Write-Host "════════════════════════════════════════"

foreach ($tsFile in @($TsCn, $TsEn)) {
    if (-not (Test-Path $tsFile)) {
        Write-Error "未找到 .ts 文件: $tsFile"
        exit 1
    }
    
    Write-Info "正在编译: $tsFile"
    & $LRelease $tsFile
    
    if ($LASTEXITCODE -ne 0) {
        Write-Error "编译失败: $tsFile"
        exit 1
    }
    
    Write-Success "编译完成: $tsFile"
}
Write-Host ""

# ── 步骤 2：同步 .qm 文件到各发布目录 ────────────────────────────────────────
Write-Host "════════════════════════════════════════"
Write-Info "步骤 2/2  发布 .qm 到目标目录"
Write-Host "════════════════════════════════════════"

foreach ($dest in $QmDestDirs) {
    if (-not (Test-Path $dest)) {
        Write-Warn "目标目录不存在，创建: $dest"
        New-Item -ItemType Directory -Path $dest -Force | Out-Null
    }
    
    foreach ($qm in @($QmCn, $QmEn)) {
        if (-not (Test-Path $qm)) {
            Write-Error ".qm 文件未生成: $qm"
            exit 1
        }
        
        $destFile = Join-Path $dest $qm
        Copy-Item -Path $qm -Destination $destFile -Force
        Write-Info "  复制: $qm → $dest\"
    }
    
    Write-Success "已发布到: $dest"
}
Write-Host ""

# ── 汇总 ──────────────────────────────────────────────────────────────────────
Write-Host "════════════════════════════════════════"
Write-Success "翻译文件更新完成！"
Write-Host ""
Write-Host "  生成文件:"
foreach ($qm in @($QmCn, $QmEn)) {
    $size = (Get-Item $qm).Length
    $sizeKB = [math]::Round($size / 1KB, 2)
    Write-Host "    $qm  ($sizeKB KB)"
}
Write-Host ""
Write-Host "  已同步目录:"
foreach ($dest in $QmDestDirs) {
    Write-Host "    $dest\"
}
Write-Host "════════════════════════════════════════"
Write-Host ""
Write-Success "下一步：重新编译 C++ 项目以应用代码更改"
Write-Info "在 Qt Creator 中执行: Build -> Rebuild All"
