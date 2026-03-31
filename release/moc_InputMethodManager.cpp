/****************************************************************************
** Meta object code from reading C++ file 'InputMethodManager.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.10.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../client/src/keyboard/InputMethodManager.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'InputMethodManager.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.10.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_InputMethodManager_t {
    QByteArrayData data[7];
    char stringdata0[84];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_InputMethodManager_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_InputMethodManager_t qt_meta_stringdata_InputMethodManager = {
    {
QT_MOC_LITERAL(0, 0, 18), // "InputMethodManager"
QT_MOC_LITERAL(1, 19, 12), // "showKeyboard"
QT_MOC_LITERAL(2, 32, 0), // ""
QT_MOC_LITERAL(3, 33, 12), // "hideKeyboard"
QT_MOC_LITERAL(4, 46, 21), // "editableWidgetChanged"
QT_MOC_LITERAL(5, 68, 8), // "QWidget*"
QT_MOC_LITERAL(6, 77, 6) // "widget"

    },
    "InputMethodManager\0showKeyboard\0\0"
    "hideKeyboard\0editableWidgetChanged\0"
    "QWidget*\0widget"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_InputMethodManager[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   29,    2, 0x06 /* Public */,
       3,    0,   30,    2, 0x06 /* Public */,
       4,    1,   31,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 5,    6,

       0        // eod
};

void InputMethodManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        InputMethodManager *_t = static_cast<InputMethodManager *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->showKeyboard(); break;
        case 1: _t->hideKeyboard(); break;
        case 2: _t->editableWidgetChanged((*reinterpret_cast< QWidget*(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            typedef void (InputMethodManager::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&InputMethodManager::showKeyboard)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (InputMethodManager::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&InputMethodManager::hideKeyboard)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (InputMethodManager::*_t)(QWidget * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&InputMethodManager::editableWidgetChanged)) {
                *result = 2;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject InputMethodManager::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_InputMethodManager.data,
      qt_meta_data_InputMethodManager,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *InputMethodManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *InputMethodManager::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_InputMethodManager.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int InputMethodManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void InputMethodManager::showKeyboard()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void InputMethodManager::hideKeyboard()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void InputMethodManager::editableWidgetChanged(QWidget * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
