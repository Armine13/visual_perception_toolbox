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
    imageprocessing.cpp \
    filters.cpp \
    GUI/window.cpp \
    GUI/player.cpp


HEADERS  += GUI/mainwindow.h \
    imageprocessing.h \
    filters.h \
    GUI/window.h \
    GUI/player.h


FORMS    += GUI/mainwindow.ui \


#Linux
#CONFIG += link_pkgconfig
#PKGCONFIG += opencv

#RESOURCES += \
#    GUI/tangomfk.qrc

#Windows
#INCLUDEPATH += C:\opencv-mingw\eigen-eigen-c58038c56923

INCLUDEPATH += C:\opencv-mingw\install\include
LIBS += -L"C:/opencv-mingw/install/x64/mingw/bin"
LIBS += -lopencv_core2410 -lopencv_highgui2410 -lopencv_imgproc2410

