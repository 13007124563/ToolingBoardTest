/*******************************************************************
* SymbolKeyboard.h
 * Copyright 2025-2029 zhilai-tech
 * description: 符号键盘类
 * 2025-12-05, niezixuan
*******************************************************************/
#ifndef SYMBOL_KEYBOARD_H
#define SYMBOL_KEYBOARD_H

#include "AbstractKeyboard.h"

class KeyboardLayout;

class SymbolKeyboard : public AbstractKeyboard
{
    Q_OBJECT

public:
    enum class KeyboardType
    {
        emType = 4
    };

    explicit SymbolKeyboard(QWidget *parent = nullptr);
    ~SymbolKeyboard() override;

    SymbolKeyboard(const SymbolKeyboard &) = delete;

    auto operator=(const SymbolKeyboard &) -> SymbolKeyboard & = delete;

    SymbolKeyboard(SymbolKeyboard &&) = delete;

    auto operator=(SymbolKeyboard &&) -> SymbolKeyboard & = delete;

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

    static QHash<quint16, QString> m_symbolHash; // 符号哈希表
    KeyboardLayout *m_layout;                    // 键盘布局对象
};

#endif // SYMBOL_KEYBOARD_H
