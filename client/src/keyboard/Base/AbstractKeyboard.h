/*******************************************************************
* AbstractKeyboard.h
 * Copyright 2025-2029 zhilai-tech
 * description: 抽象键盘基类
 * 2025-12-05, niezixuan
*******************************************************************/
#ifndef ABSTRACT_KEYBOARD_H
#define ABSTRACT_KEYBOARD_H

#include <QWidget>

class QAbstractButton;

class AbstractKeyboard : public QWidget
{
    Q_OBJECT

public:
    enum class KeyboardType
    {
        emType = -1
    };

    enum class KeyType
    {
        UnknownKey,   // 未知按键
        NormalKey,    // 普通按键
        SwitchKey,    // 布局切换键
        ShiftKey,     // 大小写切换键
        SpaceKey,     // 空格键
        BackspaceKey, // 退格键
    };

Q_SIGNALS:
    void buttonClicked(const QWidget *text); // 按钮点击事件

public:
    explicit AbstractKeyboard(QWidget *parent = nullptr);
    virtual ~AbstractKeyboard() = default;

    [[nodiscard]] virtual auto name() const -> QString = 0; // 获取键盘名

    [[nodiscard]] virtual auto type() const -> int = 0; // 获取键盘类型

    [[nodiscard]] virtual auto upper() const -> bool; // 获取大小写切换
    virtual auto setUpper(bool enable) -> void;       // 大小写切换

    [[nodiscard]] virtual auto matched(const QString &input) const -> QStringList; // 获取匹配词

    // 禁止拷贝和移动（抽象基类）

    AbstractKeyboard(const AbstractKeyboard &) = delete;

    auto operator=(const AbstractKeyboard &) -> AbstractKeyboard & = delete;

    AbstractKeyboard(AbstractKeyboard &&) = delete;

    auto operator=(AbstractKeyboard &&) -> AbstractKeyboard & = delete;
};

#endif // ABSTRACT_KEYBOARD_H
