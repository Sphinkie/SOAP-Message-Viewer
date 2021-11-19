QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

VERSION = 1.3.0

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Sources/BodyPrintableParser.cpp \
    Sources/BodyTableParser.cpp \
    Sources/BodyTextParser.cpp \
    Sources/BodyTreeParser.cpp \
    Sources/Common.cpp \
    Sources/FileConverter.cpp \
    Sources/FileParser.cpp \
    Sources/FileRepair.cpp \
    Sources/FileSplitter.cpp \
    Sources/BodyPrintableParser.cpp \
    Sources/BodyTableParser.cpp \
    Sources/main.cpp \
    Sources/mainwindow.cpp \
    Sources/messages.cpp

HEADERS += \
    Sources/BodyPrintableParser.h \
    Sources/BodyTableParser.h \
    Sources/BodyTextParser.h \
    Sources/BodyTreeParser.h \
    Sources/Common.h \
    Sources/FileConverter.h \
    Sources/FileParser.h \
    Sources/FileRepair.h \
    Sources/FileSplitter.h \
    Sources/BodyPrintableParser.h \
    Sources/mainwindow.h \
    Sources/messages.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


win32: LIBS += -L$$PWD/Expat/lib/ -llibexpat
win32: RC_ICONS += $$PWD/resources/soapOutlined.ico

INCLUDEPATH += $$PWD/Expat/include
DEPENDPATH += $$PWD/Expat/include

DISTFILES +=

RESOURCES += \
    resources.qrc
