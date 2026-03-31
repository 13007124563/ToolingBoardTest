/*******************************************************************
* KeyboardLayout.cpp
 * Copyright 2025-2029 zhilai-tech
 * description: 键盘布局实现
 * 2025-12-05, niezixuan
*******************************************************************/
#include "KeyboardLayout.h"

#include <QDebug>
#include <QLayout>
#include <QToolButton>

#include "KeyButton.h"

/*******************************************************************
 * description: 默认构造函数
 * para: void
 * return: void
 *******************************************************************/
KeyCell::KeyCell()
    : m_row{}
    , m_column{}
    , m_rowSpan{}
    , m_columnSpan{}
{
}

/**************************************************************************
 * @brief 构造函数（指定行列）
 * @param row 行号
 * @param column 列号
 * @return void
 **************************************************************************/
KeyCell::KeyCell(int row, int column)
    : m_row{row}
    , m_column{column}
    , m_rowSpan{}
    , m_columnSpan{}
{
}

/**************************************************************************
 * @brief 构造函数（指定行列和跨度）
 * @param row 行号
 * @param column 列号
 * @param rowSpan 行跨度
 * @param columnSpan 列跨度
 * @return void
 **************************************************************************/
KeyCell::KeyCell(int row, int column, int rowSpan, int columnSpan)
    : m_row{row}
    , m_column{column}
    , m_rowSpan{rowSpan}
    , m_columnSpan{columnSpan}
{
}

/*******************************************************************
 * description: 拷贝构造函数
 * para: cell 源对象
 * return: void
 *******************************************************************/
KeyCell::KeyCell(const KeyCell &cell) = default;

/*******************************************************************
 * description: 析构函数
 * para: void
 * return: void
 *******************************************************************/
KeyCell::~KeyCell() = default;

/*******************************************************************
 * description: 构造函数
 * para: parent 父控件
 * return: void
 *******************************************************************/
KeyboardLayout::KeyboardLayout(QWidget *parent)
    : QGridLayout(parent)
    , m_keyCellMap{}
    , m_btnMap{}
{
    setContentsMargins(0, 0, 0, 0);
    constexpr int Spacing = 8;
    setSpacing(Spacing);
}

/*******************************************************************
 * description: 析构函数
 * para: void
 * return: void
 *******************************************************************/
KeyboardLayout::~KeyboardLayout() = default;

/*******************************************************************
 * description: 清空节点
 * para: void
 * return: void
 *******************************************************************/
auto KeyboardLayout::clear() -> void
{
    m_keyCellMap.clear();
    m_btnMap.clear();
}

/*******************************************************************
 * description: 获取节点
 * para: index 节点索引
 * return: KeyCell 节点对象
 *******************************************************************/
auto KeyboardLayout::node(quint16 index) -> KeyCell
{
    return m_keyCellMap.value(index);
}

/*******************************************************************
 * description: 添加节点
 * para: cell 节点对象
 * return: QToolButton* 按键指针（由 Qt 父子关系管理生命周期）
 *******************************************************************/
auto KeyboardLayout::addNode(const KeyCell &cell) -> QToolButton*
{
    auto index = cell.index();

    auto* keyBtn = new KeyButton(parentWidget());

    m_keyCellMap.insert(index, cell);
    m_btnMap.insert(index, keyBtn);

    if (cell.rowSpan() == 0 || cell.columnSpan() == 0) {
        addWidget(keyBtn, cell.row(), cell.column());
        return keyBtn;
    }

    keyBtn->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    addWidget(keyBtn, cell.row(), cell.column(), cell.rowSpan(), cell.columnSpan());

    return keyBtn;
}

/*******************************************************************
 * description: 删除节点
 * para: index 节点索引
 * return: void
 *******************************************************************/
auto KeyboardLayout::removeNode(quint16 index) -> void
{
    m_keyCellMap.remove(index);
    // 从 map 中移除指针，widget 的生命周期由 Qt 父子关系管理
    m_btnMap.remove(index);
}

/*******************************************************************
 * description: 获取按键
 * para: index 按键索引
 * return: QToolButton* 按键指针
 *******************************************************************/
auto KeyboardLayout::key(quint16 index) -> QToolButton*
{
    return m_btnMap.value(index, nullptr);
}

/**************************************************************************
 * @brief 设置按键文本
 * @param index 按键索引
 * @param c 字符
 * @return void
 **************************************************************************/
auto KeyboardLayout::setKey(quint16 index, QChar c) -> void
{
    auto* btn = m_btnMap.value(index, nullptr);
    if (btn == nullptr) {
        return;
    }

    btn->setText(c);
}

/*******************************************************************
 * description: 获取所有按键索引
 * para: void
 * return: QList<quint16> 索引列表
 *******************************************************************/
auto KeyboardLayout::indexes() const -> QList<quint16>
{
    return m_keyCellMap.keys();
}

/*******************************************************************
 * description: 获取按键
 * para: index 按键索引
 * return: QToolButton* 按键指针
 *******************************************************************/
auto KeyboardLayout::button(quint16 index) -> QToolButton*
{
    return m_btnMap.value(index, nullptr);
}
