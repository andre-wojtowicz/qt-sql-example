VERSION = 1.2
QMAKE_TARGET_DESCRIPTION = "Qt app that displays SQL table; for educational purposes"
QMAKE_TARGET_COPYRIGHT   = "Andrzej Wojtowicz, Adam Mickiewicz University in Poznan"
QMAKE_TARGET_PRODUCT     = "Qt SQL Example"

QT      += core gui sql widgets

TARGET   = qt-sql-example
TEMPLATE = app

SOURCES += src/main.cpp\
           src/mainwindow.cpp \
           src/db_controller.cpp

HEADERS += src/mainwindow.h \
           src/db_controller.h

FORMS   += src/mainwindow.ui

RESOURCES += res/icons.qrc

win32:RC_ICONS += res/database.ico
