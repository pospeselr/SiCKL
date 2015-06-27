QT -= core gui

TEMPLATE = lib
CONFIG += staticlib
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

debug:TARGET = SiCKLD
release:TARGET = SiCKL

# compiler flags

win32 {
    DEFINES += _CRT_SECURE_NO_WARNINGS
}
macx {
    QMAKE_MAC_SDK = macosx10.10
}
unix {
    QMAKE_CXXFLAGS += -std=c++11
    QMAKE_CXXFLAGS += -Wno-invalid-offsetof
    debug {
        QMAKE_CXXFLAGS += -O0
    }
    release {
        # any bottleneck in a real program is going to be the device computation, so opt
        # to minimize the size of the code required for kernel generation
        QMAKE_CXXFLAGS += -Os
    }
}

# precompiled header

# include paths

INCLUDEPATH += include

win32 {
    INCLUDEPATH += \
        "C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v6.5\include"
} else:macx {
    INCLUDEPATH +=\
        /usr/local/include
}

# includes

HEADERS += \
    include/AST.h \
    include/Common.h \
    include/Compiler.h \
    include/Declares.h \
    include/Defines.h \
    include/DeviceBuffer.h \
    include/Enums.h \
    include/HostBuffer.h \
    include/Program.h \
    include/Runtime.h \
    include/SiCKL.h \
    include/Source.h \
    include/KernelBuffers.h \
    include/KernelFunctions.h \
    include/KernelTypes.h \
    include/KernelConstructs.h

# sources

SOURCES += \
    source/AST.cpp \
    source/Compiler.cpp \
    source/Program.cpp \
    source/Runtime.cpp \
    source/Source.cpp \
    source/KernelTypes.cpp \
    source/KernelFunctions.cpp \
    source/KernelConstructs.cpp


# unix {
#   target.path = /usr/lib
#    INSTALLS += target
# }

# output binary directories

debug:DESTDIR = $$PWD/../../bin/debug
release:DESTDIR = $$PWD/../../bin/release

OBJECTS_DIR = $$DESTDIR/.obj
MOC_DIR = $$DESTDIR/.moc
RCC_DIR = $$DESTDIR/.qrc
UI_DIR = $$DESTDIR/.ui


