/*******************************************************************
* InputHandler.cpp
 * Copyright 2025-2029 zhilai-tech
 * description: 输入处理类实现
 * 2025-12-05, niezixuan
*******************************************************************/
#include "InputHandler.h"

#include <QComboBox>
#include <QDebug>
#include <QLineEdit>
#include <QObject>
#include <QPlainTextEdit>
#include <QTextBrowser>
#include <QTextCursor>
#include <QTextEdit>
#include <QWidget>

/*******************************************************************
 * description: 构造函数
 * para: parent 父对象
 * return: void
 *******************************************************************/
InputHandler::InputHandler(QObject *parent)
    : QObject{parent}
    , m_editableWidget(nullptr)
    , m_cursor()
{
}

/*******************************************************************
 * description: 析构函数
 * para: void
 * return: void
 *******************************************************************/
InputHandler::~InputHandler() = default;

/*******************************************************************
 * description: 设置可编辑的控件
 * para: widget 可编辑控件
 * return: InputHandler* 自身指针
 *******************************************************************/
auto InputHandler::setEditableWidget(QWidget *widget) -> InputHandler *
{
    if (widget == m_editableWidget) {
        return this;
    }

    m_editableWidget = widget;
    if (QTextEdit *textEdit = qobject_cast<QTextEdit *>(widget)) {
        m_cursor = textEdit->textCursor();
    } else if (QPlainTextEdit *plainTextEdit = qobject_cast<QPlainTextEdit *>(widget)) {
        m_cursor = plainTextEdit->textCursor();
    } else if (QTextBrowser *textBrowser = qobject_cast<QTextBrowser *>(widget)) {
        m_cursor = textBrowser->textCursor();
    }

    return this;
}

/*******************************************************************
 * description: 添加文字到光标处
 * para: text 要添加的文字
 * return: void
 *******************************************************************/
auto InputHandler::write(const QString &text) -> void
{
    if (!m_editableWidget) {
        return;
    }

    if (QLineEdit *lineEdit = qobject_cast<QLineEdit *>(m_editableWidget)) {
        lineEdit->insert(text);
    } else if (QComboBox *comboBox = qobject_cast<QComboBox *>(m_editableWidget)) {
        if (comboBox->isEditable()) {
            comboBox->lineEdit()->insert(text);
        }
    } else if (QTextEdit *textEdit = qobject_cast<QTextEdit *>(m_editableWidget)) {
        m_cursor.insertText(text);
        textEdit->setTextCursor(m_cursor);
    } else if (QPlainTextEdit *plainTextEdit = qobject_cast<QPlainTextEdit *>(m_editableWidget)) {
        m_cursor.insertText(text);
        plainTextEdit->setTextCursor(m_cursor);
    } else if (QTextBrowser *textBrowser = qobject_cast<QTextBrowser *>(m_editableWidget)) {
        m_cursor.insertText(text);
        textBrowser->setTextCursor(m_cursor);
    }
}

/*******************************************************************
 * description: 删除光标处的文字
 * para: choped 删除字符数
 * return: void
 *******************************************************************/
auto InputHandler::backspace(int choped) -> void
{
    if (!m_editableWidget) {
        return;
    }

    if (QLineEdit *lineEdit = qobject_cast<QLineEdit *>(m_editableWidget)) {
        QString text = lineEdit->text();
        int cursorPos = lineEdit->cursorPosition();
        if (cursorPos > 0) {
            int deleteLength = qMin(choped, cursorPos);
            text.remove(cursorPos - deleteLength, deleteLength);
            lineEdit->setText(text);
            lineEdit->setCursorPosition(cursorPos - deleteLength);
        }
    } else if (QComboBox *comboBox = qobject_cast<QComboBox *>(m_editableWidget)) {
        if (comboBox->isEditable()) {
            QLineEdit *lineEdit = comboBox->lineEdit();
            QString text = lineEdit->text();
            int cursorPos = lineEdit->cursorPosition();
            if (cursorPos > 0) {
                int deleteLength = qMin(choped, cursorPos);
                text.remove(cursorPos - deleteLength, deleteLength);
                lineEdit->setText(text);
                lineEdit->setCursorPosition(cursorPos - deleteLength);
            }
        }
    } else if (QTextEdit *textEdit = qobject_cast<QTextEdit *>(m_editableWidget)) {
        for (int i = 0; i < choped; ++i) {
            m_cursor.deletePreviousChar();
        }
        textEdit->setTextCursor(m_cursor);
    } else if (QPlainTextEdit *plainTextEdit = qobject_cast<QPlainTextEdit *>(m_editableWidget)) {
        for (int i = 0; i < choped; ++i) {
            m_cursor.deletePreviousChar();
        }
        plainTextEdit->setTextCursor(m_cursor);
    } else if (QTextBrowser *textBrowser = qobject_cast<QTextBrowser *>(m_editableWidget)) {
        for (int i = 0; i < choped; ++i) {
            m_cursor.deletePreviousChar();
        }
        textBrowser->setTextCursor(m_cursor);
    }
}
