/*******************************************************************
* DragFilter.cpp
 * Copyright 2025-2029 zhilai-tech
 * description: 拖拽过滤器实现
 * 2025-12-05, niezixuan
*******************************************************************/
#include "DragFilter.h"

#include <QDebug>
#include <QEvent>
#include <QMouseEvent>
#include <QWidget>

/*******************************************************************
 * description: 构造函数
 * para: parent 父控件
 * return: void
 *******************************************************************/
DragFilter::DragFilter(QWidget *parent)
    : QObject{parent}
    , m_parent{parent}
    , m_dragPosition{}
{
}

/*******************************************************************
 * description: 析构函数
 * para: void
 * return: void
 *******************************************************************/
DragFilter::~DragFilter() = default;

/**************************************************************************
 * @brief 事件过滤处理
 * @param watched 被监视的对象
 * @param event 事件
 * @return bool 是否处理事件
 **************************************************************************/
auto DragFilter::eventFilter(QObject *watched, QEvent *event) -> bool
{
    switch (event->type()) {
    case QEvent::MouseButtonPress:
    {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
        if (mouseEvent->button() == Qt::LeftButton) {
            m_dragPosition = mouseEvent->globalPos() - m_parent->frameGeometry().topLeft();
            m_dragging = true;
            return true;
        }

        break;
    }
    case QEvent::MouseMove:
    {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
        if (m_dragging && (mouseEvent->buttons() & Qt::LeftButton)) {
            m_parent->move(mouseEvent->globalPos() - m_dragPosition);
            return true;
        }

        break;
    }
    case QEvent::MouseButtonRelease:
    {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
        if (mouseEvent->button() == Qt::LeftButton) {
            m_dragging = false;
            return true;
        }

        break;
    }
    default:
        break;
    }

    return QObject::eventFilter(watched, event);
}
