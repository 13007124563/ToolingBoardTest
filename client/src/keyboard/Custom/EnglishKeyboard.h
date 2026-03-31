/*******************************************************************
* EnglishKeyboard.h
 * Copyright 2025-2029 zhilai-tech
 * description: 英文键盘类
 * 2025-12-05, niezixuan
*******************************************************************/
#ifndef ENGLISH_KEYBOARD_H
#define ENGLISH_KEYBOARD_H

#include "AbstractKeyboard.h"

class KeyboardLayout;

class EnglishKeyboard : public AbstractKeyboard
{
    Q_OBJECT

public:
    enum class KeyboardType
    {
        emType = 1
    };

    explicit EnglishKeyboard(QWidget *parent = nullptr);
    ~EnglishKeyboard() override;

    EnglishKeyboard(const EnglishKeyboard &) = delete;

    auto operator=(const EnglishKeyboard &) -> EnglishKeyboard & = delete;

    EnglishKeyboard(EnglishKeyboard &&) = delete;

    auto operator=(EnglishKeyboard &&) -> EnglishKeyboard & = delete;

    [[nodiscard]] auto type() const -> int override
    {
        return static_cast<int>(KeyboardType::emType);
    } // 获取键盘类型

    [[nodiscard]] auto name() const -> QString override; // 获取键盘名
    [[nodiscard]] auto upper() const -> bool override;   // 获取大小写切换
    auto setUpper(bool enable) -> void override;         // 大小写切换

private:
    void initKeyboard();   // 初始化布局
    void initConnection(); // 初始化连接

    void initBasicBtn();   // 初始化基础按键
    void initSpecialBtn(); // 初始化特殊按键

    static QHash<quint16, QString> m_letterHash; // 字母哈希表
    bool m_upperEnable{};                        // 大写使能
    KeyboardLayout *m_layout;                    // 键盘布局对象
};

#endif // ENGLISH_KEYBOARD_H
