# QT       += core gui openglwidgets

QT       += core gui opengl charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
greaterThan(QT_MAJOR_VERSION, 5): QT += openglwidgets

CONFIG += \
    c++17 \
    sdk_no_version_check \

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ../model/model.cc \
    ../model/graph_neuron.cc \
    ../model/graph_layer.cc \
    ../model/graph_perceptron.cc \
    main.cc \
    paintscene.cc \
    view.cc

HEADERS += \
    ../model/model.h \
    ../model/graph_neuron.h \
    ../model/graph_layer.h \
    ../model/graph_perceptron.h \
    ../controller/controller.h \
    ../model/neural_network.h \
    paintscene.h \
    view.h

FORMS += \
    view.ui
# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \

RESOURCES +=
