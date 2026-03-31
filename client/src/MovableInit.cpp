#include "MovableInit.h"
#include <QApplication>
#include <QEvent>
#include <QMouseEvent>
#include <QWidget>

MovableInit *MovableInit::self = 0;
MovableInit::MovableInit(QObject *parent) : QObject(parent)
{
}

bool MovableInit::eventFilter(QObject *obj, QEvent *evt)
{
    QMouseEvent *event = static_cast<QMouseEvent *>(evt);
    if(!event){
        return QObject::eventFilter(obj, evt);
    }
    QWidget *w = qobject_cast<QWidget *>(obj);
    if(!w){
        return QObject::eventFilter(obj, evt);
    }
    if (!w->property("canMove").toBool()) {
        return QObject::eventFilter(obj, evt);
    }

    static QPoint mousePoint;
    static bool mousePressed = false;

    if (event->type() == QEvent::MouseButtonPress) {
        if (event->button() == Qt::LeftButton) {
            w->setCursor(Qt::SizeAllCursor);
            mousePressed = true;
            mousePoint = event->globalPos() - w->pos();
            return QObject::eventFilter(obj, evt);
        }
    } else if (event->type() == QEvent::MouseButtonRelease) {
        w->setCursor(Qt::ArrowCursor);
        mousePressed = false;
        return QObject::eventFilter(obj, evt);
    } else if (event->type() == QEvent::MouseMove) {
        if (mousePressed && (event->buttons() && Qt::LeftButton)) {
            w->setCursor(Qt::SizeAllCursor);
            w->move(event->globalPos() - mousePoint);
            return QObject::eventFilter(obj, evt);
        }else{
            w->setCursor(Qt::ArrowCursor);
        }
    }

    return QObject::eventFilter(obj, evt);
}

void MovableInit::start()
{
    qApp->installEventFilter(this);
}
