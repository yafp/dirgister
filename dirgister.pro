QT += widgets

HEADERS       = mainwindow.h
SOURCES       = main.cpp \
                mainwindow.cpp

RESOURCES     = dirgister.qrc


# fidel
CONFIG += debug


# install
target.path = $$[QT_INSTALL_EXAMPLES]/widgets/mainwindows/application
INSTALLS += target
