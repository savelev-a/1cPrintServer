#-------------------------------------------------
#
# Project created by QtCreator 2017-07-27T13:02:20
#
#-------------------------------------------------

QT       += core gui widgets network printsupport sql serialport

TARGET = 1cPrintServer
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS


SOURCES += main.cpp\
           mainwindow.cpp \
    webserver.cpp \
    httprequest.cpp \
    httpclient.cpp \
    application.cpp \
    printservice.cpp \
    printerbzb2.cpp \
    cheque.cpp \
    chequeline.cpp \
    databaseservice.cpp \
    chequewindow.cpp \
    chequelistmodel.cpp \
    chequelinesmodel.cpp

HEADERS += mainwindow.h \
    webserver.h \
    httprequest.h \
    httpclient.h \
    application.h \
    printservice.h \
    printerbzb2.h \
    cheque.h \
    chequeline.h \
    databaseservice.h \
    chequewindow.h \
    chequelistmodel.h \
    chequelinesmodel.h

FORMS   += mainwindow.ui \
    chequewindow.ui

win32:RC_ICONS += icon.ico
