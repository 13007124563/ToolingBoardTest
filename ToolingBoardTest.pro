#-------------------------------------------------
#
# Project created by QtCreator 2020-07-31T14:18:12
#
#-------------------------------------------------

QT       += core gui sql network serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ToolingBoardTest
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# 通用包含路径
INCLUDEPATH +=  \
        $$PWD/client/src \
        $$PWD/common/api \
        $$PWD/common/api\Excel \
        $$PWD/common/third_party

# Windows平台：包含asio库用于串口通信
win32 {
    INCLUDEPATH += $$PWD/common/third_party/asio-1.16.1
}

# Linux平台：不需要asio库，使用QProcess调用shell脚本
unix {
    # asio库已禁用

    # 针对ARM64交叉编译（米尔编译链）的时间类型配置
    # 强制使用64位文件偏移和时间类型，避免timesize-32.h缺失问题
    # 以 QMAKE_CXXFLAGS 方式写入，确保所有源文件（含子 .pri）都能生效
    DEFINES += _FILE_OFFSET_BITS=64
    DEFINES += _TIME_BITS=64
    QMAKE_CXXFLAGS += -D_FILE_OFFSET_BITS=64 -D_TIME_BITS=64

    # 添加C++11标准支持
    QMAKE_CXXFLAGS += -std=c++11

    # ARM架构标志已移除，避免在x86_64环境编译报错
    # 如需使用米尔交叉编译，请使用 build_myir_arm64.sh 脚本
    message("Building for Unix/Linux platform")
}

include(projectOutDirConfig.pri)
include(modules.pri)
include(client/src/keyboard/KeyBoard.pri)


SOURCES += \
        main.cpp \
    client/src/AppModel.cpp \
    client/src/BackendWnd.cpp \
    client/src/BannerWnd.cpp \
    client/src/MainApp.cpp \
    client/src/MainWnd.cpp \
    client/src/MovableInit.cpp \
    client/src/MsgWnd.cpp \
    client/src/SingleApplication.cpp \
    client/src/tablehelper.cpp \
    common/api/DBConnector.cpp \
    common/api/GlobalSignal.cpp \
    common/api/JsonConfigurator.cpp \
    common/api/SerialManager.cpp \
    common/api/CanPortTester.cpp \
    common/api/Rs232PortTester.cpp \
    common/api/UsbPortTester.cpp \
    common/api/TfCardTester.cpp \
    common/api/ThCn40Tester.cpp \
    common/api/protocol/modbuscrc.cpp \
    common/api/protocol/protocolframe.cpp \
    common/api/protocol/responseparser.cpp \
    common/api/TestRecordManager.cpp


HEADERS += \
    client/src/AppModel.h \
    client/src/BackendWnd.h \
    client/src/BannerWnd.h \
    client/src/MainApp.h \
    client/src/MainWnd.h \
    client/src/MovableInit.h \
    client/src/MsgWnd.h \
    client/src/SingleApplication.h \
    client/src/tablehelper.h \
    common/api/commondefine.h \
    common/api/CommonType.h \
    common/api/CSerialCommandWrapper.h \
    common/api/DBConnector.h \
    common/api/GlobalSignal.h \
    common/api/JsonConfigurator.h \
    common/api/LogHandler.h \
    common/api/MyProxyStyle.h \
    common/api/SerialManager.h \
    common/api/CanPortTester.h \
    common/api/Rs232PortTester.h \
    common/api/UsbPortTester.h \
    common/api/TfCardTester.h \
    common/api/ThCn40Tester.h \
    common/api/protocol/modbuscrc.h \
    common/api/protocol/protocolconstants.h \
    common/api/protocol/protocolframe.h \
    common/api/protocol/responseparser.h \
    common/api/TestRecordManager.h


FORMS += \
    client/src/BackendWnd.ui \
    client/src/BannerWnd.ui \
    client/src/MainWnd.ui \
    client/src/MsgWnd.ui

RESOURCES += \
    client/res/ToolingBoardTest_res.qrc

TRANSLATIONS += ToolingBoardTest_cn.ts \
                ToolingBoardTest_en.ts

# Platform-specific libraries
unix {
    LIBS += -lpthread

    # 添加数学库支持（某些交叉编译环境需要显式链接）
    LIBS += -lm
    LIBS += -ldl
}
