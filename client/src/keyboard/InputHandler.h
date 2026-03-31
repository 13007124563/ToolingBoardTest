/*******************************************************************
* InputHandler.h
 * Copyright 2025-2029 zhilai-tech
 * description: 输入事件处理类
 * 2025-12-05, niezixuan
*******************************************************************/
#ifndef INPUTHANDLER_H
#define INPUTHANDLER_H

#include <QObject>
#include <QTextCursor>

class InputHandler : public QObject
{
    Q_OBJECT

public:
    explicit InputHandler(QObject *parent = nullptr);
    ~InputHandler() override;

    auto setEditableWidget(QWidget *widget) -> InputHandler *; // 设置可编辑的控件
    auto write(const QString &text) -> void;                   // 写入文字
    auto backspace(int choped = 1) -> void;                    // 删除文字

private:
    QWidget *m_editableWidget; // 可编辑的控件
    QTextCursor m_cursor;      // 输入光标
};

#endif // INPUTHANDLER_H
