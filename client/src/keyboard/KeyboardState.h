/*******************************************************************
* KeyboardState.h
 * Copyright 2025-2029 zhilai-tech
 * description: 键盘状态管理类
 * 2025-12-05, niezixuan
*******************************************************************/
#ifndef KEYBOARD_STATE_H
#define KEYBOARD_STATE_H

#include <QObject>
#include <QScopedPointer>

class QState;

class QStateMachine;

class KeyboardState : public QObject
{
    Q_OBJECT

public:
    enum class State
    {
        Unknown,       // 未知状态
        Initial,       // 初始化状态
        LowerCase,     // 小写字母状态
        UpperCase,     // 大写字母状态
        CapsLock,      // 大写锁状态
        Numeric,       // 数字状态
        Symbol,        // 符号状态
        FunctionKeys,  // 功能键状态
        SpecialSymbol, // 特殊符号状态
        InputComplete, // 输入完成状态
        HiddenState    // 键盘隐藏状态
    };

    explicit KeyboardState(QObject *parent = nullptr);
    ~KeyboardState() override;

    [[nodiscard]] auto state() const -> State; // 获取当前状态
    [[nodiscard]] auto stateMachine() const -> const QScopedPointer<QStateMachine> &
    {
        return m_stateMachine;
    } // 获取状态机
    [[nodiscard]] auto initialState() const -> const QScopedPointer<QState> &
    {
        return m_initialState;
    } // 获取初始化状态
    [[nodiscard]] auto lowercaseState() const -> const QScopedPointer<QState> &
    {
        return m_lowercaseState;
    } // 获取小写字母状态
    [[nodiscard]] auto uppercaseState() const -> const QScopedPointer<QState> &
    {
        return m_uppercaseState;
    } // 获取大写字母状态
    [[nodiscard]] auto capsLockState() const -> const QScopedPointer<QState> &
    {
        return m_capsLockState;
    } // 获取大写锁状态
    [[nodiscard]] auto numericState() const -> const QScopedPointer<QState> &
    {
        return m_numericState;
    } // 获取数字状态
    [[nodiscard]] auto symbolState() const -> const QScopedPointer<QState> &
    {
        return m_symbolState;
    } // 获取符号状态
    [[nodiscard]] auto functionKeysState() const -> const QScopedPointer<QState> &
    {
        return m_functionKeysState;
    } // 功能键状态
    [[nodiscard]] auto specialSymbolsState() const -> const QScopedPointer<QState> &
    {
        return m_specialSymbolsState;
    } // 特殊符号状态
    [[nodiscard]] auto inputCompleteState() const -> const QScopedPointer<QState> &
    {
        return m_inputCompleteState;
    } // 输入完成状态
    [[nodiscard]] auto hiddenState() const -> const QScopedPointer<QState> &
    {
        return m_hiddenState;
    } // 键盘隐藏状态

Q_SIGNALS:
    void shiftPressed();    // Shift按下信号
    void shiftReleased();   // shift释放信号
    void capsLockPressed(); // 大写锁按下信号
    void capsLockRelease(); // 大写锁释放信号

private:
    auto initializeState() -> void; // 初始化状态机

    QScopedPointer<QStateMachine> m_stateMachine; // 状态机对象
    QScopedPointer<QState> m_initialState;        // 初始化状态
    QScopedPointer<QState> m_lowercaseState;      // 小写字母状态
    QScopedPointer<QState> m_uppercaseState;      // 大写字母状态
    QScopedPointer<QState> m_capsLockState;       // 大写锁状态
    QScopedPointer<QState> m_numericState;        // 数字状态
    QScopedPointer<QState> m_symbolState;         // 符号状态
    QScopedPointer<QState> m_functionKeysState;   // 功能键状态
    QScopedPointer<QState> m_specialSymbolsState; // 特殊符号状态
    QScopedPointer<QState> m_inputCompleteState;  // 输入完成状态
    QScopedPointer<QState> m_hiddenState;         // 键盘隐藏状态
};

#endif // KEYBOARD_STATE_H
