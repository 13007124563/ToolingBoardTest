# modules path
MODULES_PATH = $$PWD/modules

# INCLUDEPATH
INCLUDEPATH += $$MODULES_PATH

# Windows平台：使用串口通信（asio库）
win32 {
    # ASIO defines
    DEFINES += \
        ASIO_STANDALONE \
        ASIO_HAS_SERIAL_PORT \
        ASIO_HAS_STD_THREAD
    
    LIBS += -lws2_32
    
    HEADERS += \
        $$MODULES_PATH/SerialCommand/ISerialCommand.h \
        $$MODULES_PATH/SerialCommand/CSerialCommandControllor.h
    
    SOURCES += \
        $$MODULES_PATH/SerialCommand/CSerialCommandControllor.cpp
}

# Linux平台：使用shell脚本方式，不需要串口模块
unix {
    # 链接pthread库（多线程支持）
    LIBS += -lpthread
    
    # 串口模块已禁用，Linux上通过QProcess调用shell脚本
    # HEADERS += $$MODULES_PATH/SerialCommand/...
    # SOURCES += $$MODULES_PATH/SerialCommand/...
    
    # 交叉编译时可能需要的额外配置
    message("Building for Unix/Linux platform - Serial port disabled, using QProcess")
}
