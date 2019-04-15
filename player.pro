#-------------------------------------------------
#
# Project created by QtCreator 2018-10-08T14:23:49
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = player
TEMPLATE = app

include(CustomTitleWidget/CustomTitleWidget.pri)

SOURCES += main.cpp\
    surfacestyle.cpp \
    videoplayer.cpp \
    videoplayer_thread.cpp \
    videoplayer_showvideowidget.cpp \
    videoslider.cpp \
    volumeslider.cpp

HEADERS  += \
    surfacestyle.h \
    videoplayer.h \
    videoplayer_thread.h \
    videoplayer_showvideowidget.h \
    videoslider.h \
    volumeslider.h

FORMS    += \
    surfacestyle.ui \
    videoplayer.ui \
    videoplayer_showvideowidget.ui

INCLUDEPATH    += $$PWD/ffmpeg/include \
                  $$PWD/SDL2/include


LIBS     += $$PWD/ffmpeg/lib/avcodec.lib \
            $$PWD/ffmpeg/lib/avformat.lib \
            $$PWD/ffmpeg/lib/avdevice.lib \
            $$PWD/ffmpeg/lib/avutil.lib \
            $$PWD/ffmpeg/lib/avfilter.lib \
            $$PWD/ffmpeg/lib/swscale.lib \
            $$PWD/ffmpeg/lib/swresample.lib \
            $$PWD/ffmpeg/lib/postproc.lib \
            $$PWD/SDL2/lib/x86/SDL2.lib


RESOURCES += \
    image.qrc
