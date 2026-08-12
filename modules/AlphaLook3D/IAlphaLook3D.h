/****************************************************************************
**
** @file IAlphaLook3D.h
** @brief AlphaLook3D人脸识别模块控制接口
** 
** 该文件定义了控制模块的接口以及相关的事件处理接口，方便开发者使用该人脸识别模块，
** 省去了直接处理串口数据报文的麻烦
** 
** @author xiongpengchao@chinawebox.com
** @version 1.0.0
** @date 2019-10-21
****************************************************************************/

#ifndef IALPHALOOK3D_H_
#define IALPHALOOK3D_H_

#include <iostream>

namespace zl
{
namespace zlmodules
{

/// \brief AlphaLook3D回传事件通知接口
///
/// 当人脸识别模块有主动上传事件时，会调用对应的事件回调接口，当用户需要处理相关的事件
/// 时，应该重新实现该接口 
class IAlphaLook3DEventNotice
{
public:
    IAlphaLook3DEventNotice() = default;
    virtual ~IAlphaLook3DEventNotice() = default;

    /// \brief 人脸识别结束事件
    ///
    /// \param[in] result 人脸验证的结果
    /// \param[in] user_id 识别出的用户ID（如果识别失败为0）
    virtual void EventVerifyResult(uint8_t result, int32_t user_id) {};

    /// \brief 人脸录入结束事件
    ///
    /// \param[in] result 人脸录入结果
    /// \param[in] user_id 人脸录入成功后返回的用户ID（如果失败为0）
    virtual void EventEnrollResult(uint8_t result, int32_t user_id) {};

    /// \brief 模块就绪事件
    virtual void EventNoteReady() {};

    /// \brief 人脸识别状态信息
    ///
    /// \param[in] code 当前检测到的人脸信息状态
    /// \param[in] lf 人脸录入进度（向左），仅在人脸录入过程中有效
    /// \param[in] sf 人脸录入进度（正脸），仅在人脸录入过程中有效
    /// \param[in] rf 人脸录入进度（向右），仅在人脸录入过程中有效
    virtual void EventNoteFaceState(uint8_t code, bool lf, bool sf, bool rf) {};

    /// \brief 人脸图像上传事件
    virtual void EventNoteImage(const unsigned char* image_data, size_t len) {};

};

/// \brief AlphaLook3D接口
///
/// AlphaLook3D人脸识别模块接口
class IAlphaLook3D
{
public:
    virtual ~IAlphaLook3D() = default;

    virtual int32_t Connect(const std::string& port, int32_t baud) = 0;

    virtual void Disconnect() = 0;

    /// \brief 重置模块
    ///
    /// 停止所有当前在处理的消息，模块进入standby状态
    virtual int32_t Reset() = 0;

    /// \brief 立即返回模块当前状态
    virtual int32_t GetStatus() = 0;

    /// \brief 鉴权解锁
    virtual int32_t Verify(uint8_t flag) = 0;

    /// \brief 新用户录入
    virtual int32_t Enroll() = 0;

    /// \brief 传送图片
    virtual int32_t GetImage() = 0;

    /// \brief 删除一个注册用户
    virtual int32_t DelUser(int32_t user_id) = 0;

    /// \brief 删除所有注册用户
    virtual int32_t DelAll() = 0;

    /// \brief 模块关机
    ///
    /// 模块准备关机，结束当前在处理的任务，清除文件系统缓存
    virtual int32_t PowerDown() = 0;
};

} // namespace zlmodules
} // namespace zl

#endif // IALPHALOOK3D_H_
