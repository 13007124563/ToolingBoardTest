#ifndef C_SERIAL_COMMAND_CONTROLLOR_H_
#define C_SERIAL_COMMAND_CONTROLLOR_H_

#include "ISerialCommand.h"
#include <mutex>
#include <thread>
#include <condition_variable>

#include <list>

namespace zl
{
namespace zlmodules
{

class CSerialCommandControllor : public ISerialCommand
{
public:
    CSerialCommandControllor(ISerialCommandEventNotice* obs);
    ~CSerialCommandControllor();

    int32_t Connect(const std::string& port, int32_t baud) override;
    void Disconnect() override;

    int32_t SimTestCommand(const std::string& apn_config) override;

    int32_t IotTestCommand() override;

    int32_t TotalTestCommand(const std::string& apn_config) override;

    int32_t GetVersionCommand() override;
protected:

    int32_t cmd_sim_test_command(const std::string& apn_config);

    int32_t cmd_iot_test_command();

    int32_t cmd_total_test_command(const std::string& apn_config);

    int32_t cmd_get_version_command();

    int32_t analyze_receive_cmd_buffer();
    int32_t analyze_receive_msg_buffer();

    bool cv_wait(int32_t timeout = 1000);
    size_t sync_write_cmd(unsigned char* buff, size_t len, int32_t timeout = 300);
    void process_package(const unsigned char* pkg, size_t len);
    void start_read_work_thread();

    // 获取当前命令的指令索引号
    unsigned char get_cmd_index(unsigned char cmd);

    // 将指令索引号 加入保存队列
    int32_t add_cmd_index(unsigned char cmd, unsigned char index);

    // 判断指令索引号 是否在保存队列中
    int32_t is_index_valid(unsigned char cmd, unsigned char index);

    // 从保存队列中 将指令索引号删除
    int32_t remove_cmd_index(unsigned char cmd, unsigned char index);
private:
    struct IOImpl;
    IOImpl *io_;

    std::mutex mutex_;
    std::mutex mutex_cv_;
    std::condition_variable cv_;
    bool flag_cv_;
    struct IOReply;
    IOReply* io_reply_;

    bool io_read_flag_;
    std::thread* io_read_thread_;

    ISerialCommandEventNotice* obs_;

    std::list<unsigned char> sim_cmd_list;
    std::list<unsigned char> iot_cmd_list;
    std::list<unsigned char> all_cmd_list;
    std::list<unsigned char> ver_cmd_list;
};

} // namespace zlmodules
} // namespace zl

#endif // C_SERIAL_COMMAND_CONTROLLOR_H_
