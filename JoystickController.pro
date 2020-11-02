QT       += core gui
QT       += serialport
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# Adds sdl2 to our project
LIBS += -L$$PWD/SDL2-2.0.6/lib/x64 -lSDL2
INCLUDEPATH += $$PWD/SDL2-2.0.6/include

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    controller.cpp \
    joystick.cpp \
    main.cpp \
    serial.cpp

HEADERS += \
    controller.h \
    joystick.h \
    serial.h

FORMS += \
    controller.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
