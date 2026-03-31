/*******************************************************************
 * InputMethodEditor.cpp
 * Copyright 2025-2029 zhilai-tech Co.,ltd
 * description: 输入法编辑器实现
 * 2025-12-05, niezixuan
 *******************************************************************/
#include "InputMethodEditor.h"

#include <QDebug>
#include <QFile>
#include <QStackedWidget>
#include <QVBoxLayout>

#include "AbstractKeyboard.h"
#include "DragFilter.h"
#include "InputHandler.h"
#include "InputTitleBar.h"
#include "KeyboardState.h"
#include "KeyboardTimer.h"

namespace {
bool isBackendInputWidget(const QWidget *widget)
{
    if (widget == nullptr) {
        return false;
    }

    const QWidget *curr = widget;
    while (curr != nullptr) {
        if (QString(curr->metaObject()->className()) == "BackendWnd") {
            return true;
        }
        curr = curr->parentWidget();
    }

    return false;
}
}

/*******************************************************************
 * description: 构造函数
 * para: parent 父控件
 * return: void
 *******************************************************************/
InputMethodEditor::InputMethodEditor(QWidget *parent)
    : QWidget(parent)
    , m_state{new KeyboardState(this)}
    , m_timer{new KeyboardTimer(this)}
    , m_dragFilter{new DragFilter(this)}
    , m_inputHandler{new InputHandler(this)}
    , m_contentFrame{new QFrame(this)}
    , m_inputTitleBar{new InputTitleBar(m_contentFrame)}
    , m_stackedWidget{new QStackedWidget(m_contentFrame)}
{
    initStyle();
    initLayout();
    initConnection();
    hide();
}

/*******************************************************************
 * description: 析构函数
 * para: void
 * return: void
 *******************************************************************/
InputMethodEditor::~InputMethodEditor() = default;

/*******************************************************************
 * description: 清空输入缓存
 * para: void
 * return: void
 *******************************************************************/
auto InputMethodEditor::clear() -> void
{
    m_inputText.clear();
    Q_EMIT inputChanged({});
}

/*******************************************************************
 * description: 获取输入
 * para: void
 * return: QString 输入字符串
 *******************************************************************/
auto InputMethodEditor::input() const -> QString
{
    return m_inputText;
}

/*******************************************************************
 * description: 设置输入
 * para: input 输入字符串
 * return: void
 *******************************************************************/
auto InputMethodEditor::setInput(const QString &input) -> void
{
    if (input == m_inputText) {
        return;
    }

    m_inputText = input;
    Q_EMIT inputChanged(input);
}

/*******************************************************************
 * description: 获取当前布局索引
 * para: void
 * return: int 当前布局索引
 *******************************************************************/
auto InputMethodEditor::currentIndex() const -> int
{
    auto widget = m_stackedWidget->currentWidget();
    if (!widget) {
        return -1;
    }

    auto keyboard = reinterpret_cast<AbstractKeyboard *>(widget);
    if (!keyboard) {
        return -2;
    }

    return keyboard->type();
}

/*******************************************************************
 * description: 获取当前键盘
 * para: void
 * return: AbstractKeyboard* 当前键盘指针
 *******************************************************************/
auto InputMethodEditor::currentKeyboard() const -> AbstractKeyboard *
{
    auto widget = m_stackedWidget->currentWidget();
    if (!widget) {
        return nullptr;
    }

    auto keyboard = reinterpret_cast<AbstractKeyboard *>(widget);
    if (!keyboard) {
        return nullptr;
    }

    return keyboard;
}

/*******************************************************************
 * description: 切换布局
 * para: type 键盘类型
 * return: void
 *******************************************************************/
auto InputMethodEditor::switchLayout(int type) -> void
{
    int count = m_stackedWidget->count();
    if (count == 0) {
        qWarning() << "failed to switch to" << type << "layout, stacked widget is empty";
        return;
    }

    if (!m_layoutTypeList.contains(type)) {
        qWarning() << "failed to switch to layout" << type << ", layout not found";
        return;
    }

    auto layoutIndex = m_layoutTypeList.indexOf(type);
    if (layoutIndex == m_stackedWidget->currentIndex()) {
        qDebug() << "current layout not changed";
        return;
    }

    qDebug() << "switch to layout" << type;
    m_stackedWidget->setCurrentIndex(layoutIndex);

    Q_EMIT editFinished();
}

/*******************************************************************
 * description: 添加键盘
 * para: keyboard 键盘指针
 * return: void
 *******************************************************************/
auto InputMethodEditor::addLayout(AbstractKeyboard* keyboard) -> void
{
    m_stackedWidget->addWidget(keyboard);
    m_layoutTypeList.append(keyboard->type());

    connect(keyboard, &AbstractKeyboard::buttonClicked, this, &InputMethodEditor::onButtonClicked);
}

/*******************************************************************
 * description: 移除键盘
 * para: keyboard 键盘指针
 * return: void
 *******************************************************************/
auto InputMethodEditor::removeLayout(AbstractKeyboard* keyboard) -> void
{
    m_stackedWidget->removeWidget(keyboard);
    m_layoutTypeList.removeAll(keyboard->type());
}

/*******************************************************************
 * description: 设置当前选定的可编辑的控件
 * para: editableWidget 可编辑控件
 * return: void
 *******************************************************************/
auto InputMethodEditor::setEditableWidget(QWidget *editableWidget) -> void
{
    m_editableWidget = editableWidget;
    if (!m_editableWidget) {
        return;
    }

    Q_EMIT editFinished();
}

/*******************************************************************
 * description: 初始化样式
 * para: void
 * return: void
 *******************************************************************/
auto InputMethodEditor::initStyle() -> void
{
    setObjectName("InputMethodEditor");
    setAttribute(Qt::WA_StyledBackground);

    QFile styleFile(":/style/white_keyboard.qss");
    if (styleFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QString styleSheet = styleFile.readAll();
        setStyleSheet(styleSheet);
        styleFile.close();
    } else {
        qWarning() << "Failed to open IME style file" << styleFile.fileName();
    }

    // 设置焦点策略，键盘不应获取焦点
    setFocusPolicy(Qt::NoFocus);
}

/*******************************************************************
 * description: 初始化布局
 * para: void
 * return: void
 *******************************************************************/
auto InputMethodEditor::initLayout() -> void
{
    // 主布局
    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    mainLayout->addWidget(m_contentFrame);

    // 内容区布局
    auto contentLayout = new QVBoxLayout(m_contentFrame);
    contentLayout->setContentsMargins(8, 8, 8, 8);
    contentLayout->setSpacing(0);

    m_inputTitleBar->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
    m_stackedWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
    m_contentFrame->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    // 让 InputMethodEditor 自适应内容大小
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    contentLayout->addWidget(m_inputTitleBar, 0);
    contentLayout->addWidget(m_stackedWidget, 0);
}

/*******************************************************************
 * description: 更新键盘位置
 * para: void
 * return: void
 *******************************************************************/
auto InputMethodEditor::updatePosition() -> void
{
    if (!parentWidget()) {
        return;
    }

    // 根据内容区域调整大小
    m_contentFrame->adjustSize();
    adjustSize();

    // 计算底部居中位置
    QRect parentRect = parentWidget()->rect();
    int x = (parentRect.width() - width()) / 2;
    int y = parentRect.height() - height();

    move(x, y);
    raise();
}

/*******************************************************************
 * description: 初始化信号槽连接
 * para: void
 * return: void
 *******************************************************************/
auto InputMethodEditor::initConnection() -> void
{
    m_inputTitleBar->installEventFilter(m_dragFilter);

    // 所有子控件不响应焦点事件
    const QList<QWidget *> children = findChildren<QWidget *>();
    for (QWidget *child : children) {
        child->setFocusPolicy(Qt::NoFocus);
    }
    setFocusPolicy(Qt::NoFocus);

    connect(m_stackedWidget, &QStackedWidget::currentChanged, this, &InputMethodEditor::onLayoutChagned);
    connect(this, &InputMethodEditor::inputChanged, this, &InputMethodEditor::onInputChanged);
    connect(m_inputTitleBar, &InputTitleBar::matchSelected, this, &InputMethodEditor::onMatchSelected);
    connect(this, &InputMethodEditor::editFinished, this, &InputMethodEditor::onInputFinished);
    connect(m_inputTitleBar, &InputTitleBar::collapseClicked, this, &InputMethodEditor::onCollpased);
}

/*******************************************************************
 * description: 自动切换布局
 * para: void
 * return: void
 *******************************************************************/
auto InputMethodEditor::autoSwitchLayout() -> void
{
    auto nextIndex = m_stackedWidget->currentIndex() + 1;
    if (nextIndex > m_layoutTypeList.count() - 1) {
        nextIndex = 0;
    }

    onLayoutChagned(nextIndex);
}

/*******************************************************************
 * description: 布局变更处理
 * para: index 布局索引
 * return: void
 *******************************************************************/
auto InputMethodEditor::onLayoutChagned(int index) -> void
{
    if (index < 0 || index > m_layoutTypeList.count() - 1) {
        qWarning() << "failed to change layout, index" << index << "out of range";
        return;
    }

    switchLayout(m_layoutTypeList.at(index));
}

/*******************************************************************
 * description: 按键点击处理函数
 * para: widget 按键控件
 * return: void
 *******************************************************************/
auto InputMethodEditor::onButtonClicked(const QWidget *widget) -> void
{
    if (!widget) {
        return;
    }

    if (!m_editableWidget) {
        qWarning() << "failed process input event, got invalid editable widget";
        return;
    }

    switch (widget->property("type").toInt()) {
    case static_cast<int>(AbstractKeyboard::KeyType::UnknownKey):
    {
        break;
    }
    case static_cast<int>(AbstractKeyboard::KeyType::NormalKey):
    {
        m_inputHandler->setEditableWidget(m_editableWidget)->write(widget->property("text").toString());
        if (widget->property("text").toString().front().isLetter()) {
            setInput(input() + widget->property("text").toString());
        } else {
            setInput({});
        }
        break;
    }
    case static_cast<int>(AbstractKeyboard::KeyType::SwitchKey):
    {
        autoSwitchLayout();
        break;
    }
    case static_cast<int>(AbstractKeyboard::KeyType::ShiftKey):
    {
        auto *keyboard = currentKeyboard();
        if (keyboard == nullptr) {
            break;
        }

        keyboard->setUpper(!keyboard->upper());
        break;
    }
    case static_cast<int>(AbstractKeyboard::KeyType::SpaceKey):
    {
        m_inputHandler->setEditableWidget(m_editableWidget)->write(" ");
        setInput(input() + " ");
        break;
    }
    case static_cast<int>(AbstractKeyboard::KeyType::BackspaceKey):
    {
        if (!input().isEmpty()) {
            setInput(input().chopped(1));
        }
        m_inputHandler->setEditableWidget(m_editableWidget)->backspace();
        break;
    }
    }
}

/*******************************************************************
 * description: 输入改变处理函数
 * para: input 输入字符串
 * return: void
 *******************************************************************/
auto InputMethodEditor::onInputChanged(const QString &input) -> void
{
    // 后台页面输入框禁用候选列表。
    if (isBackendInputWidget(m_editableWidget)) {
        m_inputTitleBar->setMatched({});
        return;
    }

    auto *currKeyboard = currentKeyboard();
    if (currKeyboard == nullptr) {
        return;
    }

    if (input.isEmpty()) {
        m_inputTitleBar->setMatched({});
        return;
    }

    auto matchedWords = currKeyboard->matched(input);
    m_inputTitleBar->setMatched(matchedWords);
}

/*******************************************************************
 * description: 匹配词选择处理函数
 * para: word 选中的词
 * return: void
 *******************************************************************/
auto InputMethodEditor::onMatchSelected(const QString &word) -> void
{
    if (word.isEmpty()) {
        return;
    }

    m_inputTitleBar->setMatched({});
    m_inputHandler->setEditableWidget(m_editableWidget)->backspace(input().size());
    m_inputHandler->setEditableWidget(m_editableWidget)->write(word);

    setInput({});
    Q_EMIT editFinished();
}

/*******************************************************************
 * description: 输入结束处理函数
 * para: void
 * return: void
 *******************************************************************/
auto InputMethodEditor::onInputFinished() -> void
{
    m_inputText.clear();
    m_inputTitleBar->setMatched({});
    onCollpased();
}

/*******************************************************************
 * description: 收起事件处理函数
 * para: void
 * return: void
 *******************************************************************/
auto InputMethodEditor::onCollpased() -> void
{
    if (m_inputTitleBar->matched().isEmpty()) {
        m_inputTitleBar->updateCandidateHeight(false);
        m_stackedWidget->setVisible(true);
        updatePosition();
        return;
    }

    bool keyboardWillHide = m_stackedWidget->isVisible();

    // 键盘隐藏前记录当前宽度和高度，展开后保持一致
    if (keyboardWillHide) {
        m_contentFrame->setFixedSize(m_contentFrame->size());
        m_inputTitleBar->updateCandidateHeight(true, m_stackedWidget->height());
    } else {
        // 恢复自适应尺寸
        m_contentFrame->setMinimumSize(0, 0);
        m_contentFrame->setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
        m_inputTitleBar->updateCandidateHeight(false, 0);
    }

    m_stackedWidget->setVisible(!keyboardWillHide);
    updatePosition();
}

/*******************************************************************
 * description: 显示事件 handling
 * para: event
 * return: void
 *******************************************************************/
void InputMethodEditor::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    updatePosition();
}

/*******************************************************************
 * description: 大小改变事件 handling
 * para: event
 * return: void
 *******************************************************************/
void InputMethodEditor::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    updatePosition();
}
