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
RCC_DIR = ./bin
UI_DIR = ./bin
RCC_DIR = ./bin
OBJECTS_DIR = ./bin

# Fervor autoupdater
# (set TARGET and VERSION of your app before including Fervor.pri)
!include("./fervor/fervor.pri") {
        error("Unable to include Fervor autoupdater.")
}
!include("./chat/chat.pri") {
        error("Unable to include chat library")
}

win32:{
    RC_FILE += BiJiaTools.rc
    DESTDIR = ./bin/win32
}
macx:{
    DESTDIR = ./bin/macosx
}
SOURCES += main.cpp\
        bjwidget.cpp \
    bijiaarkthread.cpp \
    bijiabehthread.cpp \
    bijiacombthread.cpp \
    bijiaoakthread.cpp \
    bijiaaokthread.cpp \
    bijiaastatechthread.cpp \
    bjsystem_tray.cpp \
    bjmainwidget.cpp

HEADERS  += bjwidget.h \
    bijiaarkthread.h \
    bijiabehthread.h \
    bijiacombthread.h \
    bijiaoakthread.h \
    bijiaaokthread.h \
    bijiaastatechthread.h \
    bjsystem_tray.h \
    bjmainwidget.h

FORMS    += \
    bjwidget.ui \
    bjstackedwidget.ui \
    bjmainwidget.ui

DISTFILES += \
    comblocks.html

RESOURCES += \
    bijia.qrc
