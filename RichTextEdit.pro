QT       += core gui printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    finddialog.cpp \
    main.cpp \
    mainwindow.cpp \
    subwindow.cpp

HEADERS += \
    finddialog.h \
    mainwindow.h \
    subwindow.h

FORMS += \
    finddialog.ui \
    mainwindow.ui

RESOURCES += resources.qrc

TRANSLATIONS += \
    RichTextEdit_cs_CZ.ts
CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
