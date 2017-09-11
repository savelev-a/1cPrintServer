#-------------------------------------------------
#
# Project created by QtCreator 2017-07-27T13:02:20
#
#-------------------------------------------------

QT       += core gui widgets network printsupport sql serialport

TARGET = 1cPrintServer
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS


SOURCES += src/main.cpp\
           src/ui/mainwindow.cpp \
    src/network/webserver.cpp \
    src/network/httprequest.cpp \
    src/network/httpclient.cpp \
    src/application.cpp \
    src/print/printservice.cpp \
    src/print/printerbzb2.cpp \
    src/model/cheque.cpp \
    src/model/chequeline.cpp \
    src/databaseservice.cpp \
    src/ui/chequewindow.cpp \
    src/model/chequelistmodel.cpp \
    src/model/chequelinesmodel.cpp \
    src/print/winprinter.cpp

HEADERS += src/ui/mainwindow.h \
    src/network/webserver.h \
    src/network/httprequest.h \
    src/network/httpclient.h \
    src/application.h \
    src/print/printservice.h \
    src/print/printerbzb2.h \
    src/model/cheque.h \
    src/model/chequeline.h \
    src/databaseservice.h \
    src/ui/chequewindow.h \
    src/model/chequelistmodel.h \
    src/model/chequelinesmodel.h \
    src/print/winprinter.h

FORMS   += src/ui/mainwindow.ui \
    src/ui/chequewindow.ui

win32:RC_ICONS += icon.ico
win32:LIBS += -lwinspool

RESOURCES += \
    res.qrc
