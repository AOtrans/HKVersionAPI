#-------------------------------------------------
#
# Project created by QtCreator 2018-07-14T22:35:38
#
#-------------------------------------------------

UI_DIR  = obj/Gui
MOC_DIR = obj/Moc
OBJECTS_DIR = obj/Obj

QT +=	core \
        gui \
        opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = HKVision
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

DEFINES += THREAD_ABLE

SOURCES += main.cpp \
    testform.cpp \
    structs/devicedata.cpp \
    structs/channeldata.cpp \
    tools/IniUtil.cpp \
    tools/commonfuncs.cpp \
    tree/mytreeitem.cpp \
    dialog/devicedialog.cpp \
    tools/senderthread.cpp \
    tools/conversion.cpp \
    tools/pyloader.cpp \
    dialog/displayframe.cpp

HEADERS  += \
    common.h \
    testform.h \
    structs/devicedata.h \
    structs/channeldata.h \
    HK/PlayM4.h \
    HK/LinuxPlayM4.h \
    HK/HCNetSDK.h \
    HK/DemoPublic.h \
    structs/commonstructs.h \
    tools/IniUtil.h \
    tools/commonfuncs.h \
    tree/mytreeitem.h \
    dialog/devicedialog.h \
    tools/senderthread.h \
    tools/conversion.h \
    tools/pyloader.h \
    dialog/displayframe.h

FORMS    += \
    testform.ui \
    dialog/devicedialog.ui

LIBS += -L/home/zg/CH_HCNetSDK_V5.3.5.2_build20171124_Linux64/QtDemo/Linux64/lib -lhcnetsdk -lPlayCtrl -lAudioRender -lSuperRender -lhpr -lHCCore
LIBS += -lopencv_core \
-lopencv_imgproc \
-lopencv_highgui \
-lopencv_ml \
-lopencv_features2d \
-lopencv_calib3d \
-lopencv_objdetect \
-lopencv_contrib \
-lopencv_legacy \
-lopencv_flann

INCLUDEPATH +=  /usr/include/python2.7\
                /usr/include/x86_64-linux-gnu/python2.7\
                /usr/local/lib/python2.7/dist-packages/numpy/core/include\
                -fno-strict-aliasing -Wdate-time -D_FORTIFY_SOURCE=2 -g -fstack-protector-strong -Wformat -Werror=format-security  -DNDEBUG -g -fwrapv -O2 -Wall -Wstrict-prototypes

LIBS += -L/usr/lib/python2.7/config-x86_64-linux-gnu -L/usr/lib -lpython2.7\
        -lpthread -ldl  -lutil -lm  -Xlinker -export-dynamic -Wl,-O1 -Wl,-Bsymbolic-functions
