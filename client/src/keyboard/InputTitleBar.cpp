/*******************************************************************
* InputTitleBar.cpp
 * Copyright 2025-2029 zhilai-tech
 * description: 输入法标题栏实现
 * 2025-12-05, niezixuan
*******************************************************************/
#include "InputTitleBar.h"

#include <QDebug>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QScroller>

/*******************************************************************
 * description: 构造函数
 * para: parent 父控件
 * return: void
 *******************************************************************/
InputTitleBar::InputTitleBar(QWidget *parent)
    : QWidget(parent)
    , m_startPos{}
    , m_collapseBtn(new QToolButton(this))
    , m_bufferWidget(new QWidget(this))
    , m_candidateWidget(new QListWidget(this))
{
    initLayout();
    initCollapseBtn();
    initConnection();
}

/*******************************************************************
 * description: 析构函数
 * para: void
 * return: void
 *******************************************************************/
InputTitleBar::~InputTitleBar() = default;

/*******************************************************************
 * description: 清空候选词
 * para: void
 * return: void
 *******************************************************************/
auto InputTitleBar::clear() -> void
{
    m_candidateWidget->clear();
}

/*******************************************************************
 * description: 获取候选词
 * para: void
 * return: QStringList 候选词列表
 *******************************************************************/
auto InputTitleBar::matched() const -> QStringList
{
    QStringList candidates;
    for (int i = 0; i < m_candidateWidget->count(); ++i) {
        candidates << m_candidateWidget->item(i)->text();
    }

    return candidates;
}

/*******************************************************************
 * description: 设置候选词
 * para: candidateWords 候选词列表
 * return: void
 *******************************************************************/
auto InputTitleBar::setMatched(const QStringList &candidateWords) -> void
{
    if (candidateWords.isEmpty()) {
        m_candidateWidget->clear();
        m_candidateWidget->setVisible(false);
        return;
    }

    m_candidateWidget->setVisible(true);
    m_candidateWidget->clear();
    for (const QString &word : candidateWords) {
        auto *item = new QListWidgetItem(word);
        item->setTextAlignment(Qt::AlignCenter);
        m_candidateWidget->addItem(item);
    }
}

/*******************************************************************
 * description: 更新候选词区域高度（自适应）
 * para: expanded 是否展开（多行显示）
 * para: keyboardHeight 键盘高度（展开时使用）
 * return: void
 *******************************************************************/
auto InputTitleBar::updateCandidateHeight(bool expanded, int keyboardHeight) -> void
{
    constexpr int SingleLineHeight = 40;

    int targetHeight = expanded ? keyboardHeight : SingleLineHeight;
    m_bufferWidget->setMinimumHeight(targetHeight);
    m_bufferWidget->setMaximumHeight(targetHeight);
}

/**************************************************************************
 * @brief 事件过滤处理
 * @param watched 被监视的对象
 * @param event 事件
 * @return bool 是否处理事件
 **************************************************************************/
auto InputTitleBar::eventFilter(QObject *watched, QEvent *event) -> bool
{
    return QObject::eventFilter(watched, event);
}

/*******************************************************************
 * description: 初始化布局
 * para: void
 * return: void
 *******************************************************************/
auto InputTitleBar::initLayout() -> void
{
    auto hBoxLayout = new QHBoxLayout(this);
    hBoxLayout->setContentsMargins(0, 0, 0, 0);
    constexpr int LayoutSpacing = 7;
    hBoxLayout->setSpacing(LayoutSpacing);
    constexpr int WidgetMargin = 6;
    setContentsMargins({WidgetMargin, 0, WidgetMargin, 0});

    setLayout(hBoxLayout);

    hBoxLayout->addWidget(m_bufferWidget);
    hBoxLayout->addWidget(m_collapseBtn);

    m_bufferWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);

    initBufferArea();
    initCandidateWidget();
}

/*******************************************************************
 * description: 初始化缓存区
 * para: void
 * return: void
 *******************************************************************/
auto InputTitleBar::initBufferArea() -> void
{
    auto hBoxLayout = new QHBoxLayout(m_bufferWidget);
    hBoxLayout->setContentsMargins(0, 0, 0, 0);

    hBoxLayout->addWidget(m_candidateWidget);
    m_bufferWidget->setLayout(hBoxLayout);

    m_candidateWidget->setVerticalScrollMode(QListWidget::ScrollPerPixel);
    auto scroller = QScroller::scroller(m_candidateWidget);
    scroller->grabGesture(m_candidateWidget, QScroller::LeftMouseButtonGesture);

    m_bufferWidget->installEventFilter(this);
}

/*******************************************************************
 * description: 初始化候选词区
 * para: void
 * return: void
 *******************************************************************/
auto InputTitleBar::initCandidateWidget() -> void
{
    m_candidateWidget->setObjectName("candidate");
    m_candidateWidget->setWrapping(true);
    m_candidateWidget->setFlow(QListView::LeftToRight);
    m_candidateWidget->setResizeMode(QListView::Adjust);
    m_candidateWidget->setSpacing(0);
    m_candidateWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_candidateWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    updateCandidateHeight(false);
}

/*******************************************************************
 * description: 初始化收起按钮
 * para: void
 * return: void
 *******************************************************************/
auto InputTitleBar::initCollapseBtn() -> void
{
    m_collapseBtn->setToolButtonStyle(Qt::ToolButtonIconOnly);
    m_collapseBtn->setObjectName("collapse");
    m_collapseBtn->setIcon(QIcon(":/icon/icon/collapse@2x.png"));
}

/*******************************************************************
 * description: 初始化信号槽连接
 * para: void
 * return: void
 *******************************************************************/
auto InputTitleBar::initConnection() -> void
{
    connect(m_candidateWidget, &QListWidget::currentTextChanged, this, &InputTitleBar::matchSelected);
    connect(m_collapseBtn, &QToolButton::clicked, this, &InputTitleBar::collapseClicked);
}
