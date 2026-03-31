/*******************************************************************
* InputMethodManager.h
 * Copyright 2025-2029 zhilai-tech
 * description: 输入法管理类
 * 2025-12-05, niezixuan
*******************************************************************/
#ifndef INPUT_METHOD_MANAGER_H
#define INPUT_METHOD_MANAGER_H

#include <QMap>
#include <QMutex>
#include <QObject>

class QApplication;

class InputHandler;

class InputMethodEditor;

class AbstractKeyboard;

class InputEventFilter;

class InputMethodManager : public QObject
{
    Q_OBJECT

public:
    static auto instance(QWidget *parent = nullptr) -> InputMethodManager *; // 获取管理实例（单例模式）
    void destroy();                                                          // 析构管理实例

    [[nodiscard]] auto defaultType() const -> int
    {
        return m_defaultType;
    }                                      // 获取默认键盘类型
    auto setDefaultType(int type) -> void; // 设置默认键盘类型

    [[nodiscard]] auto currType() const -> int
    {
        return m_currType;
    }                                   // 获取当前键盘类型
    auto setCurrType(int type) -> void; // 设置当前键盘类型

    [[nodiscard]] auto keyboardTypes() const -> QList<int>; // 获取所有键盘类型
    auto addKeyboard(AbstractKeyboard* keyboard, bool isDefault = false) -> void; // 添加键盘
    auto removeKeyboard(int type) -> void;                                                       // 删除键盘

Q_SIGNALS:
    void showKeyboard();                         // 显示键盘信号
    void hideKeyboard();                         // 隐藏键盘信号
    void editableWidgetChanged(QWidget *widget); // 可编辑控件变更信号

private:
    explicit InputMethodManager(QObject *parent = nullptr);
    ~InputMethodManager();

public:
    InputMethodManager(const InputMethodManager &) = delete;

    auto operator=(const InputMethodManager &) -> InputMethodManager & = delete;

    InputMethodManager(InputMethodManager &&) = delete;

    auto operator=(InputMethodManager &&) -> InputMethodManager & = delete;

    auto initKeyboard() -> void;                             // 初始化键盘
    auto initConnection() -> void;                           // 初始化信号槽连接
    auto initEventFilter() -> void;                          // 初始化输入事件过滤器
    auto onShowKeyboard() -> void;                           // 显示键盘
    auto onHideKeyboard() -> void;                           // 隐藏键盘
    auto onKeyboardChanged() -> void;                        // 键盘变更处理
    auto onEdittingTriggered(const QWidget *widget) -> void; // 输入触发处理
    auto onStatusChanged() -> void;                          // 输入状态改变处理

    static InputMethodManager *m_manager;                      // 输入管理器对象
    static QMutex g_mutex;                                     // 单例互斥锁
    int m_currType{-1};                                        // 当前键盘类型
    int m_defaultType{-1};                                     // 默认键盘类型
    InputMethodEditor* m_ime{nullptr};                         // 键盘面板
    InputEventFilter *m_inputEventFilter;                      // 输入事件过滤器
    InputHandler *m_inputHandler;                              // 输入处理对象
    QMap<int, AbstractKeyboard*> m_keyboardMap; // 键盘字典
};

#endif // INPUT_METHOD_MANAGER_H
