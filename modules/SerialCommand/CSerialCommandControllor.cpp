#include "CSerialCommandControllor.h"
#include <sstream>
#include <iomanip>
#include <string>
#include <vector>
#include <cstring>
#include <QDebug>

#include "asio.hpp"

namespace zl
{
namespace zlmodules
{

struct CSerialCommandControllor::IOReply
{
    unsigned char data[1024];
    size_t len;

    IOReply() : data{0x00}, len(0) {}
};

struct CSerialCommandControllor::IOImpl
{
    asio::io_context io;
    asio::serial_port serial;
    IOImpl() : serial(io)
    {
    }
};

unsigned char genChk(const unsigned char *vptr, uint32_t len)
{
    int size = len;

    int index;
    unsigned char chk;

    chk = vptr[0];
    index = 1;

    for (; index < size; ++index)
        chk ^= vptr[index];

    return chk;
}

CSerialCommandControllor::CSerialCommandControllor(ISerialCommandEventNotice* obs)
    : io_(new IOImpl()),
      io_reply_(new IOReply()),
      io_read_flag_(false),
      io_read_thread_(nullptr),
      obs_(obs)
{
}

CSerialCommandControllor::~CSerialCommandControllor()
{
    Disconnect();
    if (io_ != nullptr)
    {
        delete io_;
        io_ = nullptr;
    }
}

int32_t CSerialCommandControllor::Connect(const std::string &port, int32_t baud)
{
    if (io_ == nullptr)
    {
        return -1;
    }

    if (io_->serial.is_open())
    {
        return 0;
    }

    try
    {
        // open serial port
        asio::error_code ec;
        io_->serial = asio::serial_port(io_->io);

        io_->serial.open(port, ec);
        if (ec)
        {
            std::cerr << "open serial port failed:" << ec.message() << std::endl;
            qDebug() << "[ERROR]" << "open serial port failed: " << QString::fromLocal8Bit(ec.message().data());
            return -1;
        }
        io_->serial.set_option(asio::serial_port::baud_rate(baud));
        io_->serial.set_option(asio::serial_port::flow_control(
            asio::serial_port::flow_control::none));
        io_->serial.set_option(asio::serial_port::parity(
            asio::serial_port::parity::none));
        io_->serial.set_option(asio::serial_port::stop_bits(
            asio::serial_port::stop_bits::one));
        io_->serial.set_option(asio::serial_port::character_size(8));

        // start io read work thread
        start_read_work_thread();

        return 0;
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        qDebug() << "[ERROR]" << QString::fromLocal8Bit(e.what());
        return -1;
    }
}

void CSerialCommandControllor::Disconnect()
{
    try
    {
        if (io_ != nullptr)
        {
            io_->serial.close();
            qDebug() << "[INFO]" << "close asio io serial";
        }

        io_read_flag_ = false;
        if (io_read_thread_ != nullptr && io_read_thread_->joinable())
        {
            io_read_thread_->join();
        }
        delete io_read_thread_;
        io_read_thread_ = nullptr;
    }
    catch (const std::exception &e)
    {
//        std::cerr << e.what() << std::endl;
        qDebug() << "[ERROR]" << QString::fromLocal8Bit(e.what());
    }
}

int32_t CSerialCommandControllor::SimTestCommand(const std::string& apn_config)
{
    std::unique_lock<std::mutex> ul(mutex_);
    std::cout << __FUNCTION__ << std::endl;

    if (apn_config.empty())
    {
        qDebug() << "[ERROR]" << "send sim test error, apn config is empty";
        return -2;
    }

    // send command
    auto ret = cmd_sim_test_command(apn_config);
    if (ret != 0)
    {
//        std::cerr << "send sim test command return failed: " << ret << std::endl;
        qDebug() << "[ERROR]" << "send sim test command return failed: " << ret;
        return -1;
    }

    return 0;
}

int32_t CSerialCommandControllor::IotTestCommand()
{
    std::unique_lock<std::mutex> ul(mutex_);
    std::cout << __FUNCTION__ << std::endl;

    // send command
    auto ret = cmd_iot_test_command();
    if (ret != 0)
    {
//        std::cerr << "send iot test command return failed: " << ret << std::endl;
        qDebug() << "[ERROR]" << "send iot test command return failed: " << ret;
        return -1;
    }

    return 0;
}

int32_t CSerialCommandControllor::TotalTestCommand(const std::string& apn_config)
{
    std::unique_lock<std::mutex> ul(mutex_);
    std::cout << __FUNCTION__ << std::endl;

    if (apn_config.empty())
    {
        qDebug() << "[ERROR]" << "send all test error, apn config is empty";
        return -2;
    }

    // send command
    auto ret = cmd_total_test_command(apn_config);
    if (ret != 0)
    {
//        std::cerr << "send total test command return failed: " << ret << std::endl;
        qDebug() << "[ERROR]" << "send total test command return failed: " << ret;
        return -1;
    }

    return 0;
}

int32_t CSerialCommandControllor::GetVersionCommand()
{
    std::unique_lock<std::mutex> ul(mutex_);
    std::cout << __FUNCTION__ << std::endl;

    // send command
    auto ret = cmd_get_version_command();
    if (ret != 0)
    {
//        std::cerr << "send total test command return failed: " << ret << std::endl;
        qDebug() << "[ERROR]" << "send total test command return failed: " << ret;
        return -1;
    }

    return 0;
}

int32_t CSerialCommandControllor::cmd_sim_test_command(const std::string& apn_config)
{
    size_t apn_len = apn_config.length();
    size_t cmd_len = 6 + apn_len;  // 6个固定字节 + APN长度
    unsigned char command = CMD_SIM;
    unsigned char index = get_cmd_index(command);

    std::vector<unsigned char> cmd(cmd_len);

    // 初始化为0
    for (size_t i = 0; i < cmd_len; i++)
        cmd[i] = 0x00;

    cmd[0] = CMD_HEAD_FLAG;        // 0xAA
    cmd[1] = command;              // 0x01
    cmd[2] = static_cast<unsigned char>(cmd_len);  // 6+N

    // 复制APN字符串到指令内容区域
    memcpy(&cmd[3], apn_config.c_str(), apn_len);

    cmd[cmd_len-3] = index;                                    // 指令编号
    cmd[cmd_len-2] = genChk(cmd.data(), cmd_len-2);           // 校验值
    cmd[cmd_len-1] = END_FLAG;                                // 0x55

    // 日志打印发送内容
    QString hex;
    for (size_t i = 0; i < cmd_len; ++i) hex += QString::asprintf("%02X ", cmd[i]);
    qDebug() << "[SEND][SIM]" << hex.trimmed();
    // write command
    auto ret = sync_write_cmd(cmd.data(), cmd_len);
    if (ret < static_cast<int>(cmd_len))
    {
        qDebug() << "[ERROR]" << "sim test command write to io failed:" << ret;
        return -1;
    }

    add_cmd_index(command, index);

    return 0;
}

int32_t CSerialCommandControllor::cmd_iot_test_command()
{
    constexpr size_t cmd_len = MIN_LEN;
    unsigned char command = CMD_IOT;
    unsigned char index = get_cmd_index(command);

    unsigned char cmd[cmd_len]{};

    for (uint i = 0; i < cmd_len; i++)
        cmd[i] = 0x00;

    cmd[0] = CMD_HEAD_FLAG;
    cmd[1] = command;
    cmd[2] = cmd_len;

    cmd[cmd_len-3] = index;
    cmd[cmd_len-2] = genChk(cmd, cmd_len-2);
    cmd[cmd_len-1] = END_FLAG;

    // 日志打印发送内容
    QString hex;
    for (size_t i = 0; i < cmd_len; ++i) hex += QString::asprintf("%02X ", cmd[i]);
    qDebug() << "[SEND][IOT]" << hex.trimmed();
    // write command
    auto ret = sync_write_cmd(cmd, cmd_len);
    if (ret < cmd_len)
    {
        qDebug() << "[ERROR]" << "iot test command write to io failed:" << ret;
        return -1;
    }

    add_cmd_index(command, index);

    return 0;
}

int32_t CSerialCommandControllor::cmd_total_test_command(const std::string& apn_config)
{
    size_t apn_len = apn_config.length();
    size_t cmd_len = 6 + apn_len;  // 6个固定字节 + APN长度
    unsigned char command = CMD_TOTAL;
    unsigned char index = get_cmd_index(command);

    std::vector<unsigned char> cmd(cmd_len);

    // 初始化为0
    for (size_t i = 0; i < cmd_len; i++)
        cmd[i] = 0x00;

    cmd[0] = CMD_HEAD_FLAG;        // 0xAA
    cmd[1] = command;              // 0x03
    cmd[2] = static_cast<unsigned char>(cmd_len);  // 6+N

    // 复制APN字符串到指令内容区域
    memcpy(&cmd[3], apn_config.c_str(), apn_len);

    cmd[cmd_len-3] = index;                                    // 指令编号
    cmd[cmd_len-2] = genChk(cmd.data(), cmd_len-2);           // 校验值
    cmd[cmd_len-1] = END_FLAG;                                // 0x55

    // 日志打印发送内容
    QString hex;
    for (size_t i = 0; i < cmd_len; ++i) hex += QString::asprintf("%02X ", cmd[i]);
    qDebug() << "[SEND][TOTAL]" << hex.trimmed();
    // write command
    auto ret = sync_write_cmd(cmd.data(), cmd_len);
    if (ret < static_cast<int>(cmd_len))
    {
        qDebug() << "[ERROR]" << "iot&sim test command write to io failed:" << ret;
        return -1;
    }

    add_cmd_index(command, index);

    return 0;
}

int32_t CSerialCommandControllor::cmd_get_version_command()
{
    constexpr size_t cmd_len = MIN_LEN;
    unsigned char command = CMD_VERSION;
    unsigned char index = get_cmd_index(command);

    unsigned char cmd[cmd_len]{};

    for (uint i = 0; i < cmd_len; i++)
        cmd[i] = 0x00;

    cmd[0] = CMD_HEAD_FLAG;
    cmd[1] = command;
    cmd[2] = cmd_len;

    cmd[cmd_len-3] = index;
    cmd[cmd_len-2] = genChk(cmd, cmd_len-2);
    cmd[cmd_len-1] = END_FLAG;

    // write command
    auto ret = sync_write_cmd(cmd, cmd_len);
    if (ret < cmd_len)
    {
//        std::cerr << "get version test command write to io failed: " << ret << std::endl;
        qDebug() << "[ERROR]" << "get version test command write to io failed:" << ret;
        return -1;
    }

    add_cmd_index(command, index);
    return 0;
}

bool CSerialCommandControllor::cv_wait(int32_t timeout)
{
    // wait for reply
    std::unique_lock<std::mutex> ul2(mutex_cv_);
    flag_cv_ = false;
    memset(io_reply_->data, 0, 1024);
    io_reply_->len = 0;

    auto st = cv_.wait_for(ul2, std::chrono::milliseconds(timeout), [&] { return flag_cv_; });
    if (!st)
    {
        std::cerr << "cv wait timeout" << std::endl;
        return false;
    }
    return true;
}

int32_t CSerialCommandControllor::analyze_receive_cmd_buffer()
{
    CommandResultType type(Crt_Unknown);

    type = Crt_Success;

    if (io_reply_->len < MIN_LEN)
    {
        if (io_reply_->len < 1) return -1;

        if (obs_ != nullptr)
            obs_->CommandResult(io_reply_->data[1], Crt_Length_error, io_reply_->data, io_reply_->len);
    }

    unsigned char command = io_reply_->data[1];
    unsigned char len = io_reply_->data[2];
    unsigned char state = io_reply_->data[3];

    unsigned char cmd_index = io_reply_->data[io_reply_->len - 3];
    unsigned char check = io_reply_->data[io_reply_->len - 2];

    if (command < CMD_SIM || command > CMD_VERSION) type = Crt_Command_error;
    if (io_reply_->len != len) type = Crt_Length_error;

    if (check != genChk(io_reply_->data, io_reply_->len - 2)) type = Crt_Bcc_error;

    if (state != RET_BUSY_FLAG && state != RET_VALID_FLAG && io_reply_->len == MIN_LEN) type = Crt_State_error;
    if (state == RET_ERROR_FLAG && io_reply_->len == MIN_LEN) type = Crt_Check_fail;

    if (state == RET_BUSY_FLAG && io_reply_->len == MIN_LEN) type = Crt_Machine_busy;

    if (is_index_valid(command, cmd_index) != 0)
    {
        qDebug() << "[ERROR]" << "receive command return error, cmd:" << command << " cmd index:" << cmd_index;

        type = Crt_Index_error;
        return -1;
    }

    if (type == Crt_Machine_busy)
    {
        remove_cmd_index(command, cmd_index);
    }

    if (obs_ != nullptr && type != Crt_Index_error)
    {
        obs_->CommandResult(command, type, io_reply_->data, io_reply_->len);
    }

    return 0;
}

int32_t CSerialCommandControllor::analyze_receive_msg_buffer()
{
    CommandResultType type(Crt_Unknown);

    type = Crt_Success;

    if (io_reply_->len < MIN_LEN)
    {
        if (io_reply_->len < 1) return -1;

        if (obs_ != nullptr)
            obs_->CommandResult(io_reply_->data[1], Crt_Length_error, io_reply_->data, io_reply_->len);
    }

    unsigned char command = io_reply_->data[1];
    unsigned char len = io_reply_->data[2];

    unsigned char cmd_index = io_reply_->data[io_reply_->len - 3];
    unsigned char check = io_reply_->data[io_reply_->len - 2];


    if (command < CMD_SIM || command > CMD_VERSION) type = Crt_Command_error;
    if (io_reply_->len != len) type = Crt_Length_error;
    if (check != genChk(io_reply_->data, io_reply_->len - 2)) type = Crt_Bcc_error;

    if (io_reply_->data[3] == RET_ERROR_FLAG && io_reply_->len == MIN_LEN) type = Crt_Check_fail;


    if (is_index_valid(command, cmd_index) != 0)
    {
        qDebug() << "[ERROR]" << "receive msg return error, cmd:" << command << " cmd index:" << cmd_index;

        type = Crt_Index_error;
        return -1;
    }
    else
    {
        remove_cmd_index(command, cmd_index);
    }

    if (obs_ != nullptr)
    {
        if (type == Crt_Success)
            obs_->MsgData(command, io_reply_->data + 3, io_reply_->len - 6);
        else
            obs_->CommandResult(command, type, io_reply_->data, io_reply_->len);
    }

    return 0;
}

size_t CSerialCommandControllor::sync_write_cmd(unsigned char *buff, size_t len, int32_t timeout)
{
    try
    {
        // async handler
        auto phandler = [&](const std::error_code &ec, std::size_t len,
                            std::error_code *ec_out, std::size_t *len_out) -> void {
            *ec_out = ec;
            *len_out = len;
        };

        // async send
        std::size_t length(0);
        std::error_code ec;
        io_->serial.async_write_some(
            asio::buffer(buff, len),
            std::bind(phandler, std::placeholders::_1, std::placeholders::_2, &ec, &length));
        io_->io.restart();
        io_->io.run_for(std::chrono::milliseconds(timeout));
        if (!io_->io.stopped())
        {
            io_->serial.cancel();
            io_->io.run();
        }
        return length;
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        return 0;
    }
}
/*
void CSerialCommandControllor::process_package(const unsigned char *pkg, size_t len)
{
    // static cache
    constexpr size_t max_buffer_len = 2 * 65535;
    static unsigned char _buffer[max_buffer_len]{0x00};
    static size_t _buffer_size = 0;

    if (len + _buffer_size >= max_buffer_len)
    {
        memset(_buffer, 0, max_buffer_len);
        _buffer_size = 0;
        return;
    }
    else
    {
        memcpy(_buffer + _buffer_size, pkg, len);
        _buffer_size += len;
    }

    if (_buffer_size <= 2)
    {
        return;
    }

    // parse package
    unsigned char msg_len = _buffer[2];

    while ((_buffer[0] == CMD_HEAD_FLAG || _buffer[0] == MSG_HEAD_FLAG)             // 指令头标记
           &&_buffer[msg_len - 1] == END_FLAG                                       // 指令尾标记
           && msg_len <= _buffer_size                                               //
           )
    {
        std::lock_guard<std::mutex> lg(mutex_cv_);
        memcpy(io_reply_->data, _buffer, msg_len);
        io_reply_->len = msg_len;
        flag_cv_ = true;
        cv_.notify_one();

        if (_buffer[0] == CMD_HEAD_FLAG)
            analyze_receive_cmd_buffer();
        else if (_buffer[0] == MSG_HEAD_FLAG)
            analyze_receive_msg_buffer();


        memcpy(_buffer, _buffer + msg_len, _buffer_size - msg_len);
        _buffer_size -= msg_len;

        if (_buffer_size <= 2)
        {
            return;
        }

        msg_len = _buffer[2];
    }

//    // parse package
//    if ((_buffer[0] == CMD_HEAD_FLAG || _buffer[0] == MSG_HEAD_FLAG) && _buffer[_buffer_size - 1] == END_FLAG)
//    {



//        std::lock_guard<std::mutex> lg(mutex_cv_);
//        memcpy(io_reply_->data, _buffer, _buffer_size);
//        io_reply_->len = _buffer_size;
//        flag_cv_ = true;
//        cv_.notify_one();

//        if (_buffer[0] == CMD_HEAD_FLAG)
//            analyze_receive_cmd_buffer();
//        else if (_buffer[0] == MSG_HEAD_FLAG)
//            analyze_receive_msg_buffer();
//    }

    // clear _buffer
    memset(_buffer, 0, max_buffer_len);
    _buffer_size = 0;
}
*/
void CSerialCommandControllor::process_package(const unsigned char *pkg, size_t len)
{
    // static cache
    constexpr size_t max_buffer_len = 2 * 65535;
    static unsigned char _buffer[max_buffer_len]{0x00};
    static size_t _buffer_size = 0;

    // copy data to cache
    if (pkg == nullptr || len <= 0)
    {
        return;
    }

    if (_buffer_size + len > max_buffer_len)
    {
        _buffer_size = 0;
        return;
    }

    // 日志打印接收内容
    QString recvHex;
    for (size_t i = 0; i < len; ++i) recvHex += QString::asprintf("%02X ", pkg[i]);
    qDebug() << "[RECV][RAW]" << recvHex.trimmed();
    memcpy(_buffer + _buffer_size, pkg, len);
    _buffer_size += len;

    // 数据解析
    size_t current_offset = 0;

    while (_buffer_size > current_offset && _buffer_size > 3)
    {
        // 查找协议头部
        if (_buffer[current_offset + 0] != CMD_HEAD_FLAG && _buffer[current_offset + 0] != MSG_HEAD_FLAG)
        {
            current_offset += 1;
            break;
        }

        size_t required_len = _buffer[current_offset + 2];
        if (required_len > (_buffer_size - current_offset))
        {
            break;
        }

        std::lock_guard<std::mutex> lg(mutex_cv_);

        if (_buffer[current_offset + 1] == CMD_SIM) {
            QString simHex;
            for (size_t i = 0; i < required_len; ++i) simHex += QString::asprintf("%02X ", _buffer[current_offset + i]);
            qDebug() << "[RECV][SIM]" << simHex.trimmed();
        } else if (_buffer[current_offset + 1] == CMD_IOT) {
            QString iotHex;
            for (size_t i = 0; i < required_len; ++i) iotHex += QString::asprintf("%02X ", _buffer[current_offset + i]);
            qDebug() << "[RECV][IOT]" << iotHex.trimmed();
        } else if (_buffer[current_offset + 1] == CMD_TOTAL) {
            QString totalHex;
            for (size_t i = 0; i < required_len; ++i) totalHex += QString::asprintf("%02X ", _buffer[current_offset + i]);
            qDebug() << "[RECV][TOTAL]" << totalHex.trimmed();
        }
        if (_buffer[current_offset + 1] >= CMD_SIM && _buffer[current_offset + 1] <= CMD_VERSION)
        {
            memcpy(io_reply_->data, _buffer+ current_offset, required_len);
            io_reply_->len = required_len;
            flag_cv_ = true;
            cv_.notify_one();
        }

        if (_buffer[current_offset + 0] == CMD_HEAD_FLAG)
            analyze_receive_cmd_buffer();
        else if (_buffer[current_offset + 0] == MSG_HEAD_FLAG)
            analyze_receive_msg_buffer();

        // continue
        current_offset += required_len;
    }


    if (current_offset > 0)
    {
        memcpy(_buffer, _buffer + current_offset, _buffer_size - current_offset);
        _buffer_size -= current_offset;
    }
}

void CSerialCommandControllor::start_read_work_thread()
{
    try
    {
        if (io_read_thread_ != nullptr && io_read_thread_->joinable())
        {
            io_read_thread_->join();
        }
        delete io_read_thread_;
        io_read_flag_ = true;
        io_read_thread_ = new std::thread([&] {
            try
            {
                auto pfunc_handler = [&](const std::error_code &ec, std::size_t len,
                                         std::error_code *ec_out, std::size_t *len_out) {
                    *ec_out = ec;
                    *len_out = len;
                };

                int32_t err_cnt(0);

                // async receive
                while (io_read_flag_)
                {
                    std::size_t length = 0;
                    std::error_code ec;
                    constexpr size_t tmp_len = 512;
                    unsigned char buff[tmp_len]{0x00};
                    io_->serial.async_read_some(asio::buffer(buff, tmp_len),
                                                std::bind(pfunc_handler, std::placeholders::_1, std::placeholders::_2, &ec, &length));
                    io_->io.restart();
                    io_->io.run_for(std::chrono::milliseconds(300));
                    if (!io_->io.stopped())
                    {
                        io_->serial.cancel();
                        io_->io.run();
                    }

                    if (length <= 0)
                    {
                        err_cnt += 1;
                        if (err_cnt >= 2)
                        {
                            process_package(nullptr, 0);                            
                            err_cnt = 0;
                        }
                        continue;
                    }

                    std::cout << "io read: " << std::hex << buff << " " << length << std::endl;
                    process_package(buff, length);
                }
            }
            catch (const std::exception &e)
            {
                std::cerr << e.what() << '\n';
            }
        });
    }
    catch (const std::exception &e)
    {
        std::cerr << "start read work thread failed:" << e.what() << std::endl;
    }
}

unsigned char CSerialCommandControllor::get_cmd_index(unsigned char cmd)
{
    static unsigned char sim_index = 0;
    static unsigned char iot_index = 0;
    static unsigned char all_index = 0;
    static unsigned char ver_index = 0;

    auto funcGetCurrentIndex = [&](unsigned char& index)
    {
        return ((++index) == 0x00 ? (++index) : index);
    };

    switch (cmd)
    {
    case CMD_SIM: return funcGetCurrentIndex(sim_index);
    case CMD_IOT: return funcGetCurrentIndex(iot_index);
    case CMD_TOTAL: return funcGetCurrentIndex(all_index);
    case CMD_VERSION: return funcGetCurrentIndex(ver_index);
    default: return 0x00;
    }

    return 0x00;
}

int32_t CSerialCommandControllor::add_cmd_index(unsigned char cmd, unsigned char index)
{
    auto funcAddIndexToList = [&](std::list<unsigned char>& cmd_list)
    {
        std::list<unsigned char>::iterator it = cmd_list.begin();

        while (it != cmd_list.end())
        {
            if (index == *it) return -1;

            it++;
        }

        cmd_list.push_back(index);
        return 0;
    };

    switch (cmd)
    {
    case CMD_SIM: return funcAddIndexToList(sim_cmd_list);
    case CMD_IOT: return funcAddIndexToList(iot_cmd_list);
    case CMD_TOTAL: return funcAddIndexToList(all_cmd_list);
    case CMD_VERSION: return funcAddIndexToList(ver_cmd_list);
    default: return -1;
    }

    return -1;
}

int32_t CSerialCommandControllor::is_index_valid(unsigned char cmd, unsigned char index)
{
    auto funcIndexInList = [&](std::list<unsigned char>& cmd_list)
    {
        std::list<unsigned char>::iterator it = cmd_list.begin();

        while (it != cmd_list.end())
        {
            if (index == *it) return 0;

            it++;
        }

        return -1;
    };

    switch (cmd)
    {
    case CMD_SIM: return funcIndexInList(sim_cmd_list);
    case CMD_IOT: return funcIndexInList(iot_cmd_list);
    case CMD_TOTAL: return funcIndexInList(all_cmd_list);
    case CMD_VERSION: return funcIndexInList(ver_cmd_list);
    default: return -1;
    }

    return -1;
}

int32_t CSerialCommandControllor::remove_cmd_index(unsigned char cmd, unsigned char index)
{
    auto funcRemoveIndex = [&](std::list<unsigned char>& cmd_list)
    {
        std::list<unsigned char>::iterator it = cmd_list.begin();

        while (it != cmd_list.end())
        {
            if (index == *it)
            {
                it = cmd_list.erase(it);
                return 0;
            }

            it++;
        }

        return -1;
    };

    switch (cmd)
    {
    case CMD_SIM: return funcRemoveIndex(sim_cmd_list);
    case CMD_IOT: return funcRemoveIndex(iot_cmd_list);
    case CMD_TOTAL: return funcRemoveIndex(all_cmd_list);
    case CMD_VERSION: return funcRemoveIndex(ver_cmd_list);
    default: return -1;
    }

    return -1;
}

} // namespace zlmodules
} // namespace zl
