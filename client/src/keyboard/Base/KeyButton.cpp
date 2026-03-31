/*******************************************************************
* KeyButton.cpp
 * Copyright 2025-2029 zhilai-tech
 * description: 键盘按键按钮实现
 * 2025-12-05, niezixuan
*******************************************************************/
#include "KeyButton.h"

#include <QDebug>

/*******************************************************************
 * description: 构造函数
 * para: parent 父控件
 * return: void
 *******************************************************************/
KeyButton::KeyButton(QWidget *parent)
    : QToolButton(parent)
{
    initBtn();
    adjustFont();
}

/*******************************************************************
 * description: 析构函数
 * para: void
 * return: void
 *******************************************************************/
KeyButton::~KeyButton() = default;

/*******************************************************************
 * description: 设置按键长宽比
 * para: ratio 长宽比
 * return: void
 *******************************************************************/
auto KeyButton::setAspectRatio(double ratio) -> void
{
    if (ratio == m_aspectRatio) {
        return;
    }

    m_aspectRatio = ratio;
}

/*******************************************************************
 * description: 缩放事件处理
 * para: event 缩放事件
 * return: void
 *******************************************************************/
auto KeyButton::resizeEvent(QResizeEvent *event) -> void
{
    // adjustButtonSize();
    adjustFont();
    QToolButton::resizeEvent(event);
}

/*******************************************************************
 * description: 初始化按钮
 * para: void
 * return: void
 *******************************************************************/
auto KeyButton::initBtn() -> void
{
    constexpr int MinButtonSize = 38;
    constexpr int MaxButtonSize = 80;
    setMinimumSize(MinButtonSize, MinButtonSize);
    setMaximumSize(MaxButtonSize, MaxButtonSize);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
}

/*******************************************************************
 * description: 按钮大小自适应
 * para: void
 * return: void
 *******************************************************************/
auto KeyButton::adjustButtonSize() -> void
{
    int width = this->width();
    int height = static_cast<int>(width / m_aspectRatio);
    if (height > this->height()) {
        height = this->height();
        width = static_cast<int>(height * m_aspectRatio);
    }

    qDebug() << "width:" << width << ", height:" << height;
    setFixedSize(width, height);
}

/*******************************************************************
 * description: 文字大小自适应
 * para: void
 * return: void
 *******************************************************************/
auto KeyButton::adjustFont() -> void
{
    int buttonWidth = width();
    int buttonHeight = height();
    int fontSize = qMin(buttonWidth, buttonHeight) / 3;

    auto btnFont = font();
    btnFont.setPixelSize(fontSize);
    setFont(btnFont);

    // qDebug() << "==== adjust font" << fontSize << font().pixelSize();
}
