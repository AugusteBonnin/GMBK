#-------------------------------------------------
#
# Project created by QtCreator 2018-08-13T18:07:27
#
#-------------------------------------------------

QT       += core gui xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MyGameBookEditor
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
INCLUDEPATH += /opt/local/include

LIBS += -L. -lhunspell-1.7

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    document.cpp \
    form.cpp \
    narytreedialog.cpp \
    textedit.cpp \
    booleanalgebradialog.cpp \
    eventlist.cpp \
    binarytreedialog.cpp \
    pseudorandomdialog.cpp

HEADERS += \
        mainwindow.h \
    document.h \
    form.h \
    narytreedialog.h \
    textedit.h \
    booleanalgebradialog.h \
    eventlist.h \
    binarytreedialog.h \
    pseudorandomdialog.h

FORMS += \
        mainwindow.ui \
    form.ui \
    booleanalgebradialog.ui \
    eventlist.ui \
    binarytreedialog.ui \
    narytreedialog.ui \
    pseudorandomdialog.ui

RESOURCES += \
    resources.qrc
