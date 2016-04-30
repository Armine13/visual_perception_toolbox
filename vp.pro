#-------------------------------------------------
#
# Project created by QtCreator 2016-02-20T12:44:34
#
#-------------------------------------------------

QT       += core gui
CONFIG += c++11
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = vp
TEMPLATE = app


SOURCES += main.cpp\
    thirdParty/cvMatToQImage.cpp \
    GUI/mainwindow.cpp \
    filters.cpp \
    GUI/window.cpp \
    GUI/videowindow.cpp \
    GUI/sliderwidget.cpp \
    cameraCalibration/cameraCalib.cpp


HEADERS  += GUI/mainwindow.h \
    GUI/window.h \
    GUI/videowindow.h \
    GUI/sliderwidget.h


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
LIBS += -lopencv_core2410 -lopencv_highgui2410 -lopencv_imgproc2410  -lopencv_features2d2410 -lopencv_calib3d2410 -lopencv_nonfree2410 -lopencv_flann2410 -lopencv_stitching2410
