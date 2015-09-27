#-------------------------------------------------
#
# Project created by QtCreator 2015-09-27T13:54:02
#
#-------------------------------------------------

QT       += core gui serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = frontend
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    vnadevice.cpp \
    vnacommands.cpp

HEADERS  += mainwindow.h \
    vnadevice.h \
    ../4code/inc/commands.h \
    vnacommands.h

FORMS    +=

CONFIG += c++11

RESOURCES += \
    icons.qrc

DISTFILES += \
    icons/file_send.png
