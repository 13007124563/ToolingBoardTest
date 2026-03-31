/*******************************************************************
* InputEventFilter.h
 * Copyright 2025-2029 zhilai-tech
 * description: 输入事件捕获类
 * 2025-12-05, niezixuan
*******************************************************************/
#ifndef INPUT_EVENT_FILTER_H
#define INPUT_EVENT_FILTER_H

#include <QObject>

class InputMethodEditor;

class InputEventFilter : public QObject
{
    Q_OBJECT

public:
    explicit InputEventFilter(InputMethodEditor* ime, QObject *parent = nullptr);
    ~InputEventFilter() override;

    InputEventFilter(const InputEventFilter &) = delete;

    auto operator=(const InputEventFilter &) -> InputEventFilter & = delete;

    InputEventFilter(InputEventFilter &&) = delete;

    auto operator=(InputEventFilter &&) -> InputEventFilter & = delete;

    [[nodiscard]] auto editableNames() const -> QStringList
    {
        return m_editableNames;
    }
    void setEditableNames(const QStringList &names); // 设置可编辑控件的名称列表

    [[nodiscard]] auto ignoreNames() const -> QStringList
    {
        return m_ignoreNames;
    };
    void setIgnoreNames(const QStringList &names); // 设置被忽略的控件名列表

Q_SIGNALS:
    void edittingTriggered(QWidget *editableWidget); // 编辑触发信号
    void lostFocus();                                // 失去焦点信号

protected:
    auto eventFilter(QObject *obj, QEvent *event) -> bool override; // 事件过滤处理

private:
    auto onEdittingTrigger(QObject *obj, QEvent *event) -> bool; // 编辑触发函数
    auto onFocusOut(QObject *obj, QEvent *event) -> bool;        // 失焦处理函数
    auto isEditableWidget(QWidget *widget) -> bool;              // 判断控件是否可编辑
    auto isIgnored(QWidget *widget) -> bool;                     // 判断控件是否为键盘（应被忽略）

    InputMethodEditor* m_ime{nullptr}; // IME对象
    QStringList m_editableNames;             // 可编辑的控件名列表
    QStringList m_ignoreNames;               // 被忽略的控件名列表
};

#endif // INPUT_EVENT_FILTER_H
