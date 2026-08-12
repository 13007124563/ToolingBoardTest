#ifndef CALPHALOOK3DCONTROLLOR_H_
#define CALPHALOOK3DCONTROLLOR_H_

#include "IAlphaLook3D.h"
#include <mutex>
#include <thread>
#include <condition_variable>


namespace zl
{
namespace zlmodules
{

class CAlphaLook3DControllor : public IAlphaLook3D
{
public:
    CAlphaLook3DControllor(IAlphaLook3DEventNotice* obs);
    ~CAlphaLook3DControllor();

    int32_t Connect(const std::string& port, int32_t baud) override;
    void Disconnect() override;

    int32_t Reset() override;
    int32_t GetStatus() override;
    int32_t Verify(uint8_t flag) override;
    int32_t Enroll() override;
    int32_t GetImage() override;
    int32_t DelUser(int32_t user_id) override;
    int32_t DelAll() override;
    int32_t PowerDown() override;

protected:
    int32_t cmd_reset();
    int32_t cmd_get_status();
    int32_t cmd_verify(uint8_t flag);
    int32_t cmd_enroll();
    int32_t cmd_get_image();
    int32_t cmd_del_user(int32_t user_id);
    int32_t cmd_del_all();

    bool cv_wait(int32_t timeout = 200);
    uint8_t generate_bcc_checksum(const unsigned char* buff, size_t len);
    int32_t convert_userid(int32_t userid);
    size_t sync_write_cmd(unsigned char* buff, size_t len, int32_t timeout = 2000);
    void process_package(const unsigned char* pkg, size_t len);
    void process_reply_package(const unsigned char* pkg, size_t len);
    void process_note_package(const unsigned char* pkg, size_t len);
    void process_image_package(const unsigned char* pkg, size_t len);
    void start_read_work_thread();

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

    IAlphaLook3DEventNotice* obs_;
};

} // namespace zlmodules
} // namespace zl

#endif // CALPHALOOK3DCONTROLLOR_H_
