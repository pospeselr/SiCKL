QT -= core gui

TEMPLATE = app

CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

# fix config to ONLY contain our build type
CONFIG(debug, debug|release) {
    CONFIG -= release
    CONFIG += debug
} else {
    CONFIG -= debug
    CONFIG += release
}

# binary name

TARGET = Mandelbrot

# compiler flags
win32 {

}
macx {

}
unix {
    QMAKE_CXXFLAGS += -std=c++11
}

# includes

INCLUDEPATH += ../SiCKL/include
INCLUDEPATH += ../../extern/EasyBMP_1.06

win32 {
    INCLUDEPATH += "C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v6.5\include"
}
# sources

SOURCES += \
    Main.cpp \
    ../../extern/EasyBMP_1.06/EasyBMP.cpp

# linking

debug:LIBS += -L$$PWD/../../bin/debug -l SiCKLD
release:LIBS += -L$$PWD/../../bin/release -l SiCKL

win32 {
    LIBS += -L"C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v6.5\lib\x64" -lopencl
} 
macx {
    LIBS += -framework OpenCL
}
unix {

    LIBS += -lOpenCL}

# output directories

release:DESTDIR = $$PWD/../../bin/release
debug:DESTDIR = $$PWD/../../bin/debug

OBJECTS_DIR = $$DESTDIR/.obj
MOC_DIR = $$DESTDIR/.moc
RCC_DIR = $$DESTDIR/.qrc
UI_DIR = $$DESTDIR/.ui
