#-------------------------------------------------
#
# Project created by QtCreator 2016-02-20T12:44:34
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = vp
TEMPLATE = app


SOURCES += main.cpp\
    thirdParty/cvMatToQImage.cpp \
    GUI/mainwindow.cpp \
    imageprocessing.cpp


HEADERS  += GUI/mainwindow.h \
    imageprocessing.h


FORMS    += GUI/mainwindow.ui \


#OpenCV
CONFIG += link_pkgconfig
PKGCONFIG += opencv

RESOURCES += \
    GUI/tangomfk.qrc
