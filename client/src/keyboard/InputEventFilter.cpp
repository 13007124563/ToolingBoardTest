/*******************************************************************
* InputEventFilter.cpp
 * Copyright 2025-2029 zhilai-tech
 * description: 输入事件过滤器实现
 * 2025-12-05, niezixuan
*******************************************************************/
#include "InputEventFilter.h"

#include <QComboBox>
#include <QDebug>
#include <QEvent>
#include <QFocusEvent>
#include <QLineEdit>
#include <QWidget>

#include "InputMethodEditor.h"

/**************************************************************************
 * @brief 构造函数
 * @param ime 输入法编辑器指针
 * @param parent 父对象
 * @return void
 **************************************************************************/
InputEventFilter::InputEventFilter(InputMethodEditor* ime, QObject *parent)
    : QObject{parent}
    , m_ime(ime)
    , m_editableNames{}
    , m_ignoreNames{}
{
}

/*******************************************************************
 * description: 析构函数
 * para: void
 * return: void
 *******************************************************************/
InputEventFilter::~InputEventFilter() = default;

/*******************************************************************
 * description: 设置可编辑控件的名称列表
 * para: names 控件名称列表
 * return: void
 *******************************************************************/
void InputEventFilter::setEditableNames(const QStringList &names)
{
    if (names == m_editableNames) {
        return;
    }

    m_editableNames = names;
}

/*******************************************************************
 * description: 设置被忽略的控件名列表
 * para: names 控件名称列表
 * return: void
 *******************************************************************/
void InputEventFilter::setIgnoreNames(const QStringList &names)
{
    if (names == m_ignoreNames) {
        return;
    }

    m_ignoreNames = names;
}

/**************************************************************************
 * @brief 事件过滤处理
 * @param obj 被监视的对象
 * @param event 事件
 * @return bool 是否处理事件
 **************************************************************************/
auto InputEventFilter::eventFilter(QObject *obj, QEvent *event) -> bool
{
    onFocusOut(obj, event);

    // 当点击在键盘内部时，阻止事件冒泡到父控件
    if (onEdittingTrigger(obj, event)) {
        return true;
    }

    return QObject::eventFilter(obj, event);
}

/**************************************************************************
 * @brief 编辑触发函数
 * @param obj 对象
 * @param event 事件
 * @return bool 是否消费事件
 **************************************************************************/
auto InputEventFilter::onEdittingTrigger(QObject *obj, QEvent *event) -> bool
{
    if (event->type() != QEvent::MouseButtonPress) {
        return false;
    }

    auto widget = qobject_cast<QWidget *>(obj);
    if (not widget) {
        return false;
    }

    // 点击在键盘内部
    if (isIgnored(widget)) {
        // 其他键盘子控件不消费事件，让按钮正常工作
        if (QString(widget->metaObject()->className()) == "InputMethodEditor") {
            return true;
        }
        return false;
    }

    // 点击可编辑控件，显示键盘
    if (isEditableWidget(widget)) {
        Q_EMIT edittingTriggered(widget);
        return false;
    }

    // 点击非键盘、非可编辑控件，隐藏键盘
    Q_EMIT edittingTriggered({});
    return false;
}

/**************************************************************************
 * @brief 失焦处理函数
 * @param obj 对象
 * @param event 事件
 * @return bool 是否处理事件
 **************************************************************************/
auto InputEventFilter::onFocusOut(QObject *obj, QEvent *event) -> bool
{
    // 非失焦事件不处理
    if (event->type() != QEvent::FocusOut) {
        return QObject::eventFilter(obj, event);
    }

    auto focusEvent = static_cast<QFocusEvent *>(event);
    if (obj->metaObject()->className() != m_ime->metaObject()->className()) {
        // qDebug() << "==== focus out:" << obj->metaObject()->className();
        return QObject::eventFilter(obj, event);
    }

    switch (focusEvent->reason()) {
    case Qt::BacktabFocusReason:
    case Qt::ActiveWindowFocusReason:
    case Qt::PopupFocusReason:
    case Qt::ShortcutFocusReason:
    case Qt::MenuBarFocusReason:
        // Q_EMIT lostFocus();
        break;
    default:
        break;
    }

    return QObject::eventFilter(obj, event);
}

/*******************************************************************
 * description: 判断控件是否可编辑
 * para: widget 控件指针
 * return: bool 是否可编辑
 *******************************************************************/
auto InputEventFilter::isEditableWidget(QWidget *widget) -> bool
{
    if (not widget) {
        return false;
    }

    bool isEditable{};
    for (const auto &className : qAsConst(m_editableNames)) {
        if (not widget->inherits(className.toStdString().c_str())) {
            continue;
        }

        if (className == "QComboBox") {
            auto comboBox = qobject_cast<QComboBox *>(widget);
            if (comboBox) {
                isEditable = comboBox->isEditable();
                break;
            }
        } else if (className == "QLineEdit") {
            auto lineEdit = qobject_cast<QLineEdit *>(widget);
            if (lineEdit) {
                isEditable = not lineEdit->isReadOnly();
                break;
            }
        }

        isEditable = true;
        break;
    }

    return isEditable;
}

/*******************************************************************
 * description: 判断控件是否为键盘（应被忽略）
 * para: widget 控件指针
 * return: bool 是否应被忽略
 *******************************************************************/
auto InputEventFilter::isIgnored(QWidget *widget) -> bool
{
    if (not widget) {
        return true;
    }

    // 遍历父控件链，查找是否有被忽略的控件
    while (widget) {
        if (m_ignoreNames.contains(widget->metaObject()->className())) {
            qDebug() << "[DEBUG] InputEventFilter: ignored widget hit:" << widget->metaObject()->className();
            return true;
        }
        widget = widget->parentWidget();
    }

    return false;
}
