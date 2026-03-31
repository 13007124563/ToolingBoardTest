/*******************************************************************
* ChineseKeyboard.h
 * Copyright 2025-2029 zhilai-tech
 * description: 中文键盘类
 * 2025-12-05, niezixuan
*******************************************************************/
#ifndef CHINESE_KEYBOARD_H
#define CHINESE_KEYBOARD_H

#include <QMultiHash>

#include "AbstractKeyboard.h"

class KeyboardLayout;

class ChineseKeyboard : public AbstractKeyboard
{
    Q_OBJECT

public:
    enum class KeyboardType
    {
        emType = 2
    };

    explicit ChineseKeyboard(QWidget *parent = nullptr);
    ~ChineseKeyboard() override;

    ChineseKeyboard(const ChineseKeyboard &) = delete;

    auto operator=(const ChineseKeyboard &) -> ChineseKeyboard & = delete;

    ChineseKeyboard(ChineseKeyboard &&) = delete;

    auto operator=(ChineseKeyboard &&) -> ChineseKeyboard & = delete;

    [[nodiscard]] auto type() const -> int override
    {
        return static_cast<int>(KeyboardType::emType);
    } // 获取键盘类型

    [[nodiscard]] auto name() const -> QString override; // 获取键盘名
    [[nodiscard]] auto upper() const -> bool override;   // 获取大小写切换
    auto setUpper(bool enable) -> void override;         // 大小写切换

    [[nodiscard]] auto matched(const QString &input) const -> QStringList override; // 获取候选词

private:
    void initKeyboard();   // 初始化布局
    void initConnection(); // 初始化连接

    void initBasicBtn();                               // 初始化基础按键
    void initSpecialBtn();                             // 初始化特殊按键
    void readDictionary();                             // 读取字典
    void splitPhrase(QString phrase, QString chinese); // 解析拼音词组

    static QHash<quint16, QString> m_letterHash; // 字母哈希表
    bool m_upperEnable{};                        // 大写使能
    KeyboardLayout *m_layout;                    // 键盘布局对象
    QMultiHash<QString, QString> m_chineseHash;  // 中文哈希表
};

#endif // CHINESE_KEYBOARD_H
