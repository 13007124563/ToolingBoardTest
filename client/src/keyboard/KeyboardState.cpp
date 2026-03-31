/*******************************************************************
* KeyboardState.cpp
 * Copyright 2025-2029 zhilai-tech
 * description: 键盘状态机实现
 * 2025-12-05, niezixuan
*******************************************************************/
#include "KeyboardState.h"

#include <QStateMachine>

/*******************************************************************
 * description: 构造函数
 * para: parent 父对象
 * return: void
 *******************************************************************/
KeyboardState::KeyboardState(QObject *parent)
    : QObject{parent}
    , m_stateMachine{new QStateMachine(this)}
    , m_initialState{new QState()}
    , m_lowercaseState{new QState()}
    , m_uppercaseState{new QState()}
    , m_capsLockState{new QState()}
    , m_numericState{new QState()}
    , m_symbolState{new QState()}
    , m_functionKeysState{new QState()}
    , m_specialSymbolsState{new QState()}
    , m_inputCompleteState{new QState()}
    , m_hiddenState{new QState()}
{
    initializeState();
}

/*******************************************************************
 * description: 析构函数
 * para: void
 * return: void
 *******************************************************************/
KeyboardState::~KeyboardState() = default;

/*******************************************************************
 * description: 获取当前状态
 * para: void
 * return: KeyboardState::State 当前状态
 *******************************************************************/
auto KeyboardState::state() const -> KeyboardState::State
{
    const QSet<QAbstractState *> currentStates = stateMachine()->configuration();
    if (currentStates.contains(initialState().data())) {
        return State::Initial;
    }

    return State::Unknown;
}

/*******************************************************************
 * description: 初始化状态机
 * para: void
 * return: void
 *******************************************************************/
auto KeyboardState::initializeState() -> void
{
    m_stateMachine->addState(m_initialState.data());
    m_stateMachine->addState(m_lowercaseState.data());
    m_stateMachine->addState(m_uppercaseState.data());
    m_stateMachine->addState(m_capsLockState.data());
    m_stateMachine->addState(m_numericState.data());
    m_stateMachine->addState(m_symbolState.data());
    m_stateMachine->addState(m_functionKeysState.data());
    m_stateMachine->addState(m_specialSymbolsState.data());
    m_stateMachine->addState(m_inputCompleteState.data());
    m_stateMachine->addState(m_hiddenState.data());

    m_stateMachine->setInitialState(m_initialState.data());

    // 状态转换示例
    m_lowercaseState->addTransition(this, &KeyboardState::shiftPressed, m_uppercaseState.data());
    m_uppercaseState->addTransition(this, &KeyboardState::shiftReleased, m_lowercaseState.data());
    m_capsLockState->addTransition(this, &KeyboardState::capsLockPressed, m_lowercaseState.data());
    m_lowercaseState->addTransition(this, &KeyboardState::capsLockPressed, m_capsLockState.data());

    m_stateMachine->start();
}
