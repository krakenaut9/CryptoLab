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
    accounteditorwindow.cpp \
    activationutils.cpp \
    chpasswindow.cpp \
    fileinfowindow.cpp \
    loginwindow.cpp \
    main.cpp \
    mainwindow.cpp \
    manageaccountswindow.cpp \
    manageusers.cpp \
    questioncheckwindow.cpp \
    technologieswindow.cpp

HEADERS += \
    aboutwindow.h \
    accounteditorwindow.h \
    activationutils.h \
    chpasswindow.h \
    fileinfowindow.h \
    loginwindow.h \
    mainwindow.h \
    manageaccountswindow.h \
    manageusers.h \
    pch.h \
    questioncheckwindow.h \
    technologieswindow.h

FORMS += \
    aboutwindow.ui \
    accounteditorwindow.ui \
    chpasswindow.ui \
    fileinfowindow.ui \
    loginwindow.ui \
    mainwindow.ui \
    manageaccountswindow.ui \
    questioncheckwindow.ui \
    technologieswindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
