#-------------------------------------------------
#
# Project created by QtCreator 2018-03-20T10:22:52
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ILI9225_LCDTest
TEMPLATE = app
CONFIG += qt warn_on release

#QMAKE_CXXFLAGS_RELEASE += -O3

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH = /usr/local/include/opencv \
              /usr/local/include/opencv2 \
              /usr/local/include \
              /usr/include \

LIBS += /usr/local/lib/libopencv_*.so \
        /usr/local/lib/libbcm2835.a

SOURCES  += main.cpp\
            mainwindow.cpp \
            ili9225.cpp \
            convert.cpp

HEADERS  += mainwindow.h \
            ili9225.h \
            convert.h

FORMS    += mainwindow.ui
