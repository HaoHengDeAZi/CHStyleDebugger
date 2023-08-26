# 版本号 major.minor.patch.build;build通过脚本增加
MAJOR_VERSION=1
MINOR_VERSION=1
PATCH_VERSION=00023
BUILD_VERSION=2985
VERSION = $$sprintf(%1.%2.%3.%4, $$MAJOR_VERSION, $$MINOR_VERSION, $$PATCH_VERSION, $$BUILD_VERSION)

# 用于去除文件名自动生成的版本号
CONFIG += skip_target_version_ext

# Makefile配置
CONFIG += c++11
CONFIG -= create_cmake

# 调试信息
QMAKE_CXXFLAGS_RELEASE_WITH_DEBUGINFO -= -O2
QMAKE_CFLAGS_RELEASE_WITH_DEBUGINFO -= -O2
win32:QMAKE_CXXFLAGS_RELEASE_WITH_DEBUGINFO += -Od
win32:QMAKE_CFLAGS_RELEASE_WITH_DEBUGINFO += -Od
unix:!macx:QMAKE_CXXFLAGS_RELEASE_WITH_DEBUGINFO += -O0
unix:!macx:QMAKE_CFLAGS_RELEASE_WITH_DEBUGINFO += -O0
QMAKE_CXXFLAGS_RELEASE = $$QMAKE_CXXFLAGS_RELEASE_WITH_DEBUGINFO
QMAKE_CFLAGS_RELEASE = $$QMAKE_CFLAGS_RELEASE_WITH_DEBUGINFO
QMAKE_LFLAGS_RELEASE = $$QMAKE_LFLAGS_RELEASE_WITH_DEBUGINFO

# 平台信息
win32:contains(QT_ARCH, i386) {
    PLATFORM = Windows
    ARCHITECTURE = x86
}

win32:contains(QT_ARCH, x86_64) {
    PLATFORM = Windows
    ARCHITECTURE = x64
}

unix:!macx:contains(QT_ARCH, x86_64) {
    PLATFORM = Linux
    ARCHITECTURE = x64
}

unix:!macx:contains(QT_ARCH, arm64) {
    PLATFORM = Linux
    ARCHITECTURE = aarch64
}

unix:!macx:contains(QT_ARCH, aarch64) {
    PLATFORM = Linux
    ARCHITECTURE = aarch64
}

# 预处理器定义
# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# 附加包含目录
INCLUDEPATH += $$PWD/../Depends/Include
INCLUDEPATH += $$PWD/$$TARGET/Include

# 附加库目录
LIBS += -L$$sprintf($$PWD/../Depends/Bin_%1_%2, $$PLATFORM, $$ARCHITECTURE)
LIBS += -L$$sprintf($$PWD/../Depends/Lib_%1_%2, $$PLATFORM, $$ARCHITECTURE)
LIBS += -L$$sprintf($$PWD/../Output/Bin_%1_%2_Qt%3, $$PLATFORM, $$ARCHITECTURE, $$QT_VERSION)

# 输出目录
DESTDIR = $$sprintf($$PWD/Output/Bin_%1_%2_Qt%3, $$PLATFORM, $$ARCHITECTURE, $$QT_VERSION)
# win32: DLLDESTDIR = $$sprintf($$PWD/../Output/Bin_%1_%2_Qt%3, $$PLATFORM, $$ARCHITECTURE, $$QT_VERSION)

