#-------------------------------------------------
#
# Project created by QtCreator 2010-06-02T08:59:18
#
#-------------------------------------------------

QT       += core gui
QT       += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets webkitwidgets

INCLUDEPATH += $$PWD
SOURCES += \
    $$PWD/tcpclient.cpp \
    $$PWD/tcpserver.cpp \
    $$PWD/chat.cpp \
    $$PWD/bjchatwidget.cpp

HEADERS  += \
    $$PWD/tcpclient.h \
    $$PWD/tcpserver.h \
    $$PWD/chat.h \
    $$PWD/bjchatwidget.h

FORMS    += \
    $$PWD/tcpclient.ui \
    $$PWD/tcpserver.ui \
    $$PWD/chat.ui \
    $$PWD/bjchatwidget.ui


RESOURCES += \
    $$PWD/resource.qrc
