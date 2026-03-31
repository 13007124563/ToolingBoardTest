/*******************************************************************
* KeyboardTimer.cpp
 * Copyright 2025-2029 zhilai-tech
 * description: 键盘定时器实现
 * 2025-12-05, niezixuan
*******************************************************************/
#include "KeyboardTimer.h"

/*******************************************************************
 * description: 构造函数
 * para: parent 父对象
 * return: void
 *******************************************************************/
KeyboardTimer::KeyboardTimer(QObject *parent)
    : QObject{parent}
{
}

/*******************************************************************
 * description: 析构函数
 * para: void
 * return: void
 *******************************************************************/
KeyboardTimer::~KeyboardTimer() = default;
