# 测试治具串口协议自动化

与协议文档 `YF0226C220054-V1.1-...B1024-01固件.md` 3.1–3.7 对齐。

## 文件

| 文件 | 作用 |
|------|------|
| `protocol_test.py` | CRC 校验 + 回包解析对齐测试（Python） |
| `coolterm_auto_reply.applescript` | Mac CoolTerm 自动识别请求并回包（联调模拟测试板） |

## 1. Python 对齐测试

```bash
python3 scripts/protocol_test.py
```

全部 `[PASS]` 表示文档示例帧 CRC/长度正确，解析结果与 C++ `ResponseParser` 逻辑一致。

## 2. CoolTerm 自动回包（Mac）

1. CoolTerm → Preferences → **Enable AppleScript**
2. 连接 FTDI，9600 8-N-1
3. 脚本编辑器打开 `scripts/coolterm_auto_reply.applescript` 并运行
4. 开发板程序打开串口，在下拉框选择命令后点 **Send Query**

### 支持的命令

| 章节 | CMD | 请求示例 | 回包（文档示例） |
|------|-----|----------|------------------|
| 3.1 | 0x01 | `02 7F 08 01 01 FF 06 43` | 版本 `V1.0.0.260530` |
| 3.2 | 0x02 | `02 7F 08 01 02 FF 06 B3` | 12V/5V/3.3V 三路电压 |
| 3.3 | 0x03 | `02 7F 09 01 03 FF 01 9E C2` | CN43 打印机电源（读高电平） |
| 3.4 | 0x04 | `02 7F 09 01 04 FF 01 2F 03` | CN39 5V 输出（读高电平） |
| 3.5 | 0x05 | `02 7F 09 01 05 FF 01 7E C3` | CN47 12V 输出（读高电平） |
| 3.6 | 0x06 | `02 7F 09 01 06 FF 01 8E C3` | CN13 接近开关 5V（读高电平） |
| 3.7 | 0x07 | `02 7F 09 01 07 FF 00 1E C3` | ST_INPUT IO 故障标记 |

0x03–0x06 下行 INFO（V1.1）：`00` 读低电平 / `01` 读高电平。  
0x07：先串口 INFO=`01` 置高，I2C 读 INPUT1(CN45)/INPUT2(CN13) 应为 1；再 INFO=`00` 置低，I2C 读应为 0。

## 3. 开发板 UI 预期

发送后在 **测试执行过程** 日志中应看到：

- `[TX] ...` 发送帧
- `[RX match] ...` 匹配回包
- 解析文本，例如：
  - `Test board version: V1.0.0.260530`
  - `VCC_12V (CN52-19): 12.10 V (OK)`
  - `ST_INPUT1/2 IO (CN45/CN13): OK`

0x01 成功时 **测试板版本** 栏会更新；0x02–0x07 在对应结果栏展示，例如：

| 结果栏 | CMD |
|--------|-----|
| Test Board Version | 0x01 |
| VCC 12/5/3.3V (CN52) | 0x02 |
| Printer Power (CN43) | 0x03 |
| 5V Output (CN39) | 0x04 |
| 12V Output (CN47) | 0x05 |
| 5V Proximity (CN13) | 0x06 |
| ST_INPUT IO (CN45/13) | 0x07 |
