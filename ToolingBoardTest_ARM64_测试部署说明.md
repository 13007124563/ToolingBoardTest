# ToolingBoardTest ARM64 测试部署与运行说明

本文说明测试同学如何将 `ToolingBoardTest_arm64_package.tar.gz` 拷贝到 **ARM64 开发板/工控机**，解压、赋权并启动程序。

---

## 1. 适用范围

| 项目 | 说明 |
|------|------|
| 压缩包 | `ToolingBoardTest_arm64_package.tar.gz`（约 87MB） |
| 目标机器 | **aarch64 / ARM64** Linux 开发板（如米尔 MYIR / i.MX 系列） |
| 图形环境 | 支持 Wayland 或 X11（XCB）桌面 |
| 不支持 | macOS、Windows、x86_64 PC 直接运行本包内可执行文件 |

> 本包为已打包的运行目录，内含可执行文件、Qt 依赖库与插件，**无需在测试机上再编译**。

---

## 2. 压缩包内容概览

解压后目录结构大致如下：

```text
ToolingBoardTest_arm64_package/
├── ToolingBoardTest          # 主程序（ARM64 ELF）
├── run.sh                    # 推荐启动脚本（自动选择 Wayland/XCB）
├── run_wayland.sh            # 强制 Wayland 启动
├── run_xcb.sh                # 强制 X11/XCB 启动
├── iot_start.sh              # IOT 相关脚本
├── config.json               # 配置（串口、语言等）
├── ToolingBoardTest.db       # 本地数据库
├── ToolingBoardTest_cn.qm    # 中文翻译
├── ToolingBoardTest_en.qm    # 英文翻译
├── libs/                     # Qt 及依赖动态库
├── plugins/                  # Qt 插件
└── logs/                     # 运行日志目录
```

---

## 3. 准备工作

### 3.1 确认目标机架构

在开发板上执行：

```bash
uname -m
```

应输出 `aarch64`。若为 `x86_64` / `armv7l`，则本压缩包不适用。

### 3.2 确认有图形界面

程序为 Qt GUI，需在带桌面的会话中运行（本机屏幕或远程桌面均可）。可检查：

```bash
echo "WAYLAND_DISPLAY=$WAYLAND_DISPLAY"
echo "DISPLAY=$DISPLAY"
```

至少其一有值，或存在 Wayland socket（常见路径如 `/run/user/0/wayland-0`）。

### 3.3 串口权限（做板测时需要）

若测试涉及串口，确认当前用户可访问串口设备，例如：

```bash
ls -l /dev/ttyLP* /dev/ttyUSB* /dev/ttyACM* 2>/dev/null
```

---

## 4. 将压缩包拷贝到开发机器

任选一种方式即可。

### 方式 A：U 盘拷贝

1. 将 `ToolingBoardTest_arm64_package.tar.gz` 拷入 U 盘。
2. 在开发板上挂载 U 盘（路径因系统而异，示例）：

```bash
# 查看 U 盘设备
lsblk

# 挂载示例（设备名、挂载点按实际修改）
sudo mkdir -p /mnt/usb
sudo mount /dev/sda1 /mnt/usb
```

3. 拷贝到开发板本地目录，建议放到用户主目录或统一测试目录：

```bash
mkdir -p ~/apps
cp /mnt/usb/ToolingBoardTest_arm64_package.tar.gz ~/apps/
sync
```

### 方式 B：scp 从 PC 传到开发板

在 **PC 上**执行（将 IP、用户名、路径换成实际值）：

```bash
scp /path/to/ToolingBoardTest_arm64_package.tar.gz root@192.168.x.x:~/apps/
```

若 `~/apps` 不存在，可先在开发板上创建：

```bash
mkdir -p ~/apps
```

### 方式 C：局域网共享 / 其它工具

使用 NFS、Samba、WinSCP、FileZilla 等，最终保证文件落在开发板上的某一可读路径，例如：

```text
/home/root/apps/ToolingBoardTest_arm64_package.tar.gz
```

---

## 5. 解压

进入存放目录并解压：

```bash
cd ~/apps
tar -xzf ToolingBoardTest_arm64_package.tar.gz
```

解压成功后应出现目录：

```bash
ls -la ToolingBoardTest_arm64_package
```

可看到 `ToolingBoardTest`、`run.sh`、`libs`、`plugins` 等。

> **注意：** 请始终在解压出的目录内运行，不要只拷贝单个 `ToolingBoardTest` 可执行文件；缺少 `libs/`、`plugins/` 会导致启动失败。

---

## 6. 赋权

为启动脚本和主程序添加可执行权限：

```bash
cd ~/apps/ToolingBoardTest_arm64_package

chmod +x ToolingBoardTest
chmod +x run.sh run_wayland.sh run_xcb.sh iot_start.sh
```

一键写法：

```bash
cd ~/apps/ToolingBoardTest_arm64_package
chmod +x ToolingBoardTest run.sh run_wayland.sh run_xcb.sh iot_start.sh
```

---

## 7. 运行 ToolingBoardTest

### 7.1 推荐方式（自动选择显示平台）

```bash
cd ~/apps/ToolingBoardTest_arm64_package
./run_wayland.sh
```

`run_wayland.sh` 会：

1. 设置 `LD_LIBRARY_PATH` 指向本目录 `libs/`
2. 设置 `QT_PLUGIN_PATH` 指向本目录 `plugins/`
3. 自动选择 `wayland`
4. 启动 `./ToolingBoardTest`

启动时终端会打印类似信息：

```text
[run] LD_LIBRARY_PATH=.../libs
[run] QT_PLUGIN_PATH=.../plugins
[run] QT_QPA_PLATFORM=wayland
```

### 7.2 使用 root（仅当权限不足时）

若串口或显示权限不足，可按现场规范使用：

```bash
cd ~/apps/ToolingBoardTest_arm64_package
sudo ./run_wayland.sh
```

---

## 8. 一键操作清单（最短路径）

在开发板上依次执行（路径按实际修改）：

```bash
# 1) 进入存放目录
cd ~/apps

# 2) 解压
tar -xzf ToolingBoardTest_arm64_package.tar.gz

# 3) 进入目录并赋权
cd ToolingBoardTest_arm64_package
chmod +x ToolingBoardTest run.sh run_wayland.sh run_xcb.sh iot_start.sh

# 4) 启动
./run_wayland.sh
```

---

## 9. 常见问题排查

### 9.1 提示 `Permission denied`

```bash
chmod +x ./ToolingBoardTest ./run_wayland.sh
```

### 9.2 提示找不到共享库 / `error while loading shared libraries`

请用 `./run_wayland.sh` 启动，不要直接 `./ToolingBoardTest`（除非已手动 export 库路径）。

确认 `libs` 目录存在：

```bash
ls libs | head
```

并确认当前会话有图形环境：

```bash
echo $DISPLAY $WAYLAND_DISPLAY
```

### 9.3 架构不匹配（Exec format error）

说明机器不是 aarch64，或拷错了包。用 `uname -m` 与 `file ToolingBoardTest` 核对。

### 9.4 查看运行日志

```bash
cd ~/apps/ToolingBoardTest_arm64_package
ls logs/
# 按日期查看，例如：
tail -f logs/2026-08-25.log
```

---

## 10. 停止程序

- 若在前台运行：在终端按 `Ctrl + C`
- 若在后台运行：

```bash
ps | grep ToolingBoardTest
kill <进程号>
# 或
pkill ToolingBoardTest
```
---