/*******************************************************************
* KeyButton.h
 * Copyright 2025-2029 zhilai-tech
 * description: 键盘按键按钮类
 * 2025-12-05, niezixuan
*******************************************************************/
#ifndef KEY_BUTTON_H
#define KEY_BUTTON_H

#include <QToolButton>

class KeyButton : public QToolButton
{
    Q_OBJECT

public:
    explicit KeyButton(QWidget *parent = nullptr);
    ~KeyButton() override;

    // 禁止拷贝和移动（Qt 对象）
    KeyButton(const KeyButton &) = delete;
    auto operator=(const KeyButton &) -> KeyButton & = delete;
    KeyButton(KeyButton &&) = delete;
    auto operator=(KeyButton &&) -> KeyButton & = delete;

    auto setAspectRatio(double ratio) -> void; // 设置长宽比

protected:
    auto resizeEvent(QResizeEvent *event) -> void override;

private:
    auto initBtn() -> void;          // 初始化按钮
    auto adjustButtonSize() -> void; // 按钮大小自适应
    auto adjustFont() -> void;       // 文字大小自适应
    double m_aspectRatio = 1.0;      // 按键长宽比
};

#endif // KEY_BUTTON_H
