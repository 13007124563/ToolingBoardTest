/*******************************************************************
* KeyboardTimer.h
 * Copyright 2025-2029 zhilai-tech
 * description: 键盘定时器类
 * 2025-12-05, niezixuan
*******************************************************************/
#ifndef KEYBOARD_TIMER_H
#define KEYBOARD_TIMER_H

#include <QObject>

class KeyboardTimer : public QObject
{
    Q_OBJECT
public:
    explicit KeyboardTimer(QObject *parent = nullptr);
    ~KeyboardTimer() override;
};

#endif // KEYBOARD_TIMER_H
