# BG95 模块集成说明

## ✅ 集成状态总结

**BG95 模块（串口版）已完全集成到测试程序中**，支持一键测试、结果显示和数据库记录。

> **注意**：界面显示为"BG95"，实际传递给脚本的模块名称为"BG95-SERIAL"

---

## 📋 功能验证清单

### 1. ✅ 一键测试功能

**实现位置**：`MainWnd.cpp:636-725` (on_btn_nor_all_test_clicked)

**工作流程**：
```
用户选择"BG95"模块 
  ↓
验证输入（模块类型、APN、NET地址）
  ↓
调用 executeIotScript("BG95-SERIAL", apn, net)  // 内部映射
  ↓
执行脚本：./iot_start.sh -m BG95-SERIAL -a <apn> -ping <net>
  ↓
脚本执行完整测试流程
```

**支持的参数传递**：
- `-m BG95-SERIAL`：指定模块类型
- `-a <apn>`：指定APN（如 cmnet）
- `-ping <net>`：指定网络测试地址（如 8.8.8.8）

---

### 2. ✅ 测试结果显示

#### 2.1 测试关键项输出框（实时显示）

**实现位置**：`MainWnd.cpp:1405-1570` (parseLogLineAndUpdateUI)

**显示内容**：
| 测试项 | 数据源 | 界面控件 |
|--------|-------|---------|
| 镜像版本 | 日志关键字："【镜像版本】" | lb_test_version |
| IOT模块版本 | 日志关键字："固件版本：" | lb_test_iot_module_ver |
| IMEI | 日志关键字："IMEI：" | lb_test_iot_imei |
| SIM卡ICCID | 日志关键字："SIM卡ICCID：" | lb_test_iccid |
| 网络状态 | 日志关键字："SIM卡检测通过" | lb_test_sim_network |
| 信号强度 | 结果文件：IOT_CSQ | lb_test_rssi |
| 网络制式 | 结果文件：IOT_ACT | lb_test_network_type |

#### 2.2 测试过程显示

**实现位置**：`MainWnd.cpp:1254-1310` (monitorLogFile)

**工作原理**：
- 每500ms读取一次 `/var/log/iot_start.log`
- 增量读取新内容并显示在界面上
- 支持实时查看测试进度

**显示的日志包括**：
```
[2026-02-27 14:30:00] [INFO] [BG95-SERIAL] == 开始 BG95-SERIAL 模块测试 ==
[2026-02-27 14:30:01] [SUCCESS] [BG95-SERIAL] 模块基础通信正常
[2026-02-27 14:30:02] [SUCCESS] [BG95-SERIAL] IMEI：123456789012345
[2026-02-27 14:30:03] [SUCCESS] [BG95-SERIAL] SIM卡ICCID：89860000000000000000
[2026-02-27 14:30:05] [INFO] [BG95-SERIAL] == BG95-SERIAL 模块测试完成（关键项通过） ==
[2026-02-27 14:30:06] [INFO] [BG95-SERIAL] 【BG95-SERIAL PPP拨号流程开始】
```

---

### 3. ✅ 测试记录保存

**实现位置**：`MainWnd.cpp:1354-1404` (saveTestRecordToDatabase)

**保存逻辑**：
1. **SIM卡测试记录**（如果有ICCID）
   - 测试类型：`ETestType_Sim`
   - 包含字段：ICCID、网络状态、信号强度等
   
2. **IOT测试记录**（如果有IMEI）
   - 测试类型：`ETestType_Iot`
   - 包含字段：IMEI、IOT版本、模块类型、测试日志等

**记录结构**（CommonType.h:185-210）：
```cpp
record_.module_type = "BG95-SERIAL";  // 模块类型
record_.iot_module_id = "BG95M3_R03A09";  // IOT版本
record_.iot_imei = "123456789012345";  // IMEI
record_.iccid = "89860000000000000000";  // SIM卡ICCID
record_.net_status = ESimNetStatus_Success;  // 网络状态
record_.signal_strength = "-75 dBm";  // 信号强度
record_.network_type = "LTE (4G)";  // 网络制式
record_.test_log = "完整的测试日志内容...";  // 测试日志
record_.version = "V1.0.1.260303";  // 镜像版本
```

---

### 4. ✅ 数据库查询

**查询入口**：后台管理界面 → SIM/IOT测试记录页面

**支持的查询条件**：
- 模块类型：`BG95-SERIAL`（数据库存储值）
- ICCID：SIM卡序列号
- IMEI：模块序列号
- 测试时间范围
- 测试结果（成功/失败）

**查询实现**：
- SIM记录：根据 `test_type = ETestType_Sim` 过滤
- IOT记录：根据 `test_type = ETestType_Iot` 过滤
- 流水号自动生成：`TestRecordManager::GeneratedRecordID()`

---

## 🔧 脚本集成详情

### BG95-SERIAL 脚本支持

**脚本路径**：`iot_start.sh`

**关键配置**（Line 134-145）：
```bash
check_at_port() {
    if [ $MODULE_NAME == "BG95-SERIAL" ]; then
        AT_PORT=/dev/ttyLP6  # 串口版使用ttyLP6
    fi
}
```

**测试流程**（Line 431-549）：
1. 基础通信测试（AT指令）
2. 查询IMEI
3. 查询固件版本
4. 设置APN
5. 查询运营商和网络制式
6. SIM卡检测（ICCID、PIN状态、网络注册）
7. 查询信号强度

**拨号流程**（Line 935-1007 + 1097）：
```bash
run_dial() {
    if [ $MODULE_NAME == "BG95-SERIAL" ]; then
        ln -s /etc/ppp/resolv.conf /etc/resolv.conf
        ppp_dial  # PPP拨号
    fi
}
```

**PPP拨号特性**：
- 支持移动/联通/电信三大运营商
- 支持PAP/CHAP/无认证
- 自动生成chat脚本
- 监控拨号状态
- 网络接口：ppp0

---

## 📊 测试数据流

```
iot_start.sh 脚本执行
    ↓
生成测试结果文件（/tmp/dev_info/）
    ├── IMAGE_VERSION      → 镜像版本
    ├── IOT_VERSION        → 固件版本
    ├── IOT_IMEI           → IMEI
    ├── IOT_CCID           → ICCID
    ├── IOT_CREG           → 网络注册状态
    ├── IOT_CSQ            → 信号强度
    ├── IOT_ACT            → 网络制式
    └── IOT_OPER           → 运营商
    ↓
程序读取文件（readScriptResults）
    ↓
更新界面显示（parseLogLineAndUpdateUI）
    ↓
保存数据库（saveTestRecordToDatabase）
    ├── SIM测试记录（test_type=1）
    └── IOT测试记录（test_type=2）
```

---

## 🎯 使用说明

### 测试操作步骤

1. **选择模块类型**
   - 下拉框选择：`BG95`（界面显示）
   - 实际传递参数：`BG95-SERIAL`（脚本参数）

2. **输入APN**
   - 移动：cmnet / cmiot
   - 联通：3gnet / uninet
   - 电信：ctnet

3. **输入NET测试地址**
   - 推荐：8.8.8.8（Google DNS）
   - 或：114.114.114.114（国内DNS）

4. **点击"一键测试"**
   - 等待测试完成（约30-60秒）
   - 查看测试过程输出
   - 查看测试关键项结果

5. **查看测试记录**
   - 进入后台管理 → SIM测试记录
   - 进入后台管理 → IOT测试记录
   - 根据ICCID或IMEI搜索

---

## ⚠️ 注意事项

### 环境要求

1. **硬件连接**
   - BG95模块通过串口连接到 /dev/ttyLP6
   - 确保GPIO配置正确（111/113/116/123）

2. **系统依赖**
   - pppd：PPP拨号工具
   - socat：AT指令发送工具
   - /etc/ppp/peers/quectel-dial：拨号配置文件
   - /etc/ppp/chatscripts/quectel-chat-connect：chat脚本

3. **权限要求**
   - 脚本需要root权限执行
   - 确保脚本有执行权限：`chmod +x iot_start.sh`

### 常见问题排查

**Q1: 测试结果不显示？**
- 检查脚本是否正确执行：`ps aux | grep iot_start.sh`
- 检查日志文件：`tail -f /var/log/iot_start.log`
- 检查结果文件：`ls -la /tmp/dev_info/`

**Q2: 数据库未保存记录？**
- 查看程序日志中的保存提示
- 确认测试完成标志出现："模块测试完成（关键项通过）"
- 检查IMEI和ICCID是否成功读取

**Q3: 拨号失败？**
- 检查SIM卡状态
- 检查APN配置
- 查看PPP日志：`tail -f /var/log/quectel-dial.log`

---

## 🔄 与其他模块对比

| 模块 | 连接方式 | AT端口 | 网络接口 | 拨号方式 | 集成状态 |
|------|---------|--------|---------|---------|---------|
| ME3630 | USB | /dev/ttyUSB1 | usb0 | ECM | ✅ 完全支持 |
| EC200U | USB | /dev/ttyUSB0 | usb0 | quectel-CM | ✅ 完全支持 |
| EG21 | USB | /dev/ttyUSB2 | wwan0 | quectel-CM | ✅ 完全支持 |
| **BG95** | **串口** | **/dev/ttyLP6** | **ppp0** | **PPP拨号** | ✅ **完全支持** |

> **说明**：界面显示为"BG95"，内部使用"BG95-SERIAL"作为脚本参数

---

## ✅ 验证清单

- [x] 下拉框显示 BG95 选项
- [x] 内部正确映射到 BG95-SERIAL 参数
- [x] 脚本接收正确的模块名称参数
- [x] 脚本执行BG95-SERIAL专属测试流程
- [x] 测试结果文件正确生成
- [x] 界面实时显示测试过程
- [x] 界面正确显示测试关键项
- [x] 测试完成后自动保存数据库
- [x] SIM测试记录可查询
- [x] IOT测试记录可查询
- [x] 完整测试日志保存

---

## 📝 总结
 模块已完全集成**，所有功能均已实现：

1. ✅ **可以进行一键测试**（界面选择"BG95"）
2. ✅ **测试结果和测试过程会在首页的测试关键项输出框和测试过程显示中显示**
3. ✅ **测试记录能在后台SIM和IOT测试界面中搜索出来**（搜索时使用"BG95-SERIAL"）
4. ✅ **确认对话框图片显示优化**（从180x180调整为120x120）

**界面显示与内部处理**：
- 用户界面：显示"BG95"（简洁易懂）
- 脚本参数：传递"BG95-SERIAL"（兼容脚本）
- 数据库存储：保存"BG95-SERIAL"（便于查询识别）程显示中显示**
3. ✅ **测试记录能在后台SIM和IOT测试界面中搜索出来**

**无需额外开发，直接使用即可！**
