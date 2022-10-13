#-------------------------------------------------
#
# Project created by QtCreator 2010-12-19T15:33:49
#
#-------------------------------------------------

QT       += core gui network serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QtComPort
TEMPLATE = app

unix:!mac {
  LIBS += -Wl,-rpath=\\\$$ORIGIN/libs
}

SOURCES += main.cpp\
    qcsslserver.cpp \
    qdterminallogformats.cpp \
        qfmainform.cpp \
    qdopencomport.cpp \
    qdfontdialog.cpp \
    qdterminal.cpp \
    qdbridge.cpp \
    qdcolors.cpp

HEADERS  += qfmainform.h \
    qcsslserver.h \
    qdopencomport.h \
    qdfontdialog.h \
    qdterminal.h \
    qdbridge.h \
    qdcolors.h \
    qdterminallogformats.h

FORMS    += qfmainform.ui \
    qdopencomport.ui \
    qdfontdialog.ui \
    qdterminal.ui \
    qdbridge.ui \
    qdcolors.ui \
    qdterminallogformats.ui

OTHER_FILES += \
    qt.conf \
    QtComPort_it.ts

RESOURCES += \
    Resources.qrc

TRANSLATIONS  = QtComPort_it.ts

RC_FILE = QtComPort.rc

ICON = QtComPort.icns

DISTFILES += \
    QtComPort.rc
