QT       += core gui multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/modules/envelope.cpp \
    src/modules/oscillator.cpp \
    src/modules/sampleplayer.cpp \
    src/modules/trigger.cpp \
    src/main.cpp \
    src/mainwindow.cpp \
    src/masterbus.cpp \
    src/qendpoint.cpp \
    src/samplersection.cpp \
    src/synthsection.cpp

HEADERS += \
    src/AudioFile.h \
    src/audioformat.h \
    src/mainwindow.h \
    src/masterbus.h \
    src/qendpoint.h \
    src/samplersection.h \
    src/synthsection.h \
    src/modules/envelope.h \
    src/modules/generator.h \
    src/modules/oscillator.h \
    src/modules/sampleplayer.h \
    src/modules/sequence.h \
    src/modules/trigger.h

FORMS += \
    src/mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
