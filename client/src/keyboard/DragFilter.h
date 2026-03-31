/*******************************************************************
* DragFilter.h
 * Copyright 2025-2029 zhilai-tech
 * description: 拖动事件处理类
 * 2025-12-05, niezixuan
*******************************************************************/
#ifndef DRAG_FILTER_H
#define DRAG_FILTER_H

#include <QObject>
#include <QPoint>

class DragFilter : public QObject
{
    Q_OBJECT

public:
    explicit DragFilter(QWidget *parent = nullptr);
    ~DragFilter() override;
    DragFilter(const DragFilter &) = delete;
    auto operator=(const DragFilter &) -> DragFilter & = delete;
    DragFilter(DragFilter &&) = delete;
    auto operator=(DragFilter &&) -> DragFilter & = delete;

protected:
    auto eventFilter(QObject *watched, QEvent *event) -> bool override; // 事件过滤处理

private:
    bool m_dragging = false; // 拖动标志位
    QWidget *m_parent;       // 父控件
    QPoint m_dragPosition;   // 拖动的位置
};

#endif // DRAG_FILTER_H
