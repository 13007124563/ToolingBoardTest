# 米尔ARM64交叉编译说明文档

## 环境要求

1. **米尔交叉编译工具链**：FSL IMX XWayland 6.1-mickledore
   - 默认路径：`/opt/fsl-imx-xwayland/6.1-mickledore`
   - 架构：ARM64 (aarch64)

2. **Qt版本**：确保使用的是针对ARM64交叉编译的Qt版本

## 编译步骤

### 方法一：使用构建脚本（推荐）

```bash
# 1. 赋予执行权限
chmod +x build_myir_arm64.sh

# 2. 执行构建脚本
./build_myir_arm64.sh
```

### 方法二：手动编译

```bash
# 1. 设置交叉编译环境
source /opt/fsl-imx-xwayland/6.1-mickledore/environment-setup-armv8a-poky-linux

# 2. 清理旧的构建文件
make distclean
rm -f Makefile*

# 3. 生成Makefile
qmake ToolingBoardTest.pro

# 4. 编译
make -j4
```

## 配置说明

### 关键修改点

1. **ToolingBoardTest.pro** - 添加了ARM64交叉编译支持：
   - `_FILE_OFFSET_BITS=64`：使用64位文件偏移
   - `_TIME_BITS=64`：使用64位时间类型（解决timesize-32.h缺失问题）
   - ARM64架构优化标志

2. **平台区分**：
   - Windows平台：使用ASIO库进行串口通信
   - Linux平台：使用QProcess调用shell脚本，不依赖ASIO

3. **库链接**：
   - `-lpthread`：多线程支持
   - `-lm`：数学库
   - `-ldl`：动态链接库

## 常见问题

### 1. timesize-32.h 文件找不到

**原因**：ARM64架构不应该使用32位时间类型

**解决**：已在.pro文件中添加 `_TIME_BITS=64` 定义

### 2. qmake版本不对

**检查**：
```bash
which qmake
qmake -version
```

确保使用的是交叉编译环境中的qmake

### 3. 编译器找不到

**检查环境变量**：
```bash
echo $CC
echo $CXX
```

确保已正确执行 `source environment-setup-armv8a-poky-linux`

## 部署到目标设备

```bash
# 1. 找到编译生成的可执行文件
find . -name "ToolingBoardTest" -type f -executable

# 2. 复制到目标设备（根据实际情况修改IP地址）
scp ToolingBoardTest root@192.168.1.100:/home/root/

# 3. 复制配置文件和资源文件
scp -r Win32/Release/config.json root@192.168.1.100:/home/root/
scp -r Win32/Release/*.qm root@192.168.1.100:/home/root/
```

## 注意事项

1. 确保Qt库在目标设备上已正确安装
2. 确保所需的共享库(.so文件)在目标设备上可用
3. 翻译文件(.qm)需要一起部署到目标设备
4. config.json配置文件需要根据目标环境调整

## 验证编译结果

```bash
# 查看可执行文件架构
file ToolingBoardTest

# 应该显示类似：
# ToolingBoardTest: ELF 64-bit LSB executable, ARM aarch64, ...

# 查看动态库依赖
readelf -d ToolingBoardTest | grep NEEDED
```
