#ifndef MovableInit_H
#define MovableInit_H

#include <QObject>
#include <QMutex>

/**
 * @brief The MovableInit class 是否可移动事件过滤器
 * @example
 *      QWidget widget;
 *      widget.setProperty("canMove",true);//if true can move
 */
class MovableInit : public QObject
{
    Q_OBJECT
public:
    explicit MovableInit(QObject *parent = 0);
    static MovableInit *Instance() {
        static QMutex mutex;
        if (!self) {
            QMutexLocker locker(&mutex);
            if (!self) {
                self = new MovableInit;
            }
        }
        return self;
    }

    void start();

protected:
    bool eventFilter(QObject *obj, QEvent *evt);

private:
    static MovableInit *self;

signals:

public slots:
};

#endif // MovableInit_H
