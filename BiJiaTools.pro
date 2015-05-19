#-------------------------------------------------
#
# Project created by QtCreator 2015-05-05T13:50:52
#
#-------------------------------------------------

QT       += core gui network webkit sql
VERSION = 1.0
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets webkitwidgets

TARGET = BiJiaTools
TEMPLATE = app
#CONFIG += console


# Fervor autoupdater
# (set TARGET and VERSION of your app before including Fervor.pri)
!include("./fervor/fervor.pri") {
        error("Unable to include Fervor autoupdater.")
}

win32:{
    RC_FILE += BiJiaTools.rc
}
SOURCES += main.cpp\
        widget.cpp \
    bijiaarkthread.cpp \
    bijiabehthread.cpp \
    bijiacombthread.cpp \
    bijiaoakthread.cpp \
    bijiaaokthread.cpp \
    bijiaastatechthread.cpp

HEADERS  += widget.h \
    bijiaarkthread.h \
    bijiabehthread.h \
    bijiacombthread.h \
    bijiaoakthread.h \
    bijiaaokthread.h \
    bijiaastatechthread.h

FORMS    += widget.ui

DISTFILES += \
    comblocks.html

#RESOURCES += \
#    bijia.qrc
