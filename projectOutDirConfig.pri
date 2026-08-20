
win32 {
    CONFIG(debug, debug|release){
        DESTDIR = $$PWD/Win32/Debug
        OBJECTS_DIR = $$PWD/Win32/temp/Debug/obj
    }else{
        DESTDIR = $$PWD/Win32/Release
        OBJECTS_DIR = $$PWD/Win32/temp/Release/obj
    }
}

# Unix/Linux：uic 生成的 ui_*.h 必须落在构建目录，并由 INCLUDEPATH 找到
# 避免 Makefile 仍依赖已删除的源码根目录 ../../ui_*.h
unix {
    UI_DIR = $$OUT_PWD
    MOC_DIR = $$OUT_PWD
    RCC_DIR = $$OUT_PWD
    INCLUDEPATH += $$OUT_PWD
}
