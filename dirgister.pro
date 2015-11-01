#-------------------------------------------------
#
# Project created by QtCreator 2015-10-04T14:04:10
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets


RESOURCES     = dirgister.qrc


TARGET = dirgister
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui


# app icon
#
RC_FILE = dirgister.rc          # Windows
ICON = dirgister.icns           # Mac
