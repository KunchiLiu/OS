#-------------------------------------------------
#
# Project created by QtCreator 2017-09-16T09:53:35
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = OS
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    define.cpp \
    dialog.cpp \
    file_dialog.cpp \
    create_dialog.cpp

HEADERS  += mainwindow.h \
    define.h \
    dialog.h \
    file_dialog.h \
    create_dialog.h

FORMS    += mainwindow.ui \
    initparameter.ui \
    dialog.ui \
    file_dialog.ui \
    create_dialog.ui

RESOURCES += \
    background.qrc
