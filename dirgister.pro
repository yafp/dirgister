#-------------------------------------------------
#
# Project created by QtCreator
#
#-------------------------------------------------
QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = dirgister
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp
HEADERS  += mainwindow.h
FORMS    += mainwindow.ui


# Resources like images etc
#
RESOURCES     = dirgister.qrc


# app icon
#
RC_FILE = dirgister.rc          # Windows
ICON = dirgister.icns           # Mac
