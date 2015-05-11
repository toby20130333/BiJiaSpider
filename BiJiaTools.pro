#-------------------------------------------------
#
# Project created by QtCreator 2015-05-05T13:50:52
#
#-------------------------------------------------

QT       += core gui network webkit sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets webkitwidgets

TARGET = BiJiaTools
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp \
    bijiaarkthread.cpp \
    bijiabehthread.cpp \
    bijiacombthread.cpp

HEADERS  += widget.h \
    bijiaarkthread.h \
    bijiabehthread.h \
    bijiacombthread.h

FORMS    += widget.ui

DISTFILES += \
    comblocks.html
