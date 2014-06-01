#-------------------------------------------------
#
# Project created by QtCreator 2014-05-17T17:04:02
#
#-------------------------------------------------

QT       += core gui

TARGET = BIT_Shadows
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    Utilities.cpp \
    sakbot.cpp

HEADERS  += mainwindow.h \
    Utilities.h \
    sakbot.h

FORMS    += mainwindow.ui

INCLUDEPATH += /usr/local/include/opencv

LIBS += -L/usr/local/lib \
-lopencv_core \
-lopencv_imgproc \
-lopencv_highgui \
-lopencv_ml \
-lopencv_video \
-lopencv_features2d \
-lopencv_calib3d \
-lopencv_objdetect \
-lopencv_contrib \
-lopencv_legacy \
-lopencv_flann
