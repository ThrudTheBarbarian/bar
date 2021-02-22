QT -= gui

CONFIG += c++11 console
CONFIG -= app_bundle
CONFIG += sdk_no_version_check

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += /usr/local/opt/bzip2/include

LIBS += -L/usr/local/opt/bzip2/lib -lbz2

SOURCES += \
        argsparser.cc \
        console.cc \
        create/compressor.cc \
        create/creator.cc \
        create/dirscanner.cc \
        create/filereader.cc \
        create/filesystemitem.cc \
        main.cc \
        util/scalednumber.cc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    argsparser.h \
    console.h \
    create/compressor.h \
    create/creator.h \
    create/dirscanner.h \
    create/filereader.h \
    create/filesystemitem.h \
    debug.h \
    macros.h \
    properties.h \
    structures.h \
    util/scalednumber.h
