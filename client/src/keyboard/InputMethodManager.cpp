/*******************************************************************
* InputMethodManager.cpp
 * Copyright 2025-2029 zhilai-tech
 * description: 输入法管理器实现
 * 2025-12-05, niezixuan
*******************************************************************/
#include "InputMethodManager.h"

#include <QApplication>
#include <QDebug>
#include <QScreen>

#include "EnglishKeyboard.h"
#include "InputEventFilter.h"
#include "InputHandler.h"
#include "InputMethodEditor.h"
#include "SymbolKeyboard.h"

InputMethodManager *InputMethodManager::m_manager;
QMutex InputMethodManager::g_mutex;

/*******************************************************************
 * description: 构造函数
 * para: parent 父对象
 * return: void
 *******************************************************************/
InputMethodManager::InputMethodManager(QObject *parent)
    : QObject{parent}
    , m_ime(new InputMethodEditor(qobject_cast<QWidget *>(parent)))
    , m_inputEventFilter(new InputEventFilter(m_ime, this))
    , m_inputHandler(new InputHandler(this))
    , m_keyboardMap{}
{
    initKeyboard();
    initConnection();
    initEventFilter();
}

/*******************************************************************
 * description: 析构函数
 * para: void
 * return: void
 *******************************************************************/
InputMethodManager::~InputMethodManager() = default;

/*******************************************************************
 * description: 获取管理实例（单例模式）
 * para: parent 父控件
 * return: InputMethodManager* 单例实例
 *******************************************************************/
auto InputMethodManager::instance(QWidget *parent) -> InputMethodManager *
{
    if (m_manager == nullptr) {
        QMutexLocker locker(&g_mutex);
        if (m_manager == nullptr) {
            m_manager = new InputMethodManager(parent);
        }
    }

    return m_manager;
}

/*******************************************************************
 * description: 析构管理实例
 * para: void
 * return: void
 *******************************************************************/
auto InputMethodManager::destroy() -> void
{
    delete m_manager;
    m_manager = nullptr;
}

/*******************************************************************
 * description: 设置默认键盘类型
 * para: type 键盘类型
 * return: void
 *******************************************************************/
auto InputMethodManager::setDefaultType(int type) -> void
{
    m_defaultType = type;
}

/*******************************************************************
 * description: 设置当前键盘类型
 * para: type 键盘类型
 * return: void
 *******************************************************************/
auto InputMethodManager::setCurrType(int type) -> void
{
    m_currType = type;
    onKeyboardChanged();
}

/*******************************************************************
 * description: 获取所有键盘类型
 * para: void
 * return: QList<int> 键盘类型列表
 *******************************************************************/
auto InputMethodManager::keyboardTypes() const -> QList<int>
{
    return m_keyboardMap.keys();
}

/**************************************************************************
 * @brief 添加键盘
 * @param keyboard 键盘指针
 * @param isDefault 是否设为默认键盘
 * @return void
 **************************************************************************/
auto InputMethodManager::addKeyboard(AbstractKeyboard* keyboard, bool isDefault) -> void
{
    if (keyboard == nullptr) {
        qWarning() << "failed to add keyboard, got invalid object";
        return;
    }

    if (m_keyboardMap.contains(keyboard->type())) {
        qWarning() << "keyboard" << keyboard->type() << "already exists";
        return;
    }

    // 主动设为默认值或键盘字典为空时设置为默认键盘
    if (m_keyboardMap.isEmpty()) {
        setDefaultType(keyboard->type());
        setCurrType(keyboard->type());
    } else if (isDefault) {
        setDefaultType(keyboard->type());
    }

    m_keyboardMap.insert(keyboard->type(), keyboard);
    m_ime->addLayout(keyboard);
    onKeyboardChanged();

    qDebug() << "keyboard" << keyboard->type() << keyboard->name() << "is added";
}

/*******************************************************************
 * description: 删除键盘
 * para: type 键盘类型
 * return: void
 *******************************************************************/
auto InputMethodManager::removeKeyboard(int type) -> void
{
    if (not m_keyboardMap.contains(type)) {
        qDebug() << "keyboard" << type << "does not exists";
        return;
    }

    m_keyboardMap.remove(type);
    onKeyboardChanged();
}

/*******************************************************************
 * description: 初始化键盘
 * para: void
 * return: void
 *******************************************************************/
auto InputMethodManager::initKeyboard() -> void
{
    addKeyboard(new EnglishKeyboard(m_ime));
    addKeyboard(new SymbolKeyboard(m_ime));

    m_ime->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_ime->resize(654, 300);
}

/*******************************************************************
 * description: 初始化信号槽连接
 * para: void
 * return: void
 *******************************************************************/
auto InputMethodManager::initConnection() -> void
{
    connect(this, &InputMethodManager::showKeyboard, this, &InputMethodManager::onShowKeyboard);
    connect(this, &InputMethodManager::hideKeyboard, this, &InputMethodManager::onHideKeyboard);
    connect(m_inputEventFilter, &InputEventFilter::edittingTriggered, this, &InputMethodManager::onEdittingTriggered);
    connect(m_inputEventFilter, &InputEventFilter::lostFocus, this, &InputMethodManager::onHideKeyboard);
}

/*******************************************************************
 * description: 初始化输入事件过滤器
 * para: void
 * return: void
 *******************************************************************/
auto InputMethodManager::initEventFilter() -> void
{
    qDebug() << "install input event trigger";
    m_inputEventFilter->setEditableNames({"QLineEdit", "QTextEdit", "QPlainTextEdit", "QTextBrowser", "QComboBox"});
    m_inputEventFilter->setIgnoreNames({"QDateTimeEdit", "InputMethodEditor", "QListView"});

    QApplication::instance()->installEventFilter(m_inputEventFilter);
}

/*******************************************************************
 * description: 显示键盘
 * para: void
 * return: void
 *******************************************************************/
auto InputMethodManager::onShowKeyboard() -> void
{
    if (m_ime->isVisible()) {
        return;
    }

    qDebug() << "show keyboard type:" << m_ime->currentIndex();
    m_ime->show();
}

/*******************************************************************
 * description: 隐藏键盘
 * para: void
 * return: void
 *******************************************************************/
auto InputMethodManager::onHideKeyboard() -> void
{
    if (m_ime->isHidden()) {
        return;
    }

    qDebug() << "hidden keyboard type:" << m_ime->currentIndex();
    m_ime->hide();
}

/*******************************************************************
 * description: 键盘变更处理
 * para: void
 * return: void
 *******************************************************************/
auto InputMethodManager::onKeyboardChanged() -> void
{
    // 键盘字典为空，则清除所有布局
    if (m_keyboardMap.isEmpty()) {
        m_defaultType = -1;
        m_currType = -1;
        m_ime->switchLayout(m_currType);
        return;
    }

    // 默认键盘类型所对应的键盘不存在，则将当前第一个键盘设置为默认类型
    if (not m_keyboardMap.contains(m_currType)) {
        qDebug() << "set the first as the default keyboard";
        m_currType = m_keyboardMap.firstKey();
    }

    auto keyboard = m_keyboardMap.value(m_currType);
    if (not keyboard) {
        qWarning() << "failed to switch to keyboard" << m_currType << ", got invalid keyboard";
        return;
    }

    m_ime->switchLayout(m_currType);
}

/*******************************************************************
 * description: 输入触发处理
 * para: widget 可编辑控件
 * return: void
 *******************************************************************/
auto InputMethodManager::onEdittingTriggered(const QWidget *widget) -> void
{
    if (widget == m_ime->editableWidget()) {
        if (widget) {
            Q_EMIT showKeyboard();
        }
        return;
    }

    m_ime->setEditableWidget(const_cast<QWidget *>(widget));

    Q_EMIT editableWidgetChanged(const_cast<QWidget *>(widget));

    if (not widget) {
        Q_EMIT hideKeyboard();
        return;
    }

    Q_EMIT showKeyboard();
}

/*******************************************************************
 * description: 输入状态改变处理
 * para: void
 * return: void
 *******************************************************************/
auto InputMethodManager::onStatusChanged() -> void
{
}
