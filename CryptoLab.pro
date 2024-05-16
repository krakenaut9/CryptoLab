QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += \
    $$PWD\plog\include \

SOURCES += \
    aboutwindow.cpp \
    fileinfowindow.cpp \
    main.cpp \
    mainwindow.cpp \
    technologieswindow.cpp

HEADERS += \
    aboutwindow.h \
    fileinfowindow.h \
    mainwindow.h \
    pch.h \
    technologieswindow.h

FORMS += \
    aboutwindow.ui \
    fileinfowindow.ui \
    mainwindow.ui \
    technologieswindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
