QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    embedmonclient.cpp \
    main.cpp

HEADERS += \
    embedmonclient.h

FORMS += \
    widget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc

DISTFILES += \
    images/battlebackground.png \
    sprites/FreezeAttack_fl.gif \
    sprites/FreezeIdle_fl.gif \
    sprites/FreezionIdle.gif \
    sprites/IgnisIdle.gif \
    sprites/RockyAttack_fr.gif \
    sprites/RockyIdle_fr.gif \
    sprites/freezon_sprite0.png \
    sprites/freezon_sprite1.png \
    sprites/freezon_sprite2.png \
    sprites/ignis_sprite0.png \
    sprites/ignis_sprite1.png \
    sprites/ignis_sprite2.png
