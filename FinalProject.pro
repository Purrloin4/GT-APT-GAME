QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++20

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    controller/graphicviewcontroller.cpp \
    controller/textviewcontroller.cpp \
    controller/worldcontroller.cpp \
    main.cpp \
    mainwindow.cpp \
    viewcontroller.cpp \
    worldcontroller.cpp

HEADERS += \
    controller/graphicviewcontroller.h \
    controller/textviewcontroller.h \
    controller/viewcontroller.h \
    controller/worldcontroller.h \
    mainwindow.h \
    viewcontroller.h \
    worldcontroller.h \
    pathNode.h

FORMS += \
    mainwindow.ui

RESOURCES += \
    images.qrc

QMAKE_CXXFLAGS += -fconcepts-diagnostics-depth=200

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32: LIBS += -L$$PWD/../WorldSource/release/ -lworld

INCLUDEPATH += $$PWD/../WorldSource
DEPENDPATH += $$PWD/../WorldSource
