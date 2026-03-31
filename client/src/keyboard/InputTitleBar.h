/*******************************************************************
* InputTitleBar.h
 * Copyright 2025-2029 zhilai-tech
 * description: 输入法标题栏类
 * 2025-12-05, niezixuan
*******************************************************************/
#ifndef INPUTTITLEBAR_H
#define INPUTTITLEBAR_H

#include <QLineEdit>
#include <QListWidget>
#include <QToolButton>
#include <QWidget>

class InputTitleBar : public QWidget
{
    Q_OBJECT

public:
    explicit InputTitleBar(QWidget *parent = nullptr);
    ~InputTitleBar();

    auto clear() -> void;                                       // 清空候选词
    [[nodiscard]] auto matched() const -> QStringList;          // 获取候选词
    auto setMatched(const QStringList &candidateWords) -> void; // 设置候选词
    auto updateCandidateHeight(bool expanded, int keyboardHeight = 0) -> void; // 更新候选词区域高度

Q_SIGNALS:
    void collapseClicked();                  // 收起按钮点击事件
    void startDragging(const QPoint &pos);   // 开始拖动信号
    void stopDragging();                     // 停止拖动信号
    void dragging(const QPoint &pos);        // 拖动信号
    void matchSelected(const QString &word); // 匹配词选择信号

protected:
    auto eventFilter(QObject *watched, QEvent *event) -> bool override;

private:
    auto initLayout() -> void;          // 初始化布局
    auto initBufferArea() -> void;      // 初始化输入缓存区
    auto initCandidateWidget() -> void; // 初始化候选词控件
    auto initCollapseBtn() -> void;     // 初始化收起控件
    auto initConnection() -> void;      // 初始化连接

    bool m_dragging{};                        // 是否正在拖动
    QPoint m_startPos{};                      // 开始拖动时的鼠标位置
    QToolButton *m_collapseBtn = nullptr;     // 收起按钮
    QWidget *m_bufferWidget = nullptr;        // 缓存区
    QListWidget *m_candidateWidget = nullptr; // 候选词列表
};

#endif // INPUTTITLEBAR_H
