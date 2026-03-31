QT += gui

greaterThan(QT_MAJOR_VERSION, 5): QT += statemachine

INCLUDEPATH += \
    $$PWD/Base \
    $$PWD/Custom \
    $$PWD

HEADERS += \
    $$PWD/Base/AbstractKeyboard.h \
    $$PWD/Base/keyButton.h \
    $$PWD/Base/keyNode.h \
    $$PWD/Base/KeyboardLayout.h \
    $$PWD/Custom/ChineseKeyboard.h \
    $$PWD/Custom/EnglishKeyboard.h \
    $$PWD/Custom/NumberKeyboard.h \
    $$PWD/Custom/SymbolKeyboard.h \
    $$PWD/DragFilter.h \
    $$PWD/InputEventFilter.h \
    $$PWD/InputHandler.h \
    $$PWD/InputMethodManager.h \
    $$PWD/InputMethodEditor.h \
    $$PWD/InputTitleBar.h \
    $$PWD/KeyboardState.h \
    $$PWD/KeyboardTimer.h

SOURCES += \
    $$PWD/Base/AbstractKeyboard.cpp \
    $$PWD/Base/keyButton.cpp \
    $$PWD/Base/keyNode.cpp \
    $$PWD/Base/KeyboardLayout.cpp \
    $$PWD/Custom/ChineseKeyboard.cpp \
    $$PWD/Custom/EnglishKeyboard.cpp \
    $$PWD/Custom/NumberKeyboard.cpp \
    $$PWD/Custom/SymbolKeyboard.cpp \
    $$PWD/DragFilter.cpp \
    $$PWD/InputEventFilter.cpp \
    $$PWD/InputHandler.cpp \
    $$PWD/InputMethodManager.cpp \
    $$PWD/InputMethodEditor.cpp \
    $$PWD/InputTitleBar.cpp \
    $$PWD/KeyboardState.cpp \
    $$PWD/KeyboardTimer.cpp

RESOURCES += \
    $$PWD/Resources/Keyboard.qrc
