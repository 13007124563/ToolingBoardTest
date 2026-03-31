/*******************************************************************
* KeyNode.h
 * Copyright 2025-2029 zhilai-tech
 * description: 键盘节点类
 * 2025-12-05, niezixuan
*******************************************************************/
#ifndef KEY_NODE_H
#define KEY_NODE_H

#include <QObject>

class KeyNode : public QObject
{
    Q_OBJECT
public:
    explicit KeyNode(QObject *parent = nullptr)
        : QObject{parent}
    {
    }
    ~KeyNode() override = default;
};

#endif // KEY_NODE_H
