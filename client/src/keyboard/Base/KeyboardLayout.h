/*******************************************************************
* KeyboardLayout.h
 * Copyright 2025-2029 zhilai-tech
 * description: 键盘布局管理类
 * 2025-12-05, niezixuan
*******************************************************************/
#ifndef KEYBOARD_LAYOUT_H
#define KEYBOARD_LAYOUT_H

#include <QGridLayout>
#include <QMap>

class QToolButton;

class KeyCell
{
public:
    KeyCell();
    KeyCell(int row, int column);
    KeyCell(int row, int column, int rowSpan, int columnSpan);
    KeyCell(const KeyCell &cell);
    ~KeyCell();

    auto operator=(const KeyCell &) -> KeyCell & = default;

    KeyCell(KeyCell &&) = default;

    auto operator=(KeyCell &&) -> KeyCell & = default;

    [[nodiscard]] inline auto row() const -> int
    {
        return m_row;
    }
    inline void setRow(int row)
    {
        m_row = row;
    }

    [[nodiscard]] inline auto column() const -> int
    {
        return m_column;
    }
    inline void setColumn(int column)
    {
        m_column = column;
    }

    [[nodiscard]] inline auto rowSpan() const -> int
    {
        return m_rowSpan;
    }
    inline void setRowSpan(int span)
    {
        m_rowSpan = span;
    }

    [[nodiscard]] inline auto columnSpan() const -> int
    {
        return m_columnSpan;
    }
    inline void setColumnSpan(int span)
    {
        m_columnSpan = span;
    }

    [[nodiscard]] auto index() const -> quint16
    {
        constexpr int BitsPerByte = 8;
        return m_row << BitsPerByte | m_column;
    } // 根据行列号获取索引

    inline auto operator<(const KeyCell &other) const -> bool
    {
        return other.m_row < m_row or other.m_column < m_column;
    }
    inline auto operator==(const KeyCell &other) const -> bool
    {
        return other.m_row == m_row and other.m_column == m_column;
    }

private:
    int m_row;        // 行号
    int m_column;     // 列号
    int m_rowSpan;    // 所占行数
    int m_columnSpan; // 所占列数
};

class KeyboardLayout : public QGridLayout
{
    Q_OBJECT

public:
    explicit KeyboardLayout(QWidget *parent);
    ~KeyboardLayout() override;

    auto clear() -> void;                             // 清空节点
    auto node(quint16 index) -> KeyCell;              // 获取节点
    auto addNode(const KeyCell &cell) -> QToolButton*; // 添加节点
    auto removeNode(quint16 index) -> void;           // 删除节点

    auto key(quint16 index) -> QToolButton*;          // 获取按键
    auto setKey(quint16 index, QChar c) -> void;      // 设置按键字符
    [[nodiscard]] auto indexes() const -> QList<quint16>; // 获取所有按键索引
    auto button(quint16 index) -> QToolButton*;       // 获取按键

private:
    QMap<quint16, KeyCell> m_keyCellMap;   // 按键单位字典
    QMap<quint16, QToolButton*> m_btnMap;  // 按键字典（由 Qt 父子关系管理生命周期）
};

#endif // KEYBOARD_LAYOUT_H
