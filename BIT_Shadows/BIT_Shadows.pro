#-------------------------------------------------
#
# Project created by QtCreator 2014-05-17T17:04:02
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = BIT_Shadows
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    sebg.cpp \
    parameterdialog.cpp \
    sakbotwrapper.cpp \
    sakbot.cpp \
    utilities.cpp \
    SebgUtils.cpp

HEADERS  += mainwindow.h \
    sebg.h \
    parameterdialog.h \
    sakbotwrapper.h \
    sakbot.h \
    utilities.h \
    SebgUtils.h

FORMS    += mainwindow.ui \
    parameterdialog.ui

INCLUDEPATH += /usr/local/include/opencv
LIBS += -L/usr/local/lib
LIBS += -lopencv_core
LIBS += -lopencv_imgproc
LIBS += -lopencv_highgui
LIBS += -lopencv_ml
LIBS += -lopencv_video
LIBS += -lopencv_features2d
LIBS += -lopencv_calib3d
LIBS += -lopencv_objdetect
LIBS += -lopencv_contrib
LIBS += -lopencv_legacy
LIBS += -lopencv_flann
