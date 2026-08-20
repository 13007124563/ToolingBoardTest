#!/bin/bash
######## 当前脚本已支持模块 (ME3630/EG21/EC200U/BG95-SERIAL(串口版)) ########

# 逻辑：先完成模块全量测试 → 测试通过后自动执行ECM拨号
# Applicable: Global carrier networks, supports custom APN/authentication/PDP type
# Applicable systems: Debian/Ubuntu/CentOS/RHEL
# Usage: sudo ./iot_start.sh [-m MODULE_NAME] [-a APN] [-t PDP_TYPE] [-auth AUTH_TYPE] [-u USER] [-p PWD] [-port AT_PORT] [-i ECM_IFACE] [-ping PING_HOST] [--test-url TEST_URL] [--bearer BEARER]
# Example: sudo ./iot_start.sh -m ME3630-001 -a cmnet -t IP -port /dev/ttyUSB2 -i usb1 -ping 8.8.8.8 --test-url 114.114.114.114

# ===================== 全局默认配置（拨号+测试共用） =====================
# 1. 拨号核心配置
MODULE_NAME="EG21"           # IOT模块名称
APN="internet"               # 运营商APN
PDP_TYPE="IP"            # PDP类型：IP(IPV4)/IPV6/IPV4V6
AUTH_TYPE="none"             # 认证类型：none/pap/chap
APN_USER=""                  # 认证用户名
APN_PWD=""                   # 认证密码
AT_PORT="/dev/ttyUSB1"       # AT指令端口
ECM_IFACE="usb0"             # ECM网络接口
LOG_FILE="/var/log/iot_start.log"
PING_TEST_HOST="8.8.8.8"     # 拨号后连通性测试地址
DEV_INFO_FILE="/tmp/dev_info/"
IMAGE_VERSION_FILE="/etc/sw-version"

DEFAULT_ETH_IP="192.168.1.100"

TEST_TIMEOUT=1               # 测试AT指令超时时间（秒）
TEST_RETRY=3                 # 测试指令重试次数

# Ping监控配置（新增）
INIT_PING_COUNT=20           # 初始ping次数（10次）
INIT_PING_INTERVAL=10        # 初始ping间隔（3秒 → 10×3=30秒）
LONG_PING_INTERVAL=300       # 长期ping间隔（300秒 → 5分钟）
PING_TIMEOUT=10              # 单次ping超时时间（秒）
PING_COUNT=1                 # 单次ping的包数量

# ===================== 通用函数 =====================
# 统一日志输出（带模块名标识）
log() {
    local LEVEL=$1
    local MSG=$2
    local DATETIME=$(date +%Y-%m-%d\ %H:%M:%S)
    echo "[$DATETIME] [$LEVEL] [$MODULE_NAME] $MSG" | tee -a $LOG_FILE > /dev/null
}

# 检测镜像版本（提取version字段：V1.0.1.260126）
check_image_version() {
    #mkdir -p $$DEV_INFO_FILE
    mkdir -p /tmp/dev_info/
    # 读取并解析镜像版本文件
    if [ -f "$IMAGE_VERSION_FILE" ]; then
        # 提取version行的内容（过滤空行+匹配version字段）
        IMAGE_VERSION_RAW=$(grep -o 'V[0-9.]\+' $IMAGE_VERSION_FILE)
        # 若解析到有效版本则赋值，否则为unknown
        if [ -n "$IMAGE_VERSION_RAW" ]; then
            IMAGE_VERSION=$IMAGE_VERSION_RAW
        else
            IMAGE_VERSION="unknown (version字段为空)"
        fi
    else
        IMAGE_VERSION="unknown (文件不存在：$IMAGE_VERSION_FILE)"
    fi

    # 将版本信息写入文件
    echo $IMAGE_VERSION > $DEV_INFO_FILE/IMAGE_VERSION
    log "SUCCESS" "【镜像版本】$IMAGE_VERSION"
}

# 检查root权限
check_root() {
    if [ $EUID -ne 0 ]; then
        log "ERROR" "请使用root权限运行脚本（sudo $0 [OPTIONS]）"
        exit 1
    fi
}

# 兼容新旧内核的 GPIO 控制：
# - 旧内核：全局号多为小数字（如 122）
# - 新内核（gpiochip base>=512）：同一管脚常变为 base+offset
# export 返回 Invalid argument 时，按 gpiochip base 尝试映射号
gpio_write() {
    local LEGACY_NUM=$1
    local VALUE=$2
    local DIRECTION=${3:-out}
    local candidates=("$LEGACY_NUM")
    local chip base ngpio mapped num

    for chip in /sys/class/gpio/gpiochip*; do
        [ -f "$chip/base" ] || continue
        base=$(cat "$chip/base" 2>/dev/null) || continue
        ngpio=$(cat "$chip/ngpio" 2>/dev/null) || ngpio=0
        if [ -n "$base" ] && [ "$LEGACY_NUM" -lt 256 ] && [ "$base" -ge 256 ]; then
            mapped=$((base + LEGACY_NUM))
            if [ "$mapped" -lt $((base + ngpio)) ]; then
                candidates+=("$mapped")
            fi
        fi
    done

    for num in "${candidates[@]}"; do
        if [ ! -d "/sys/class/gpio/gpio${num}" ]; then
            if ! echo "$num" > /sys/class/gpio/export 2>/dev/null; then
                continue
            fi
            sleep 0.05
        fi
        if [ ! -d "/sys/class/gpio/gpio${num}" ]; then
            continue
        fi
        if ! echo "$DIRECTION" > "/sys/class/gpio/gpio${num}/direction" 2>/dev/null; then
            continue
        fi
        if ! echo "$VALUE" > "/sys/class/gpio/gpio${num}/value" 2>/dev/null; then
            continue
        fi
        log "SUCCESS" "GPIO${LEGACY_NUM} -> sysfs gpio${num} = ${VALUE}"
        return 0
    done

    # libgpiod 兜底（按线路名查找）
    if command -v gpiofind >/dev/null 2>&1 && command -v gpioset >/dev/null 2>&1; then
        local line
        for line in "W_n_DISABLE" "wn_disable" "gpio${LEGACY_NUM}"; do
            if spec=$(gpiofind "$line" 2>/dev/null); then
                if gpioset ${spec}=${VALUE} 2>/dev/null; then
                    log "SUCCESS" "GPIO${LEGACY_NUM} controlled via gpioset ($line)=$VALUE"
                    return 0
                fi
            fi
        done
    fi

    log "WARN" "无法控制 GPIO${LEGACY_NUM}（候选: ${candidates[*]}），请检查内核 GPIO 号或设备树"
    return 1
}

check_env() {
    rm -rf /etc/udhcpc.d/50default
    ln -s /etc/zl_test/50default /etc/udhcpc.d/50default

    # 若 AT 口已存在，认为模组已上电，跳过拉 GPIO，避免错误 GPIO 号导致失败
    if [ -c /dev/ttyUSB2 ] || [ -c /dev/ttyUSB1 ] || [ -c /dev/ttyUSB0 ]; then
        log "INFO" "检测到 USB AT 口已存在，跳过 W_n_DISABLE(GPIO122) 复位"
        return 0
    fi

    #W_n_DISABLE：拉低再拉高，复位/使能模组相关电源
    log "INFO" "通过 GPIO122(W_n_DISABLE) 复位模组电源..."
    if gpio_write 122 0 out; then
        sleep 1
        gpio_write 122 1 out
        sleep 2
    else
        log "ERROR" "GPIO122 控制失败，模组可能无法上电，后续 /dev/ttyUSBx 可能不出现"
        log "ERROR" "请在板端执行: ls /sys/class/gpio/gpiochip*; cat /sys/class/gpio/gpiochip*/base"
    fi
}

check_bg95_env() {
    log "INFO" "重启BG95模块"
    #IOT_ON_OFF
    if [ ! -d "/sys/class/gpio/gpio111/" ]; then
        echo 111 > /sys/class/gpio/export
        echo out > /sys/class/gpio/gpio111/direction
    fi
    echo 1 > /sys/class/gpio/gpio111/value

    #IOT_EINT_L
    if [ ! -d "/sys/class/gpio/gpio113/" ]; then
        echo 113 > /sys/class/gpio/export
        echo in > /sys/class/gpio/gpio113/direction
    fi

    #IOT_DTR_L
    if [ ! -d "/sys/class/gpio/gpio116/" ]; then
        echo 116 > /sys/class/gpio/export
        echo out > /sys/class/gpio/gpio116/direction
    fi
    echo 0 > /sys/class/gpio/gpio116/value

    #IOT_nRESET
    if [ ! -d "/sys/class/gpio/gpio123/" ]; then
        echo 123 > /sys/class/gpio/export
        echo out > /sys/class/gpio/gpio123/direction
    fi
    echo 1 > /sys/class/gpio/gpio123/value

    sleep 1

    echo 0 > /sys/class/gpio/gpio111/value
    echo 0 > /sys/class/gpio/gpio123/value
    echo 1 > /sys/class/gpio/gpio116/value
    sleep 1
}

# 检查AT端口是否存在
check_at_port() {
    if [ $MODULE_NAME == "ME3630" ]; then
        AT_PORT=/dev/ttyUSB1
    elif [ $MODULE_NAME == "EC200U" ]; then
        AT_PORT=/dev/ttyUSB0
    elif [ $MODULE_NAME == "EG21" ]; then
        AT_PORT=/dev/ttyUSB2
    elif [ $MODULE_NAME == "BG95" ]; then
        AT_PORT=/dev/ttyUSB2
    elif [ $MODULE_NAME == "BG95-SERIAL" ]; then
        AT_PORT=/dev/ttyLP6
    fi
    wait_ecm_interface
}

# 显示帮助信息
print_help() {
    echo "模块测试+拨号一体化脚本"
    echo "用法：sudo $0 [可选参数]"
    echo "核心逻辑：先执行全量测试 → 测试通过后自动拨号"
    echo ""
    echo "可选参数（拨号相关）："
    echo "  -m <MODULE>       设置模块名称（默认：ME3630）"
    echo "  -a <APN>          设置拨号APN（默认：internet）"
    echo "  -t <PDP_TYPE>     设置PDP类型（IP/IPV6/IPV4V6，默认：IPV4V6）"
    echo "  -auth <TYPE>      设置认证类型（none/pap/chap，默认：none）"
    echo "  -u <USER>         设置APN认证用户名（默认：空）"
    echo "  -p <PWD>          设置APN认证密码（默认：空）"
    echo "  -port <PORT>      设置AT指令端口（默认：/dev/ttyUSB1）"
    echo "  -i <IFACE>        设置ECM网络接口（默认：usb0）"
    echo "  -ping <HOST>      设置拨号后测试地址（默认：8.8.8.8）"
    echo ""
    echo "  -h/--help         显示帮助信息"
    echo ""
    echo "示例："
    echo "  sudo $0 -m ME3630 -a cmnet --ping 114.114.114.114"
    exit 0
}

send_at_cmd_test() {
    local CMD=$1
    local EXPECT_RESP=$2

    if [ -z "$CMD" ] || [ -z "$EXPECT_RESP" ]; then
        log "ERROR" "参数错误：CMD或EXPECT_RESP为空"
        echo "ERROR"
        return 1
    fi
    if [ -z "$TEST_TIMEOUT" ] || [ -z "$AT_PORT" ] || [ -z "$TEST_RETRY" ]; then
        log "ERROR" "全局变量未定义：TEST_TIMEOUT/AT_PORT/TEST_RETRY"
        echo "ERROR"
        return 1
    fi
    # 校验串口是否存在
    if [ ! -c "$AT_PORT" ]; then
        log "ERROR" "串口设备不存在：$AT_PORT"
        echo "ERROR"
        return 1
    fi

    local RESP_FILE=$(mktemp)
    log "INFO" "【测试指令】AT$CMD，期望响应：$EXPECT_RESP，串口：$AT_PORT，超时：$TEST_TIMEOUT秒"

    for ((i=1; i<=TEST_RETRY; i++)); do
        # 1. 先释放串口锁（防止残留）
        fuser -k $AT_PORT >/dev/null 2>&1
        sleep 0.5
        
        # 发送AT指令
        echo -e "AT$CMD\r" | socat -t $TEST_TIMEOUT $AT_PORT,raw,echo=0,b115200 - > $RESP_FILE 2>>$LOG_FILE
        
        # 读取原始响应并标准化换行
        local RAW_RESP=$(cat $RESP_FILE | tr -d '\r' | sed '/^[ \t]*$/d')
        log "DEBUG" "第$i次原始响应：$RAW_RESP"

        # 1. 完全删除 以AT开头+当前指令 的整行回显，转义正则元字符 ? * [ ] . ^ $
        local CMD_ESC=$(echo "$CMD" | sed 's/[?*.^$\[\]]/\\&/g')
        # 2. 删除AT指令回显行 + 首尾空行 + 纯符号垃圾行
        RESP=$(echo "$RAW_RESP" \
            | sed "/^AT\s*$CMD_ESC\s*$/d" \
            | sed '/^[?]*$/d' \
            | sed '/^[ \t]*$/d' \
            | sed '/^$/d')

        # 宽松匹配期望字符
        if echo "$RESP" | grep -qi "$EXPECT_RESP"; then
            rm -f $RESP_FILE
            log "SUCCESS" "指令执行成功：AT$CMD，响应：$RESP"
            echo "$RESP"
            return 0
        fi
        log "WARN" "测试指令重试：AT$CMD（第 $i 次），未匹配到期望响应：$EXPECT_RESP"
        sleep 2
    done

    rm -f $RESP_FILE
    log "ERROR" "测试指令执行失败：AT$CMD，最后清理后响应：$RESP"
    echo "ERROR"
    return 1
}

# 解析信号强度（CSQ值转文字描述）
parse_csq() {
    local CSQ_RESP=$1
    local RSSI=$(echo $CSQ_RESP | grep -oP '\+CSQ: \K\d+' | head -1)
    echo "$RSSI" > $DEV_INFO_FILE/IOT_CSQ
    # 按你要求的CSQ→dBm映射规则转换
    if [ $RSSI -eq 0 ]; then
        dBm="-113 dBm 及以下"
    elif [ $RSSI -eq 1 ]; then
        dBm="-111 dBm"
    elif [ $RSSI -ge 2 ] && [ $RSSI -le 30 ]; then
        dBm=$((-113 + 2*RSSI))" dBm"
    elif [ $RSSI -eq 31 ]; then
        dBm="-51 dBm 及以上"
    elif [ $RSSI -eq 99 ]; then
        dBm="未知/不可测"
    else
        dBm="无效值($RSSI)"
    fi
    log "SUCCESS" "信号强度CSQ：$RSSI | 精准dBm：$dBm"
}

# 解析网络注册状态
parse_cgreg() {
    local CGREG_RESP=$1
    local REG_STATE=$(echo "$CGREG_RESP" | grep -oP '\+CGREG: \d+,\K\d+')
    echo "$REG_STATE" > $DEV_INFO_FILE/IOT_CGREG
    case $REG_STATE in
        0) echo "网络注册：未注册，模块未搜索运营商" ;;
        1) echo "网络注册：已注册（本地网络）" ;;
        2) echo "网络注册：搜索中" ;;
        3) echo "网络注册：注册被拒绝（SIM卡欠费/失效/运营商不匹配）" ;;
        4) echo "网络注册：未知" ;;
        5) echo "网络注册：已注册（漫游）" ;;
        *) echo "网络注册：解析失败" ;;
    esac
}

# 解析APN配置信息
parse_apn() {
    local CGDCONT_RESP=$1
    local APN_LINE=$(echo "$CGDCONT_RESP" | grep -oP '\+CGDCONT: .+')
    if [ -z "$APN_LINE" ]; then
        echo "APN配置：未查询到有效配置"
        return
    fi

    # 解析承载号/协议/APN名称/IP
    local BEARER_ID=$(echo "$APN_LINE" | cut -d',' -f1 | grep -oP '\d+')
    local PROTOCOL=$(echo "$APN_LINE" | cut -d',' -f2 | tr -d '"')
    local APN_NAME=$(echo "$APN_LINE" | cut -d',' -f3 | tr -d '"')
    local IP_ADDR=$(echo "$APN_LINE" | cut -d',' -f4 | tr -d '"')

    # 格式化输出
    if [ -z "$APN_NAME" ]; then
        echo "APN配置：承载$BEARER_ID，协议=$PROTOCOL，APN名称=未配置，IP地址=$IP_ADDR"
    else
        echo "APN配置：承载$BEARER_ID，协议=$PROTOCOL，APN名称=$APN_NAME，IP地址=$IP_ADDR"
    fi
}

# 设置测试APN配置
set_apn_test() {
    local APN=$1
    local BEARER=$2
    log "INFO" "开始设置测试APN：承载$BEARER，APN名称=$APN"
    local SET_CMD="+CGDCONT=$BEARER,\"IP\",\"$APN\""
    local SET_RESP=$(send_at_cmd_test "$SET_CMD" "OK")
    
    if [ "$SET_RESP" != "ERROR" ]; then
        log "SUCCESS" "测试APN设置成功：承载$BEARER，APN=$APN"
        return 0
    else
        log "ERROR" "测试APN设置失败：承载$BEARER，APN=$APN"
        return 1
    fi
}

# SIM卡全方位检测（关键步骤，失败则终止）
detect_sim() {
    local SIM_STATUS="未知"

    # 1. 检测SIM卡锁状态
    CPIN_RESP=$(send_at_cmd_test "+CPIN?" "OK")
    if [ "$CPIN_RESP" != "ERROR" ]; then
        echo "$CPIN_RESP" > $DEV_INFO_FILE/IOT_CPIN
        if echo "$CPIN_RESP" | grep -qi "READY"; then
            log "SUCCESS" "SIM卡锁状态：未锁定（READY）"
        elif echo "$CPIN_RESP" | grep -qi "PIN"; then
            log "ERROR" "SIM卡锁状态：需要输入PIN码（请执行 AT+CPIN=<PIN码> 解锁）"
            SIM_STATUS="锁定"
        elif echo "$CPIN_RESP" | grep -qi "PUK"; then
            log "ERROR" "SIM卡锁状态：需要输入PUK码（SIM卡已锁定，需运营商解锁）"
            SIM_STATUS="锁定"
        else
            log "WARN" "SIM卡锁状态：未知响应 - $CPIN_RESP"
        fi
    else
        log "ERROR" "查询SIM卡锁状态失败"
        SIM_STATUS="检测失败"
    fi

    # 2. 检测SIM卡序列号（ICCID）
    if [ $MODULE_NAME == "ME3630" ]; then
        CCID_RESP=$(send_at_cmd_test "+ZGETICCID" "OK")
    else
        CCID_RESP=$(send_at_cmd_test "+QCCID" "OK")
    fi

    if [ "$CCID_RESP" != "ERROR" ]; then
        if [ $MODULE_NAME == "ME3630" ]; then
            # 提取纯ICCID内容，去除指令头和空格
            CCID_RAW=$(echo "$CCID_RESP" | grep -v 'OK' | tr -d ' ' | sed 's/^+ZGETICCID://i')
        else
            CCID_RAW=$(echo "$CCID_RESP" | grep -v 'OK' | tr -d ' ' | sed 's/^+QCCID://i')
        fi
        
        # 修复：正则支持数字+大小写字母（适配含字母的ICCID），长度放宽至18-22位
        CCID=$(echo "$CCID_RAW" | grep -E '^[0-9A-Fa-f]{18,22}$')
        if [ -n "$CCID" ]; then
            log "SUCCESS" "SIM卡ICCID：$CCID（SIM卡在位且有效）"
            echo "$CCID" > $DEV_INFO_FILE/IOT_CCID
            SIM_STATUS="正常"
        else
            log "WARN" "SIM卡ICCID格式不标准（响应：$CCID_RESP），暂不判定为无效"
            SIM_STATUS="待验证"
        fi
    else
        log "WARN" "查询SIM卡ICCID失败，但不终止脚本（优先以网络注册状态为准）"
        SIM_STATUS="待验证"
    fi

    # 3. 辅助检测：网络注册状态
    CREG_RESP=$(send_at_cmd_test "+CREG?" "OK")
    if [ "$CREG_RESP" != "ERROR" ]; then
        CREG_STATE=$(echo "$CREG_RESP" | grep -oP '\+CREG: \d+,\K\d+')
        echo "$CREG_STATE" > $DEV_INFO_FILE/IOT_CREG
        case $CREG_STATE in
            0) CREG_DESC="未注册；模块当前未搜索要注册的运营商" ;;
            1) 
                CREG_DESC="已注册，归属地网络（正常状态）"
                SIM_STATUS="正常"  # 强制标记为正常，忽略ICCID解析异常
                ;;
            2) CREG_DESC="未注册，模块正在搜索要注册的运营商" ;;
            3) 
                CREG_DESC="注册被拒绝（可能欠费/失效/运营商不匹配）"
                log "ERROR" "辅助判断：SIM卡注册被拒绝（可能欠费/失效/运营商不匹配）"
                SIM_STATUS="注册被拒"
                ;;
            4) CREG_DESC="未知状态" ;;
            5) 
                CREG_DESC="已注册，漫游网络"
                SIM_STATUS="正常"  # 漫游注册成功也标记为正常
                ;;
            *) CREG_DESC="无效状态值($CREG_STATE)" ;;
        esac
        log "INFO" "SIM卡注册状态：$CREG_DESC"
    fi

    # 输出最终SIM卡状态并判断是否继续
    log "INFO" "SIM卡最终状态：$SIM_STATUS"
    if [ "$SIM_STATUS" = "正常" ] || [ "$SIM_STATUS" = "待验证" ]; then
        log "SUCCESS" "SIM卡检测通过：在位、未锁定、有效"
        return 0
    else
        log "ERROR" "SIM卡检测失败：状态=$SIM_STATUS，终止脚本"
        return 1
    fi
}

kill_iot_start_process() {
    log "INFO" "== 清理旧拨号进程 =="

    # 获取当前脚本PID，避免误杀自己
    CURRENT_PID=$$

    # 1. 清理iot_start.sh进程（排除自身）
    PROCESS_IDS=$(pgrep -f "iot_start.sh" 2>/dev/null | grep -v "^${CURRENT_PID}$" || ps -ef | grep -v grep | grep -v "^[^ ]* *${CURRENT_PID} " | grep "iot_start.sh" | awk '{print $2}')
    if [ -n "$PROCESS_IDS" ]; then
        log "INFO" "查杀iot_start.sh进程: $PROCESS_IDS"
        kill -9 $PROCESS_IDS >/dev/null 2>&1
    fi

    # 2. 直接清理quectel-CM进程（简洁版）
    QC_PROCESS_IDS=$(pgrep -f "quectel-CM" 2>/dev/null || ps -ef | grep -v grep | grep "quectel-CM" | awk '{print $2}')
    if [ -n "$QC_PROCESS_IDS" ]; then
        log "INFO" "查杀quectel-CM进程: $QC_PROCESS_IDS"
        kill -9 $QC_PROCESS_IDS >/dev/null 2>&1
    fi

    # 统一提示清理完成
    log "SUCCESS" "旧拨号进程清理完成"
}

# 模块全量测试（核心测试逻辑，失败则终止）
run_full_test() {
    log "INFO" "== 开始 $MODULE_NAME 模块测试 =="

    # 1. 测试模块基础通信（关键，失败直接终止）
    log "INFO" "========== 1. 测试模块基础通信 =========="
    BASIC_RESP=$(send_at_cmd_test "I" "OK")
    if [ "$BASIC_RESP" = "ERROR" ]; then
        log "ERROR" "模块基础通信失败（无法响应AT指令），终止测试和拨号"
        exit 1
    fi
    log "SUCCESS" "模块基础通信正常"

    # 2. 查询模块IMEI
    log "INFO" "========== 2. 查询模块IMEI =========="
    IMEI_RESP=$(send_at_cmd_test "+CGSN" "OK")
    if [ "$IMEI_RESP" != "ERROR" ]; then
        IMEI=$(echo "$IMEI_RESP" | grep -v 'OK' | tr -d ' ')
        echo "$IMEI" > $DEV_INFO_FILE/IOT_IMEI
        log "SUCCESS" "IMEI：$IMEI"
    else
        log "ERROR" "查询IMEI失败（非关键错误，继续测试）"
    fi

    # 4. 查询IOT版本
    log "INFO" "========== 3. 查询IOT版本 =========="
    if [ "$MODULE_NAME" = "ME3630" ]; then
        FW_RESP=$(send_at_cmd_test "+CGMR" "OK")
    else
        FW_RESP=$(send_at_cmd_test "+QGMR" "OK")
    fi
    if [ "$FW_RESP" != "ERROR" ]; then
        FW_VERSION=$(echo "$FW_RESP" | grep -v 'OK' | tr -d ' ')
        echo "$FW_VERSION" > $DEV_INFO_FILE/IOT_VERSION
        log "SUCCESS" "固件版本：$FW_VERSION"
    else
        log "ERROR" "查询固件版本失败（非关键错误，继续测试）"
    fi

    log "INFO" "========== 4. 设置APN =========="
    config_apn

    # 7. 查询运营商+网络制式
    log "INFO" "========== 5. 查询当前运营商+网络制式 =========="

    # 定义重试参数：最大重试5次，每次休眠3秒
    local MAX_RETRY=20
    local RETRY_INTERVAL=10
    local retry_count=0
    local qnwinfo_resp="ERROR"
    local plmn=""
    local cell_type_code=""

    # 循环获取AT响应，直到PLMN非空或达到最大重试次数
    while [ $retry_count -lt $MAX_RETRY ] && [ -z "$plmn" ]; do
        # 发送AT+QNWINFO指令并获取响应
        if [ $MODULE_NAME == "ME3630" ]; then
            # 优先执行+COPS?并解析（直观的运营商名称/编码）
            qnwinfo_resp=$(send_at_cmd_test "+COPS?" "OK")
            if [ "$qnwinfo_resp" != "ERROR" ]; then
                plmn=$(echo "$qnwinfo_resp" | grep -oP '\+COPS: .*?"\K[^"]+(?=")' | head -1)
            fi

            qnwinfo_resp=$(send_at_cmd_test "+ZCELLINFO?" "OK")
            if [ "$qnwinfo_resp" != "ERROR" ]; then
                cell_type_code=$(echo "$qnwinfo_resp" | grep -oP '\+ZCELLINFO: \w+,\w+,\w+,\K[^,]+' | head -1)
            fi

            # 若PLMN为空且未到最大重试次数，休眠后重试
            if [ -z "$plmn" ] && [ $((retry_count + 1)) -lt $MAX_RETRY ]; then
                retry_count=$((retry_count + 1))
                log "INFO" "PLMN解析为空，开始第${retry_count}次重试（共${MAX_RETRY}次），休眠${RETRY_INTERVAL}秒..."
                cfun_check
                sleep $RETRY_INTERVAL
            else
                break  # PLMN非空或达到最大重试次数，退出循环
            fi
        else
            qnwinfo_resp=$(send_at_cmd_test "+QENG=\"servingcell\"" "OK")
            qnwinfo_resp=$(send_at_cmd_test "+QNWINFO" "OK")
            if [ "$qnwinfo_resp" != "ERROR" ]; then
                # 解析PLMN编码（第二列，如"46011"），优先赋值PLMN用于判断是否重试
                plmn=$(echo "$qnwinfo_resp" | grep -oP '\+QNWINFO: "[^"]+","\K[^"]+' | head -1)
            fi

            # 若PLMN为空且未到最大重试次数，休眠后重试
            if [ -z "$plmn" ] && [ $((retry_count + 1)) -lt $MAX_RETRY ]; then
                retry_count=$((retry_count + 1))
                log "INFO" "PLMN解析为空，开始第${retry_count}次重试（共${MAX_RETRY}次），休眠${RETRY_INTERVAL}秒..."
                cfun_check
                sleep $RETRY_INTERVAL
            else
                break  # PLMN非空或达到最大重试次数，退出循环
            fi
        fi
    done

    log "INFO" "qnwinfo_resp $qnwinfo_resp "
    if [ "$qnwinfo_resp" != "ERROR" ]; then
        if [ $MODULE_NAME != "ME3630" ]; then
            cell_type_code=$(echo "$qnwinfo_resp" | grep -oP '\+QNWINFO: "\K[^"]+' | head -1)
        fi
    fi
    log "SUCCESS" "当前运营商：$plmn | 网络制式：$cell_type_code"
    echo "$plmn" > $DEV_INFO_FILE/IOT_OPER
    echo "$cell_type_code" > $DEV_INFO_FILE/IOT_ACT

    # 5. SIM卡检测（关键，失败则终止）
    log "INFO" "========== 6. SIM卡状态检测 =========="
    detect_sim

    # 6. 查询信号强度
    log "INFO" "========== 7. 查询信号强度 =========="
    CSQ_RESP=$(send_at_cmd_test "+CSQ" "OK")
    if [ "$CSQ_RESP" != "ERROR" ]; then
        CSQ_INFO=$(parse_csq "$CSQ_RESP")
    else
        log "ERROR" "查询信号强度失败（非关键错误，继续测试）"
    fi

    log "INFO" "========== $MODULE_NAME 模块测试完成（关键项通过） =========="
}

# ===================== 拨号专用函数 =====================
# 等待AT端口可用（最多20次重试）
wait_ecm_interface() {
    log "INFO" "等待AT端口 $AT_PORT 加载完成..."
    local MAX_RETRY=20
    local WAIT_INTERVAL=5
    local RETRY_COUNT=0
    local BASIC_RESP=""
    while [ $RETRY_COUNT -lt $MAX_RETRY ]; do
        if [ -c "$AT_PORT" ]; then
            log "SUCCESS" "$AT_PORT 端口已加载（第$((RETRY_COUNT+1))次尝试）"
            log "INFO" "========== 测试模块ATI通信 =========="
            BASIC_RESP=$(send_at_cmd_test "I" "OK")
            if [ "$BASIC_RESP" = "ERROR" ]; then
                log "ERROR" "模块基础通信失败（无法响应AT指令）:$BASIC_RESP"
            else
                log "SUCCESS" "模块基础通信正常"
                return 0
            fi
        fi

        RETRY_COUNT=$((RETRY_COUNT + 1))
        log "WARN" "$AT_PORT 端口未加载（第$RETRY_COUNT/$MAX_RETRY次），等待$WAIT_INTERVAL秒..."
        if [ $MODULE_NAME == "BG95" ] || [ $MODULE_NAME == "BG95-SERIAL" ]; then
            check_bg95_env
        fi
        sleep $WAIT_INTERVAL
    done

    log "ERROR" "$AT_PORT 端口加载超时，终止拨号"
    exit 1
}

# 发送拨号AT指令（适配海外基站延迟）
send_at_cmd_dial() {
    local CMD=$1
    local EXPECT_RESP=$2
    local TEST_TIMEOUT=1
    local RETRY=3
    local RESP_FILE=$(mktemp)
    log "INFO" "【拨号指令】AT$CMD，期望响应：$EXPECT_RESP"

    for ((i=1; i<=RETRY; i++)); do
        # 拨号指令延长超时（适配数据连接建立）
        if [ "$CMD" = "+ZECMCALL=1" ]; then
            TEST_TIMEOUT=10
        fi
        echo -e "AT$CMD\r" | socat -t $TEST_TIMEOUT $AT_PORT,raw,echo=0,b115200 - > $RESP_FILE 2>>$LOG_FILE
        RESP=$(cat $RESP_FILE)
        log "INFO" "【拨号响应】第$i次重试 - $RESP"
        
        if echo "$RESP" | grep -qi "$EXPECT_RESP"; then
            log "SUCCESS" "拨号指令执行成功：AT$CMD（第$i次重试）"
            rm -f $RESP_FILE
            # 特殊处理ECM模式查询响应
            if [ "$CMD" = "+ZSWITCH?" ]; then
                echo "$RESP" | grep -qi "ECM\|L" && return 0 || return 1
            else
                return 0
            fi
        fi
        log "WARN" "拨号指令重试：AT$CMD（第 $i 次）"
        sleep 2
    done

    log "ERROR" "拨号指令执行失败：AT$CMD，最后响应：$RESP"
    rm -f $RESP_FILE
    return 1
}

# 切换ECM模式（拨号前置步骤）
config_ecm_mode() {
    log "INFO" "配置ECM模式..."
    send_at_cmd_dial "+ZSWITCH?" "OK"

    if [ $? -eq 0 ]; then
        log "INFO" "模块已在ECM模式，无需切换"
        return 0
    fi

    log "INFO" "切换模块到ECM模式..."
    if ! send_at_cmd_dial "+ZSWITCH=L" "OK"; then
        log "ERROR" "ECM模式切换失败，请检查固件版本"
        exit 1
    fi

    # 保存配置并重启模块
    send_at_cmd_dial "&W" "OK"
    log "INFO" "ECM模式已配置，重启模块生效..."
    send_at_cmd_dial "+CFUN=1,1" "OK"
    sleep 25

    # 验证ECM模式
    log "INFO" "验证ECM模式..."
    if ! send_at_cmd_dial "+ZSWITCH?" "ECM\|L\|OK"; then
        log "ERROR" "ECM模式切换验证失败"
        exit 1
    fi
    log "SUCCESS" "ECM模式切换成功"
}

# 配置拨号APN（支持认证）
config_apn() {
    log "INFO" "配置拨号APN：$APN（PDP类型：$PDP_TYPE，认证：$AUTH_TYPE）"
        
    #SET_CFUN_RESP=$(send_at_cmd_test "+CFUN=0" "OK")
    #if [ "$SET_CFUN_RESP" != "ERROR" ]; then
        #log "SUCCESS" "AT+CFUN=0 设置成功，射频已关闭全功能模式"
    #else
        #log "ERROR" "AT+CFUN=0 设置失败，射频状态无法关闭！"
    #fi

    #if ! send_at_cmd_dial "+CGACT=0" "OK"; then
        #log "ERROR" "去激活上下文（解除激活锁定）失败"
    #fi

    if ! send_at_cmd_dial "+CGDCONT?" "OK"; then
        log "ERROR" "APN查询失败，请检查参数"
    fi

    if [ "$MODULE_NAME" = "ME3630" ]; then
        # 构造APN配置指令（支持PAP/CHAP认证）
        local CGDCONT_BASE="+CGDCONT=1,\"$PDP_TYPE\",\"$APN\",\"\""
        if [ "$AUTH_TYPE" = "pap" ]; then
            CGDCONT_CMD="$CGDCONT_BASE,0,1,\"$APN_USER\",\"$APN_PWD\""
        elif [ "$AUTH_TYPE" = "chap" ]; then
            CGDCONT_CMD="$CGDCONT_BASE,0,2,\"$APN_USER\",\"$APN_PWD\""
        else
            CGDCONT_CMD="$CGDCONT_BASE,0,0"
        fi

        if ! send_at_cmd_dial "$CGDCONT_CMD" "OK"; then
            log "ERROR" "拨号APN配置失败，请检查参数"
        fi

        send_at_cmd_dial "&W" "OK"
    else
        local CGDCONT_BASE="+CGDCONT=1,\"$PDP_TYPE\",\"$APN\""
        if ! send_at_cmd_dial "$CGDCONT_BASE" "OK"; then
            log "ERROR" "拨号APN配置失败，请检查参数"
        fi
    fi

    SET_CFUN_RESP=$(send_at_cmd_test "+CFUN=1" "OK")
    if [ "$SET_CFUN_RESP" != "ERROR" ]; then
        log "SUCCESS" "AT+CFUN=1 设置成功，射频已恢复全功能模式"
        # 可选：设置后重新查询，验证是否生效
        RECHECK_CFUN=$(send_at_cmd_test "+CFUN?" "OK" | grep -v 'OK' | tr -d ' ')
        log "INFO" "CFUN设置后重新验证：$RECHECK_CFUN"
    else
        log "ERROR" "AT+CFUN=1 设置失败，射频状态无法恢复！"
    fi

    #if ! send_at_cmd_dial "+CGACT=1,1" "OK"; then
        #log "ERROR" "激活 1 号上下文失败，请检查参数"
    #fi

    log "SUCCESS" "拨号APN配置成功"
    if ! send_at_cmd_dial "+CGDCONT?" "OK"; then
        log "ERROR" "APN查询失败，请检查参数"
    fi
}

# 激活ECM数据连接
activate_ecm_call() {
    log "INFO" "关闭旧数据连接..."
    send_at_cmd_dial "+ZECMCALL=0" "OK"
    sleep 3

    log "INFO" "激活ECM数据连接..."
    if ! send_at_cmd_dial "+ZECMCALL=1" "OK"; then
        log "ERROR" "ECM数据连接激活失败（关键错误）"
        exit 1
    fi
    log "SUCCESS" "ECM数据连接激活成功"
    sleep 8  # 等待数据连接建立
}

# 配置ECM网络接口（DHCP获取IP+连通性验证）
activate_ecm() {
    log "INFO" "等待ECM接口 $ECM_IFACE 加载..."
    # 等待接口加载（最多20次重试）
    for i in {1..20}; do
        if ip link show $ECM_IFACE >/dev/null 2>&1; then
            break
        fi
        sleep 2
    done

    if ! ip link show $ECM_IFACE >/dev/null 2>&1; then
        log "ERROR" "ECM接口 $ECM_IFACE 加载失败"
        exit 1
    fi

    # 启用接口并获取IP地址
    ip link set $ECM_IFACE up
    sleep 3
    log "INFO" "为 $ECM_IFACE 获取IP地址..."
    
    # 自动适配不同系统的DHCP客户端
    if command -v dhclient >/dev/null 2>&1; then
        dhclient -r $ECM_IFACE >/dev/null 2>&1
        dhclient $ECM_IFACE >/dev/null 2>&1
    elif command -v dhcpcd >/dev/null 2>&1; then
        dhcpcd $ECM_IFACE >/dev/null 2>&1
    elif command -v udhcpc >/dev/null 2>&1; then
        udhcpc -i $ECM_IFACE -q >/dev/null 2>&1
    else
        log "ERROR" "未找到可用的DHCP客户端（dhclient/dhcpcd/udhcpc）"
        exit 1
    fi

    # 检查IP地址获取结果
    IP_ADDR=$(ip addr show $ECM_IFACE | grep "inet " | awk '{print $2}' | cut -d/ -f1)
    if [ -z "$IP_ADDR" ]; then
        log "ERROR" "$ECM_IFACE 未获取到IP地址"
        exit 1
    else
        log "SUCCESS" "$ECM_IFACE 已获取IP地址：$IP_ADDR"
    fi
}

cfun_check() {
    CFUN_CTRL=0
    CFUN_PIN=122
    if [ $CFUN_CTRL -eq 1 ]; then
        # 1. 执行AT+CFUN?查询射频状态
        CFUN_RESP=$(send_at_cmd_test "+CFUN?" "OK")
        if [ "$CFUN_RESP" != "ERROR" ]; then
            # 2. 解析纯净CFUN状态值（过滤OK+删除空格，得到+CFUN:1格式）
            CFUN=$(echo "$CFUN_RESP" | grep -v 'OK' | tr -d ' ')
            log "SUCCESS" "当前CFUN状态：$CFUN"
            # 3. 核心判断：若CFUN≠+CFUN:1（射频未开启/异常），强制设置为1
            if [ "$CFUN" != "+CFUN:1" ]; then
                log "WARN" "射频状态异常（非全功能模式），开始执行AT+CFUN=1设置全功能模式..."
                # 执行AT+CFUN=1指令，强制开启射频全功能模式
                SET_CFUN_RESP=$(send_at_cmd_test "+CFUN=1" "OK")
                if [ "$SET_CFUN_RESP" != "ERROR" ]; then
                    log "SUCCESS" "AT+CFUN=1 设置成功，射频已恢复全功能模式"
                    # 可选：设置后重新查询，验证是否生效
                    RECHECK_CFUN=$(send_at_cmd_test "+CFUN?" "OK" | grep -v 'OK' | tr -d ' ')
                    log "INFO" "CFUN设置后重新验证：$RECHECK_CFUN"
                else
                    log "ERROR" "AT+CFUN=1 设置失败，射频状态无法恢复！"
                fi
            fi
        else
            log "ERROR" "查询CFUN状态失败，模组串口通信可能异常..."
            # 可选：查询失败时，尝试强制设置CFUN=1
            # send_at_cmd_test "+CFUN=1" "OK" >/dev/null 2>&1
        fi
    else
        log "INFO" "重置飞行脚（$CFUN_PIN）..."
        if [ ! -d "/sys/class/gpio/gpio$CFUN_PIN/" ]; then
            echo $CFUN_PIN > /sys/class/gpio/export
        fi
        echo out > /sys/class/gpio/gpio$CFUN_PIN/direction
        echo 0 > /sys/class/gpio/gpio$CFUN_PIN/value
        sleep 1
        echo 1 > /sys/class/gpio/gpio$CFUN_PIN/value
    fi
}

# ===================== Ping监控函数 =====================
ping_monitor() {
    log "INFO" "启动 $MODULE_NAME 网络连通性监控：初始ping $INIT_PING_COUNT 次，之后每5分钟ping一次"
    # 第一步：初始阶段 - 30秒内ping 10次（每3秒一次）
    for ((i=1; i<=INIT_PING_COUNT; i++)); do
        log "INFO" "【初始Ping检测】第 $i/$INIT_PING_COUNT 次 - 目标：$PING_TEST_HOST"
        if ping -c $PING_COUNT -W $PING_TIMEOUT $PING_TEST_HOST >/dev/null 2>&1; then
            log "SUCCESS" "【初始Ping检测】第 $i 次成功 - $PING_TEST_HOST 可达"
            break
        else
            log "WARN" "【初始Ping检测】第 $i 次失败 - $PING_TEST_HOST 不可达"
            if [ $MODULE_NAME != "ME3630" ]; then
                if [ $(($i%3)) -eq 0 ]; then
                    cfun_check
                fi
            fi
        fi
        # 最后一次不需要等待
        if [ $i -lt $INIT_PING_COUNT ]; then
            sleep $INIT_PING_INTERVAL
        fi
    done
    log "INFO" "初始Ping检测完成，切换到长期监控"

    # 第二步：长期阶段 - 每5分钟ping一次
    LONG_PING_COUNT=0
    while true; do
        log "INFO" "【长期Ping检测】- 目标：$PING_TEST_HOST"
        if ping -c $PING_COUNT -W $PING_TIMEOUT $PING_TEST_HOST >/dev/null 2>&1; then
            log "SUCCESS" "【长期Ping检测】成功 - $PING_TEST_HOST 可达"
            ifconfig eth0 $DEFAULT_ETH_IP
        else
            log "WARN" "【长期Ping检测】失败 - $PING_TEST_HOST 不可达"
            if [ $MODULE_NAME != "ME3630" ]; then
                cfun_check
            fi
        fi
        sleep $LONG_PING_INTERVAL
        # 长时间ping失败3次后，尝试恢复eth0网络
        LONG_PING_COUNT=$(($LONG_PING_COUNT+1))
        if [ 3 -lt $LONG_PING_COUNT ]; then
            ifconfig eth0 $DEFAULT_ETH_IP
            LONG_PING_COUNT=0
        fi
    done
}

check_ecm () {
    if [ $MODULE_NAME == "BG95" ]; then
        # 定义常量，便于维护和修改
        local EXPECTED_AT_CONFIG="ecm"
        
        local AT_CMD_CHECK="+QCFGEXT=\"usbnet\""
        local AT_CMD_FIX="+QCFGEXT=\"usbnet\",\"ecm\""


        # ==================== 1. 检测并修复AT指令配置 ====================
        log "INFO" "【检测AT指令配置：${AT_CMD_CHECK}】"
        
        # 执行AT指令并捕获返回结果，增加错误处理
        local at_response
        at_response=$(send_at_cmd_test "${AT_CMD_CHECK}" "OK")
        if [ $? -ne 0 ]; then
            log "ERROR" "【AT指令执行失败】发送${AT_CMD_CHECK}时返回非OK"
        else
            local core_config
            core_config=$(echo "$at_response" | grep -Eo '\+QCFGEXT: "usbnet","[^"]+"' | awk -F '"' '{print $4}' | tr -d '[:space:]')
            if [ "$core_config" = "${EXPECTED_AT_CONFIG}" ]; then
                log "INFO" "【AT指令检测通过】当前配置: ${core_config}"
            else
                log "ERROR" "【AT指令检测失败】当前配置: ${core_config}，期望值: ${EXPECTED_AT_CONFIG}"
                log "INFO" "【尝试自动修复】发送指令: ${AT_CMD_FIX}"
                local fix_response
                fix_response=$(send_at_cmd_test "${AT_CMD_FIX}" "OK")
                if [ $? -eq 0 ]; then
                    # 验证修复后的配置
                    at_response=$(send_at_cmd_test "${AT_CMD_CHECK}" "OK")
                    local fix_check_config
                    fix_check_config=$(echo "$at_response" | grep -Eo '\+QCFGEXT: "usbnet","[^"]+"' | awk -F '"' '{print $4}' | tr -d '[:space:]')
                    if [ "$fix_check_config" = "${EXPECTED_AT_CONFIG}" ]; then
                        log "INFO" "【AT配置修复成功】修复后配置: ${fix_check_config}"
                        check_bg95_env
                    else
                        log "ERROR" "【AT配置修复失败】修复后配置仍异常: ${fix_check_config}"
                    fi
                else
                    log "ERROR" "【AT配置修复失败】发送修复指令${AT_CMD_FIX}返回非OK"
                fi
            fi
        fi
        check_at_port
        log "INFO" "【check_ecm流程结束】"
    fi
}

check_eth () {
    local ETH_INTERFACE="eth1"
    # ==================== 2. 检测eth1网络节点 ====================
    log "INFO" "【检测网络节点：${ETH_INTERFACE}】"
    if ! command -v ip &> /dev/null; then
        log "ERROR" "【检测工具缺失】未找到ip命令，无法检测网络节点"
    else
        if ip link show "${ETH_INTERFACE}" >/dev/null 2>&1; then
            ifconfig ${ETH_INTERFACE} up
            local eth_status
            eth_status=$(ip link show "${ETH_INTERFACE}" | awk '/state/ {print $9}')
            local eth_ip
            eth_ip=$(ip addr show "${ETH_INTERFACE}" | grep -o 'inet [0-9\.]*' | awk '{print $2}')
            if [ -n "${eth_ip}" ]; then
                log "INFO" "【${ETH_INTERFACE}节点检测通过】状态: ${eth_status}，IP地址: ${eth_ip}"
            else
                log "INFO" "【${ETH_INTERFACE}节点检测通过】状态: ${eth_status}，无IP地址"
            fi
        else
            log "ERROR" "【${ETH_INTERFACE}节点检测失败】未找到该网络节点"
        fi
    fi
}

ppp_dial() {
    if [ $MODULE_NAME == "BG95-SERIAL" ]; then
        log "INFO" "【BG95-SERIAL PPP拨号流程开始】"
        # 1. 定义运营商拨号指令映射（可扩展）
        local DIAL_NUMBER="*99#"  # 移动/联通默认
        if [[ "$APN" == *"ctnet"* || "$APN" == *"ctwap"* ]]; then
            DIAL_NUMBER="#777"  # 电信专用
        fi

        # 2. 转换认证类型（CGDCONT需要数字：0=none,1=pap,2=chap）
        local AUTH_NUM=0
        if [ "$AUTH_TYPE" = "pap" ]; then
            AUTH_NUM=1
        elif [ "$AUTH_TYPE" = "chap" ]; then
            AUTH_NUM=2
        fi

        # 3. 校验核心参数
        if [ -z "$APN" ]; then
            log "ERROR" "APN参数为空，使用默认值cmnet"
            APN="cmnet"
        fi
        if [ -z "$PDP_TYPE" ]; then
            PDP_TYPE="IP"
        fi

        # 4. 动态生成chat连接脚本（替换占位符）
        local TMP_CHAT_SCRIPT="/tmp/quectel-chat-connect.tmp"
        cp /etc/ppp/chatscripts/quectel-chat-connect $TMP_CHAT_SCRIPT
        sed -i "s/{APN}/$APN/g" $TMP_CHAT_SCRIPT
        sed -i "s/{PDP_TYPE}/$PDP_TYPE/g" $TMP_CHAT_SCRIPT
        sed -i "s/{AUTH_TYPE}/$AUTH_NUM/g" $TMP_CHAT_SCRIPT
        sed -i "s/{DIAL_NUMBER}/$DIAL_NUMBER/g" $TMP_CHAT_SCRIPT
        # 5. 配置认证参数（如有）
        local PPP_AUTH_CONF="/tmp/ppp_auth.tmp"
        echo "" > $PPP_AUTH_CONF
        if [ "$AUTH_TYPE" != "none" ] && [ -n "$APN_USER" ] && [ -n "$APN_PWD" ]; then
            log "INFO" "配置PPP认证：$AUTH_TYPE, 用户=$APN_USER"
            echo "user \"$APN_USER\" password \"$APN_PWD\"" >> $PPP_AUTH_CONF
            # 追加认证配置到quectel-dial
            grep -q "user " /etc/ppp/peers/quectel-dial || cat $PPP_AUTH_CONF >> /etc/ppp/peers/quectel-dial
        else
            # 无认证时确保注释掉user/password
            sed -i '/user /d' /etc/ppp/peers/quectel-dial
            sed -i '/password /d' /etc/ppp/peers/quectel-dial
        fi

        # 6. 停止旧的pppd进程
        log "INFO" "清理旧的PPP拨号进程"
        pkill -f "pppd call quectel-dial"
        sleep 2

        # 7. 执行PPP拨号（带参数传递）
        log "INFO" "启动PPP拨号：APN=$APN, PDP=$PDP_TYPE, 认证=$AUTH_TYPE, 拨号指令=$DIAL_NUMBER"
        pppd call quectel-dial
        local PPP_PID=$!
        log "INFO" "PPP拨号进程已启动，PID=$PPP_PID"

        # 8. 等待拨号成功并验证
        sleep 10
        if ip link show ppp0 >/dev/null 2>&1; then
            local PPP_IP=$(ip addr show ppp0 | grep "inet " | awk '{print $2}' | cut -d/ -f1)
            log "SUCCESS" "BG95-SERIAL PPP拨号成功，ppp0 IP=$PPP_IP"
        else
            log "ERROR" "BG95-SERIAL PPP拨号失败，查看日志：/var/log/quectel-dial.log"
            # 重试一次
            log "INFO" "尝试重新拨号..."
            pkill -f "pppd call quectel-dial"
            sleep 3
            pppd call quectel-dial
        fi

        # 9. 清理临时文件
        #rm -f $TMP_CHAT_SCRIPT $PPP_AUTH_CONF
        log "INFO" "【BG95-SERIAL PPP拨号流程完成】"
    fi
}

quectel_dial() {
    local TARGET_INTERFACE=""
    if [ "$MODULE_NAME" = "EG21" ]; then
        TARGET_INTERFACE="wwan0"
    elif [ "$MODULE_NAME" = "EC200U" ]; then
        TARGET_INTERFACE="usb0"
    elif [ "$MODULE_NAME" = "BG95" ]; then
        TARGET_INTERFACE="usb0"
    elif [ "$MODULE_NAME" = "BG95-SERIAL" ]; then
        TARGET_INTERFACE="usb0"
    fi

    local DIAL_COMMAND="/etc/zl_test/quectel-CM"
    local RESTART_DELAY=3
    local INTERFACE_STABLE_TIME=2
    while true; do
        # 内层循环：严格检测usb0接口，确保完全就绪后再启动拨号程序
        log "INFO" "正在检测${TARGET_INTERFACE}网络接口（避免提前调用影响接口生成）..."
        while true; do
            # 1. 第一步：检测接口是否存在（基础校验，避免提前占用资源）
            if ! ip link show "${TARGET_INTERFACE}" >/dev/null 2>&1; then
                # 接口不存在时，休眠1秒重试，不执行任何拨号相关操作
                sleep 5
                continue
            fi

            # 2. 第二步：新增网口稳定校验（核心优化，避免假就绪）
            log "INFO" "检测到${TARGET_INTERFACE}接口，正在校验稳定性（${INTERFACE_STABLE_TIME}秒）..."
            sleep ${INTERFACE_STABLE_TIME}
            # 校验后再次确认接口是否存在（排除临时加载、瞬间消失的情况）
            if ip link show "${TARGET_INTERFACE}" >/dev/null 2>&1; then
                log "INFO" "${TARGET_INTERFACE}接口已稳定就绪，不会提前调用拨号程序。"
                break
            else
                log "INFO" "${TARGET_INTERFACE}接口临时消失，重新检测..."
                continue
            fi
        done

        # 3. 第三步：接口稳定就绪后，再执行拨号程序（杜绝提前调用）
        if [ -x "${DIAL_COMMAND}" ]; then
            log "INFO" "启动4G拨号工具，日志保存至/var/log/quectel_4g_dial.log"
            # 前台运行，便于守护进程监控，不提前占用资源
            if [ -z "$APN_USER" ]; then
                auth_type=""
            elif [ $AUTH_TYPE == "none" ]; then
                auth_type="0"
            elif [ $AUTH_TYPE == "pap" ]; then
                auth_type="1"
            elif [ $AUTH_TYPE == "chap" ]; then
                auth_type="2"
            else
                auth_type="0"
            fi
            log "INFO" "执行命令：${DIAL_COMMAND} -s $APN $APN_USER $APN_PWD $auth_type"
            ${DIAL_COMMAND} -s $APN $APN_USER $APN_PWD $AUTH_TYPE >/var/log/quectel_4g_dial.log 2>&1
            # 拨号进程退出后的提示
            log "WARN" "警告：$(date) quectel-CM进程意外退出，将在${RESTART_DELAY}秒后尝试重启（先重新检测usb0）..."
        else
            log "WARN" "错误：${DIAL_COMMAND} 不存在或无执行权限！"
            log "INFO" "将在${RESTART_DELAY}秒后重新检查..."
        fi

        # 4. 第四步：重启前休眠，避免频繁重试导致的资源占用
        sleep ${RESTART_DELAY}
    done
}

# 执行拨号流程
run_dial() {
    # 启动Ping监控（后台运行，不阻塞拨号流程）
    ping_monitor &
    local PING_PID=$!
    log "INFO" "Ping监控进程已启动，PID：$PING_PID"
    log "INFO" "== 开始执行 $MODULE_NAME 拨号流程 =="

    # 核心拨号步骤
    if [ $MODULE_NAME == "ME3630" ]; then
        config_ecm_mode    # 切换ECM模式
        config_apn         # 配置拨号APN
        activate_ecm_call  # 激活数据连接
        activate_ecm       # 配置网络接口+验证连通性
        log "INFO" "== $MODULE_NAME 模块拨号流程完成 =="
    elif [ $MODULE_NAME == "BG95-SERIAL" ]; then
        ln -s /etc/ppp/resolv.conf /etc/resolv.conf
        ppp_dial            # 拨号
    elif [ $MODULE_NAME == "BG95" ]; then
        ln -s /etc/ppp/resolv.conf /etc/resolv.conf
        check_eth
    else
        quectel_dial        # 拨号
    fi
}

# ===================== 参数解析 =====================
parse_args() {
    while [[ $# -gt 0 ]]; do
        case "$1" in
            # 拨号相关参数
            -m)
                MODULE_NAME="$2"
                shift 2
                ;;
            -a)
                APN="$2"
                shift 2
                ;;
            -t)
                PDP_TYPE="$2"
                # 验证PDP类型合法性
                if [[ ! "$PDP_TYPE" =~ ^(IP|IPV6|IPV4V6)$ ]]; then
                    echo "错误：无效的PDP类型 '$PDP_TYPE'，必须是 IP/IPV6/IPV4V6"
                    print_help
                fi
                shift 2
                ;;
            -auth)
                AUTH_TYPE="$2"
                # 验证认证类型合法性
                if [[ ! "$AUTH_TYPE" =~ ^(none|pap|chap)$ ]]; then
                    echo "错误：无效的认证类型 '$AUTH_TYPE'，必须是 none/pap/chap"
                    print_help
                fi
                shift 2
                ;;
            -u)
                APN_USER="$2"
                shift 2
                ;;
            -p)
                APN_PWD="$2"
                shift 2
                ;;
            -port)
                AT_PORT="$2"
                shift 2
                ;;
            -i)
                ECM_IFACE="$2"
                shift 2
                ;;
            -ping)
                PING_TEST_HOST="$2"
                shift 2
                ;;
            # 帮助信息
            -h|--help)
                print_help
                ;;
            # 未知参数
            *)
                echo "错误：未知参数 '$1'，使用 -h/--help 查看帮助"
                print_help
                ;;
        esac
    done

    # 输出最终生效配置
    log "INFO" "========== 当前配置 =========="
    log "INFO" "模块名称：$MODULE_NAME"
    log "INFO" "拨号配置：APN=$APN, PDP_TYPE=$PDP_TYPE, AUTH_TYPE=$AUTH_TYPE, PING_HOST=$PING_TEST_HOST APN_USER=$APN_USER APN_PWD=$APN_PWD"
}

# ===================== 主流程 =====================
main() {
    # 初始化日志文件
    > $LOG_FILE
    # 解析命令行参数
    parse_args "$@"
    check_image_version
    # 前置检查 root（GPIO 操作需要）
    check_root
    # 清理旧进程
    kill_iot_start_process
    check_env
    # AT端口检查
    check_at_port

    systemctl stop connman
    rm -rf /etc/resolv.conf
    ifconfig eth0 down

    check_ecm

    # 第一步：执行全量测试（关键项失败则终止）
    run_full_test

    # 第二步：测试通过后执行拨号流程
    run_dial
    while true; do
        sleep 300
    done
    #exit 0
}

# 执行主流程
main "$@"