#-------------------------------------------------
#
# Project created by QtCreator 2011-07-17T16:33:38
#
#-------------------------------------------------

QT       += core gui

TARGET = everestpm
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    platform_unix.c \
    platform_win32.c \
    mbr.c \
    log.c \
    everestpm.c

HEADERS  += mainwindow.h \
    parttypes.h \
    mbr.h \
    everestpm.h

FORMS    += mainwindow.ui
