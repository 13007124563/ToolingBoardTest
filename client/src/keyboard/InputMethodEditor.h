/*******************************************************************
 * InputMethodEditor.h
 * Copyright 2025-2029 zhilai-tech Co.,ltd
 * description: 输入法控件类
 * 2025-12-05, niezixuan
 *******************************************************************/
#ifndef INPUTMETHODEDITOR_H
#define INPUTMETHODEDITOR_H

#include <QFrame>
#include <QWidget>

class DragFilter;
class QStackedWidget;
class QVBoxLayout;
class InputTitleBar;
class KeyboardState;
class KeyboardTimer;
class InputHandler;
class AbstractKeyboard;

class InputMethodEditor : public QWidget
{
    Q_OBJECT

public:
    explicit InputMethodEditor(QWidget *parent = nullptr);
    ~InputMethodEditor() override;

    InputMethodEditor(const InputMethodEditor &) = delete;
    auto operator=(const InputMethodEditor &) -> InputMethodEditor & = delete;
    InputMethodEditor(InputMethodEditor &&) = delete;
    auto operator=(InputMethodEditor &&) -> InputMethodEditor & = delete;

    auto clear() -> void;                                                 // 清空输入缓存
    [[nodiscard]] auto input() const -> QString;                          // 获取输入
    auto setInput(const QString &inputStr) -> void;                       // 设置输入
    [[nodiscard]] auto currentIndex() const -> int;                       // 获取当前布局索引
    [[nodiscard]] auto currentKeyboard() const -> AbstractKeyboard *;     // 获取当前键盘
    auto switchLayout(int type) -> void;                                  // 切换布局
    auto addLayout(AbstractKeyboard* keyboard) -> void;    // 添加键盘
    auto removeLayout(AbstractKeyboard* keyboard) -> void; // 移除键盘
    [[nodiscard]] auto editableWidget() const -> QWidget *
    {
        return m_editableWidget;
    }                                                        // 获取当前可编辑控件
    auto setEditableWidget(QWidget *editableWidget) -> void; // 设置当前选定的可编辑的控件

protected:
    void showEvent(QShowEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

Q_SIGNALS:
    void inputChanged(const QString &input); // 输入改变信号
    void editFinished();                     // 输入结束信号

private:
    auto initStyle() -> void;      // 初始化样式
    auto initLayout() -> void;     // 初始化布局
    auto initConnection() -> void; // 初始化连接
    auto updatePosition() -> void; // 更新键盘位置（底部对齐）

    auto autoSwitchLayout() -> void; // 自动切换布局

private Q_SLOTS:
    void onLayoutChagned(int index);             // 布局变更处理
    void onButtonClicked(const QWidget *widget); // 按键点击处理函数
    void onInputChanged(const QString &input);   // 输入改变处理函数
    void onMatchSelected(const QString &word);   // 匹配此选择处理函数
    void onInputFinished();                      // 输入结束处理函数
    void onCollpased();                          // 收起事件事件处理函数

private:
    QString m_inputText{};                     // 输入文本
    KeyboardState *m_state = nullptr;          // 键盘状态
    KeyboardTimer *m_timer = nullptr;          // 键盘定时器
    DragFilter *m_dragFilter = nullptr;        // 拖动过滤器
    QWidget *m_editableWidget = nullptr;       // 可编辑的控件
    InputHandler *m_inputHandler = nullptr;    // 输入处理
    QFrame *m_contentFrame = nullptr;          // 内容区域
    InputTitleBar *m_inputTitleBar = nullptr;  // 输入标题栏
    QStackedWidget *m_stackedWidget = nullptr; // 堆栈控件
    QList<int> m_layoutTypeList{};             // 布局类型索引列表
};

#endif // INPUTMETHODEDITOR_H
