#-------------------------------------------------
#
# Project created by QtCreator 2018-04-16T09:21:57
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = app-update
TEMPLATE = app

RC_FILE += qrc/appsource.rc

HEADERS += \
    app/appupdate.h

SOURCES += \
    app/appupdate.cpp \
    app/main.cpp

RESOURCES += \
    qrc/appsource.qrc

