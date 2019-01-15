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
    dialog/devicedialog.cpp \
    tools/senderthread.cpp \
    dialog/displayframe.cpp \
    dialog/gifdialog.cpp \
    tree/mylefttreeitem.cpp \
    tree/myrighttreeitem.cpp \
    tfModelClient/model.grpc.pb.cc \
    tfModelClient/model.pb.cc \
    tfModelClient/predict.grpc.pb.cc \
    tfModelClient/predict.pb.cc \
    tfModelClient/prediction_service.grpc.pb.cc \
    tfModelClient/prediction_service.pb.cc \
    tfModelClient/resource_handle.grpc.pb.cc \
    tfModelClient/resource_handle.pb.cc \
    tfModelClient/tensor_shape.grpc.pb.cc \
    tfModelClient/tensor_shape.pb.cc \
    tfModelClient/tensor.grpc.pb.cc \
    tfModelClient/tensor.pb.cc \
    tfModelClient/tfserverclient.cpp \
    tfModelClient/types.grpc.pb.cc \
    tfModelClient/types.pb.cc \
    tools/grpcpredictor.cpp

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
    dialog/devicedialog.h \
    tools/senderthread.h \
    dialog/displayframe.h \
    dialog/gifdialog.h \
    tree/mylefttreeitem.h \
    tree/myrighttreeitem.h \
    tfModelClient/model.grpc.pb.h \
    tfModelClient/model.pb.h \
    tfModelClient/predict.grpc.pb.h \
    tfModelClient/predict.pb.h \
    tfModelClient/prediction_service.grpc.pb.h \
    tfModelClient/prediction_service.pb.h \
    tfModelClient/resource_handle.grpc.pb.h \
    tfModelClient/resource_handle.pb.h \
    tfModelClient/tensor_shape.grpc.pb.h \
    tfModelClient/tensor_shape.pb.h \
    tfModelClient/tensor.grpc.pb.h \
    tfModelClient/tensor.pb.h \
    tfModelClient/tfserverclient.h \
    tfModelClient/types.grpc.pb.h \
    tfModelClient/types.pb.h \
    tools/grpcpredictor.h

FORMS    += \
    testform.ui \
    dialog/devicedialog.ui \
    dialog/gifdialog.ui

LIBS += -L/home/sw/Work/QtProject/build-HKVision-Desktop_Qt_5_7_1_GCC_64bit-Debug/ -lhcnetsdk -lPlayCtrl -lAudioRender -lSuperRender -lhpr -lHCCore
LIBS += -L/usr/local/lib/ -lopencv_core \
-lopencv_imgproc \
-lopencv_highgui \
-lopencv_ml \
-lopencv_features2d \
-lopencv_calib3d \
-lopencv_objdetect \
-lopencv_contrib \
-lopencv_legacy \
-lopencv_flann

LIBS += -lprotobuf -pthread -lgrpc++ -lgrpc -Wl,--no-as-needed -lgrpc++_reflection -Wl,--as-needed -ldl

DISTFILES +=
