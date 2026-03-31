/*******************************************************************
* NumberKeyboard.h
 * Copyright 2025-2029 zhilai-tech
 * description: 数字键盘类
 * 2025-12-05, niezixuan
*******************************************************************/
#ifndef NUMBER_KEYBOARD_H
#define NUMBER_KEYBOARD_H

#include "AbstractKeyboard.h"

class KeyboardLayout;

class NumberKeyboard : public AbstractKeyboard
{
    Q_OBJECT

public:
    enum class KeyboardType
    {
        emType = 3
    };

    explicit NumberKeyboard(QWidget *parent = nullptr);
    ~NumberKeyboard() override;

    NumberKeyboard(const NumberKeyboard &) = delete;

    auto operator=(const NumberKeyboard &) -> NumberKeyboard & = delete;

    NumberKeyboard(NumberKeyboard &&) = delete;

    auto operator=(NumberKeyboard &&) -> NumberKeyboard & = delete;

    [[nodiscard]] auto type() const -> int override
    {
        return static_cast<int>(KeyboardType::emType);
    } // 获取键盘类型

    [[nodiscard]] auto name() const -> QString override; // 获取键盘名

private:
    void initKeyboard();   // 初始化布局
    void initConnection(); // 初始化连接

    void initBasicBtn();   // 初始化基础按键
    void initSpecialBtn(); // 初始化特殊按键

    static QHash<quint16, QString> m_numberHash; // 数字哈希表
    KeyboardLayout *m_layout;                    // 键盘布局对象
};

#endif // NUMBER_KEYBOARD_H
