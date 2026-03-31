
win32 {
    CONFIG(debug, debug|release){
        DESTDIR = $$PWD/Win32/Debug
        OBJECTS_DIR = $$PWD/Win32/temp/Debug/obj
    }else{
        DESTDIR = $$PWD/Win32/Release
        OBJECTS_DIR = $$PWD/Win32/temp/Release/obj
    }
}

# Unix/Linux uses default Qt Creator output paths
