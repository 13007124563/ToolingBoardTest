#include "CAlphaLook3DControllor.h"
#include <sstream>
#include <iomanip>

#include "asio-1.12.2/asio.hpp"
#include "bytes_convert.h"

// message id
#define SYNC_WORD1 0xEA
#define SYNC_WORD2 0x55

#define MID_RESET 0x10
#define MID_GETSTATUS 0x11
#define MID_VERIFY 0x12
#define MID_ENROLL 0x13
#define MID_GETIMAGE 0x14
#define MID_DELUSER 0x20
#define MID_DELALL 0x21
#define MID_POWERDOWN 0xED

#define MID_REPLY 0x00
#define MID_NOTE 0x01
#define MID_IMAGE 0x02

namespace zl
{
namespace zlmodules
{

struct CAlphaLook3DControllor::IOReply
{
    uint8_t mid;
    uint8_t result;
    unsigned char data[1024];
    size_t len;

    IOReply() : data{0x00}, len(0) {}
};

struct CAlphaLook3DControllor::IOImpl
{
    asio::io_context io;
    asio::serial_port serial;
    IOImpl() : serial(io)
    {
    }
};

CAlphaLook3DControllor::CAlphaLook3DControllor(IAlphaLook3DEventNotice* obs)
    : io_(new IOImpl()),
      io_reply_(new IOReply()),
      io_read_flag_(false),
      io_read_thread_(nullptr),
      obs_(obs)
{
}

CAlphaLook3DControllor::~CAlphaLook3DControllor()
{
    Disconnect();
    if (io_ != nullptr)
    {
        delete io_;
        io_ = nullptr;
    }
}

int32_t CAlphaLook3DControllor::Connect(const std::string &port, int32_t baud)
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
        return -1;
    }
}

void CAlphaLook3DControllor::Disconnect()
{
    try
    {    if (io_ != nullptr)
        {
            io_->serial.close();
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
        std::cerr << e.what() << std::endl;
    }
}

int32_t CAlphaLook3DControllor::Reset()
{
    std::unique_lock<std::mutex> ul(mutex_);
    std::cout << "reset device" << std::endl;

    // send command
    auto ret = cmd_reset();
    if (ret != 0)
    {
        std::cerr << "send cmd reset failed: " << ret << std::endl;
        return -1;
    }

    // wait for reply
    if (!cv_wait(200))
    {
        return -1;
    }

    // todo process data
    if (io_reply_->mid != MID_RESET)
    {
        std::cerr << "unmatched mid: " << io_reply_->mid << std::endl;
        return -1;
    }

    return io_reply_->result;
}

int32_t CAlphaLook3DControllor::GetStatus()
{
    std::unique_lock<std::mutex> ul(mutex_);
    std::cout << "get status" << std::endl;

    // send command
    auto ret = cmd_get_status();
    if (ret != 0)
    {
        std::cerr << "send cmd getstatus failed: " << ret << std::endl;
        return -1;
    }

    // wait for reply
    if (!cv_wait(200))
    {
        return -1;
    }

    // todo process reply
    if (io_reply_->mid != MID_GETSTATUS)
    {
        std::cerr << "unmatched mid: " << io_reply_->mid << std::endl;
        return -1;
    }

    return io_reply_->data[0];
}

int32_t CAlphaLook3DControllor::Verify(uint8_t flag)
{
    std::unique_lock<std::mutex> ul(mutex_);
    std::cout << "verify " << flag << std::endl;

    // send command
    auto ret = cmd_verify(flag);
    if (ret != 0)
    {
        std::cerr << "send cmd verify failed: " << ret << std::endl;
        return -1;
    }

    return 0;
}

int32_t CAlphaLook3DControllor::Enroll()
{
    std::unique_lock<std::mutex> ul(mutex_);
    std::cout << "enroll" << std::endl;

    // send command
    auto ret = cmd_enroll();
    if (ret != 0)
    {
        std::cerr << "send cmd enroll failed: " << ret << std::endl;
        return -1;
    }

    return 0;
}

int32_t CAlphaLook3DControllor::GetImage()
{
    std::unique_lock<std::mutex> ul(mutex_);
    std::cout << "get image" << std::endl;

    // send command
    auto ret = cmd_get_image();
    if (ret != 0)
    {
        std::cerr << "send cmd get image failed: " << ret << std::endl;
        return -1;
    }

    return 0;
}

int32_t CAlphaLook3DControllor::DelUser(int32_t user_id)
{
    std::unique_lock<std::mutex> ul(mutex_);
    std::cout << "deluser: " << user_id << std::endl;

    // send command
    auto ret = cmd_del_user(user_id);
    if (ret != 0)
    {
        std::cerr << "send cmd deluser failed: " << ret << std::endl;
        return -1;
    }

    // wait for reply
    if (!cv_wait(200))
    {
        return -1;
    }

    // todo process reply
    if (io_reply_->mid != MID_DELUSER)
    {
        std::cerr << "unmatched mid: " << io_reply_->mid << std::endl;
        return -1;
    }

    return io_reply_->result;
}

int32_t CAlphaLook3DControllor::DelAll()
{
    std::unique_lock<std::mutex> ul(mutex_);
    std::cout << "delall" << std::endl;

    // send command
    auto ret = cmd_del_all();
    if (ret != 0)
    {
        std::cerr << "send cmd delall failed: " << ret << std::endl;
        return -1;
    }

    // wait for reply
    if (!cv_wait(200))
    {
        return -1;
    }

    // todo process reply
    if (io_reply_->mid != MID_DELALL)
    {
        std::cerr << "unmatched mid: " << io_reply_->mid << std::endl;
        return -1;
    }

    return io_reply_->result;
}

int32_t CAlphaLook3DControllor::PowerDown()
{
    return 0;
}

int32_t CAlphaLook3DControllor::cmd_reset()
{
    // command
    constexpr size_t cmd_len = 6;
    unsigned char cmd[cmd_len]{0x00};
    cmd[0] = SYNC_WORD1;
    cmd[1] = SYNC_WORD2;
    cmd[2] = MID_RESET;
    cmd[3] = 0x00;
    cmd[4] = 0x00;
    cmd[5] = generate_bcc_checksum(cmd + 2, cmd_len - 3);

    // write command
    auto ret = sync_write_cmd(cmd, cmd_len);
    if (ret < cmd_len)
    {
        std::cerr << "write to io failed:" << ret << std::endl;
        return -1;
    }
    return 0;
}

int32_t CAlphaLook3DControllor::cmd_get_status()
{
    // command
    constexpr size_t cmd_len = 6;
    unsigned char cmd[cmd_len]{0x00};
    cmd[0] = SYNC_WORD1;
    cmd[1] = SYNC_WORD2;
    cmd[2] = MID_GETSTATUS;
    cmd[3] = 0x00;
    cmd[4] = 0x00;
    cmd[5] = generate_bcc_checksum(cmd + 2, cmd_len - 3);

    // write command
    auto ret = sync_write_cmd(cmd, cmd_len);
    if (ret < cmd_len)
    {
        std::cerr << "write to io failed:" << ret << std::endl;
        return -1;
    }
    return 0;
}

int32_t CAlphaLook3DControllor::cmd_verify(uint8_t flag)
{
    // command
    constexpr size_t cmd_len = 7;
    unsigned char cmd[cmd_len]{0x00};
    cmd[0] = SYNC_WORD1;
    cmd[1] = SYNC_WORD2;
    cmd[2] = MID_VERIFY;
    cmd[3] = 0x00;
    cmd[4] = 0x01;
    cmd[5] = 0x00;
    cmd[6] = generate_bcc_checksum(cmd + 2, cmd_len - 3);

    // write command
    auto ret = sync_write_cmd(cmd, cmd_len);
    if (ret < cmd_len)
    {
        std::cerr << "write to io failed:" << ret << std::endl;
        return -1;
    }
    return 0;
}

int32_t CAlphaLook3DControllor::cmd_enroll()
{
    // command
    constexpr size_t cmd_len = 6;
    unsigned char cmd[cmd_len]{0x00};
    cmd[0] = SYNC_WORD1;
    cmd[1] = SYNC_WORD2;
    cmd[2] = MID_ENROLL;
    cmd[3] = 0x00;
    cmd[4] = 0x00;
    cmd[5] = generate_bcc_checksum(cmd + 2, cmd_len - 3);

    // write command
    auto ret = sync_write_cmd(cmd, cmd_len);
    if (ret < cmd_len)
    {
        std::cerr << "write to io failed:" << ret << std::endl;
        return -1;
    }
    return 0;
}

int32_t CAlphaLook3DControllor::cmd_get_image()
{
    // command
    constexpr size_t cmd_len = 6;
    unsigned char cmd[cmd_len]{0x00};
    cmd[0] = SYNC_WORD1;
    cmd[1] = SYNC_WORD2;
    cmd[2] = MID_GETIMAGE;
    cmd[3] = 0x00;
    cmd[4] = 0x00;
    cmd[5] = generate_bcc_checksum(cmd + 2, cmd_len - 3);

    // write command
    auto ret = sync_write_cmd(cmd, cmd_len);
    if (ret < cmd_len)
    {
        std::cerr << "write to io failed:" << ret << std::endl;
        return -1;
    }
    return 0;
}

int32_t CAlphaLook3DControllor::cmd_del_user(int32_t user_id)
{
    // command
    auto v = convert_userid(user_id);
    char *ptr = (char *)&v;

    constexpr size_t cmd_len = 10;
    unsigned char cmd[cmd_len]{0x00};
    cmd[0] = SYNC_WORD1;
    cmd[1] = SYNC_WORD2;
    cmd[2] = MID_DELUSER;
    cmd[3] = 0x00;
    cmd[4] = 0x04;
    cmd[5] = ptr[0];
    cmd[6] = ptr[1];
    cmd[7] = ptr[2];
    cmd[8] = ptr[3];
    cmd[9] = generate_bcc_checksum(cmd + 2, cmd_len);

    // write command
    auto ret = sync_write_cmd(cmd, cmd_len);
    if (ret < cmd_len)
    {
        std::cerr << "write to io failed:" << ret << std::endl;
        return -1;
    }
    return 0;
}

int32_t CAlphaLook3DControllor::cmd_del_all()
{
    // command
    constexpr size_t cmd_len = 6;
    unsigned char cmd[cmd_len]{0x00};
    cmd[0] = SYNC_WORD1;
    cmd[1] = SYNC_WORD2;
    cmd[2] = MID_DELALL;
    cmd[3] = 0x00;
    cmd[4] = 0x00;
    cmd[5] = generate_bcc_checksum(cmd + 2, cmd_len - 2);

    // write command
    auto ret = sync_write_cmd(cmd, cmd_len);
    if (ret < cmd_len)
    {
        std::cerr << "write to io failed:" << ret << std::endl;
        return -1;
    }

    return 0;
}

bool CAlphaLook3DControllor::cv_wait(int32_t timeout)
{
    // wait for reply
    std::unique_lock<std::mutex> ul2(mutex_cv_);
    flag_cv_ = false;
    io_reply_->mid = 0xFF;
    io_reply_->result = 0xFF;
    io_reply_->len = 0;

    auto st = cv_.wait_for(ul2, std::chrono::milliseconds(timeout), [&] { return flag_cv_; });
    if (!st)
    {
        std::cerr << "wait for reply timeout" << std::endl;
        return false;
    }
    return true;
}

uint8_t CAlphaLook3DControllor::generate_bcc_checksum(const unsigned char *buff, size_t len)
{
    if (buff == nullptr || len <= 0)
    {
        return 0x00;
    }

    uint8_t sum = 0x00;
    for (size_t i = 0; i < len; ++i)
    {
        sum ^= buff[i];
    }

    return sum;
}

int32_t CAlphaLook3DControllor::convert_userid(int32_t userid)
{
    return ByteOrder::host_to_be(userid);
}

size_t CAlphaLook3DControllor::sync_write_cmd(unsigned char *buff, size_t len, int32_t timeout)
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

void CAlphaLook3DControllor::process_package(const unsigned char *pkg, size_t len)
{
    if (pkg == nullptr || len <= 6)
    {
        std::cerr << "invalid package size" << std::endl;
        return;
    }

    uint8_t type = pkg[2];
    std::cout << "new pkg: " << std::hex << pkg << " " << len << std::endl;

    switch (type)
    {
    case MID_REPLY:
        return process_reply_package(pkg, len);
    case MID_NOTE:
        return process_note_package(pkg, len);
    case MID_IMAGE:
        return process_image_package(pkg, len);
    default:
        std::cerr << "unknown package type: " << type << std::endl;
    }
}

void CAlphaLook3DControllor::process_reply_package(const unsigned char *pkg, size_t len)
{
    std::cout << "process reply package" << std::endl;
    size_t data_len = pkg[3] * 256 + pkg[4];
    if (data_len < 2 || data_len > len)
    {
        std::cerr << "invalid reply data len:" << data_len << std::endl;
        return;
    }

    // parse package
    uint8_t pmid = pkg[5];
    uint8_t result = pkg[6];
    int32_t tmp(0), user_id(0);
    if (data_len == 6)
    {
        memcpy(&tmp, pkg + 7, 4);
        user_id = ByteOrder::be_to_host(tmp);
    }

    // notify
    switch (pmid)
    {
    case MID_VERIFY:
        if (obs_ != nullptr)
        {
            obs_->EventVerifyResult(result, user_id);
        }
        break;
    case MID_ENROLL:
        if (obs_ != nullptr)
        {
            obs_->EventEnrollResult(result, user_id);
        }
        break;
    default:
        {
            std::lock_guard<std::mutex> lg(mutex_cv_);
            io_reply_->mid = pmid;
            io_reply_->result = result;
            io_reply_->len = data_len;
            memcpy(io_reply_->data, pkg + 7, data_len);
            flag_cv_ = true;
            cv_.notify_one();
        }
    }
}

void CAlphaLook3DControllor::process_note_package(const unsigned char *pkg, size_t len)
{
    std::cout << "process note package" << std::endl;
    size_t data_len = pkg[3] * 256 + pkg[4];
    if (data_len < 1 || data_len > len)
    {
        std::cerr << "invalid note data len:" << data_len << std::endl;
        return;
    }

    // parse pkg
    uint8_t nid = pkg[5];

    if (obs_ == nullptr)
    {
        // no event notice object is setup
        return;
    }

    if (nid == 0)
    {
        // todo ignore ready event
        obs_->EventNoteReady();
        return;
    }

    // get face state
    bool lf(false), sf(false), rf(false);
    if ((nid & 0x80) == 0x80)
    {
        lf = (nid & 0x81) == 0x81;
        sf = (nid & 0x82) == 0x82;
        rf = (nid & 0x84) == 0x84;
        nid = 0;
    }
    obs_->EventNoteFaceState(nid, lf, sf, rf);
}

void CAlphaLook3DControllor::process_image_package(const unsigned char *pkg, size_t len)
{
    std::cout << "process image package" << std::endl;

    const unsigned char* data = pkg + 5;
    size_t data_len = pkg[3] * 256 + pkg[4];

    if (obs_ != nullptr)
    {
        obs_->EventNoteImage(data, data_len);
    }
}

void CAlphaLook3DControllor::start_read_work_thread()
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
                // static cache
                constexpr size_t max_buffer_len = 2 * 65535;
                static unsigned char _buffer[max_buffer_len]{0x00};
                static size_t _buffer_size = 0;

                auto pfunc_handler = [&](const std::error_code &ec, std::size_t len,
                                         std::error_code *ec_out, std::size_t *len_out) {
                    *ec_out = ec;
                    *len_out = len;
                };

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
                        continue;
                    }

                    memcpy(_buffer + _buffer_size, buff, length);
                    _buffer_size += length;
                    std::cout << "io read: " << std::hex << buff << " " << length << std::endl;
                    // parse package
                    size_t current_offset = 0;
                    while (current_offset < _buffer_size - 1)
                    {
                        if (_buffer[current_offset] == SYNC_WORD1 &&
                                _buffer[current_offset + 1] == SYNC_WORD2)
                        {
                            // get new head
                            size_t pkg_len = _buffer[current_offset + 3] * 256 +
                                             _buffer[current_offset + 4] + 6;
                            if (pkg_len > _buffer_size)
                            {
                                // continue to read
                                break;
                            }

                            // get package
                            process_package(_buffer + current_offset, pkg_len);

                            // remove pakcage
                            _buffer_size -= pkg_len;
                            memcpy(_buffer, _buffer + current_offset, _buffer_size);
                            current_offset = 0;
                            continue;
                        }

                        current_offset++;
                    }
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

} // namespace zlmodules
} // namespace zl
