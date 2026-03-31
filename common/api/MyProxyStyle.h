#ifndef MYPROXYSTYLE_H
#define MYPROXYSTYLE_H

#include <QProxyStyle>
#include <QStyleOption>
#include <QPainter>
#include <QWidget>

//去除窗口部件的系统焦点边框;
class MyProxyStyle : public QProxyStyle
{
public:
    virtual void drawPrimitive(PrimitiveElement element, const QStyleOption * option,
        QPainter * painter, const QWidget * widget = 0) const
    {
        if (PE_FrameFocusRect == element)
        {
            //这里不做任何操作，Qt默认是绘制矩形虚线框
        }
        else
        {
            QProxyStyle::drawPrimitive(element, option,painter, widget);
        }
    }
};

#endif // MYPROXYSTYLE_H
