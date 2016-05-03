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

TARGET = spark

# compiler flags
win32 {

}
macx {

}
unix {
    QMAKE_CXXFLAGS += -std=c++11
}

# includes

INCLUDEPATH += \
    ./include \
    ./source

win32 {
    INCLUDEPATH += "C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v6.5\include"
}
# sources

PRECOMPILED_HEADER = source/precomp.h

HEADERS += \
    include/internal/common.h \
    include/internal/enums.h \
    include/internal/types.h \
    include/spark.h \
    source/precomp.h

SOURCES += \
    main.cpp \
    source/node.cpp \



# output directories

release:DESTDIR = $$PWD/../../bin/release
debug:DESTDIR = $$PWD/../../bin/debug

OBJECTS_DIR = $$DESTDIR/.obj
MOC_DIR = $$DESTDIR/.moc
RCC_DIR = $$DESTDIR/.qrc
UI_DIR = $$DESTDIR/.ui

