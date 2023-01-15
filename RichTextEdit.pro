QT       += core gui printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

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

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
