Release:DESTDIR = ../bin/release
Release:OBJECTS_DIR = ../bin/release/.obj
Release:MOC_DIR = ../bin/release/.moc
Release:RCC_DIR = ../bin/release/.rcc
Release:UI_DIR = ../bin/release/.ui
Release:LIBS += ../../library/libportscope/lib/libportscope.lib

Debug:DESTDIR = ../bin/debug
Debug:OBJECTS_DIR = ../bin/debug/.obj
Debug:MOC_DIR = ../bin/debug/.moc
Debug:RCC_DIR = ../bin/debug/.rcc
Debug:UI_DIR = ../bin/debug/.ui
Debug:LIBS += ../../library/libportscope/lib/debug/libportscope.lib


TEMPLATE = app
TARGET = PortScope
QT += xml

INCLUDEPATH += ../include \
            ../source \
            ../../library/libportscope/include
            

HEADERS += ../include/plugin.h \
           ../include/protocolanalyzer.h \
           ../source/mainwindow.h \
           ../source/version.h \
           ../source/captureengine/capturedevice.h \
           ../source/captureengine/captureengine.h \
           ../source/captureengine/captureengineconfiguration.h \
           ../source/captureengine/captureengineconfigurationdialog.h \
           ../source/protocolstack/protocolstack.h \
           ../source/protocolstack/protocolstackitem.h \
           ../source/protocolstack/protocolstackmodel.h \
           ../source/protocolstack/protocolstackview.h \
           ../source/protocolstack/rootdispatcher.h
           
FORMS += ../source/mainwindow.ui \
         ../source/captureengine/captureengineconfigurationdialog.ui
         
SOURCES += ../source/main.cpp \
           ../source/mainwindow.cpp \
           ../source/captureengine/capturedevice.cpp \
           ../source/captureengine/captureengine.cpp \
           ../source/captureengine/captureengineconfiguration.cpp \
           ../source/captureengine/captureengineconfigurationdialog.cpp \
           ../source/protocolstack/protocolstack.cpp \
           ../source/protocolstack/protocolstackitem.cpp \
           ../source/protocolstack/protocolstackmodel.cpp \
           ../source/protocolstack/protocolstackview.cpp \
           ../source/protocolstack/rootdispatcher.cpp
           
RESOURCES += ../resources/mainwindow.qrc
