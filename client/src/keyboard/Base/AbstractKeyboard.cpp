/*******************************************************************
* AbstractKeyboard.cpp
 * Copyright 2025-2029 zhilai-tech
 * description: 抽象键盘基类实现
 * 2025-12-05, niezixuan
*******************************************************************/
#include "AbstractKeyboard.h"

/*******************************************************************
 * description: 构造函数
 * para: parent 父控件
 * return: void
 *******************************************************************/
AbstractKeyboard::AbstractKeyboard(QWidget *parent)
    : QWidget{parent}
{
}

/*******************************************************************
 * description: 获取大小写切换状态
 * para: void
 * return: bool 是否为大写模式
 *******************************************************************/
auto AbstractKeyboard::upper() const -> bool
{
    return false;
}

/*******************************************************************
 * description: 设置大小写切换
 * para: enable 是否启用大写模式
 * return: void
 *******************************************************************/
auto AbstractKeyboard::setUpper(bool enable) -> void
{
}

/*******************************************************************
 * description: 获取匹配词
 * para: input 输入字符串
 * return: QStringList 匹配词列表
 *******************************************************************/
auto AbstractKeyboard::matched(const QString &input) const -> QStringList
{
    return {};
}
