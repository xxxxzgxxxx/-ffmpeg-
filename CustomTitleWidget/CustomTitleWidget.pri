
INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

SOURCES += \
        $$PWD/frmmain.cpp \
    $$PWD/iconhelper.cpp \
    $$PWD/frmmessagebox.cpp \
    CustomTitleWidget/customtitle.cpp

HEADERS  += $$PWD/frmmain.h \
    $$PWD/iconhelper.h \
    $$PWD/frmmessagebox.h \
    $$PWD/myhelper.h \
    CustomTitleWidget/customtitle.h

FORMS    += $$PWD/frmmain.ui \
    $$PWD/frmmessagebox.ui \
    CustomTitleWidget/customtitle.ui


RESOURCES += \
    $$PWD/rc.qrc
