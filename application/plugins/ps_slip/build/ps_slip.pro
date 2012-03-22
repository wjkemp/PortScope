Release:DESTDIR = ../bin/release
Release:OBJECTS_DIR = ../bin/release/.obj
Release:MOC_DIR = ../bin/release/.moc
Release:RCC_DIR = ../bin/release/.rcc
Release:UI_DIR = ../bin/release/.ui

Debug:DESTDIR = ../bin/debug
Debug:OBJECTS_DIR = ../bin/debug/.obj
Debug:MOC_DIR = ../bin/debug/.moc
Debug:RCC_DIR = ../bin/debug/.rcc
Debug:UI_DIR = bin/debug/.ui


TEMPLATE = lib
TARGET = ps_slip

INCLUDEPATH += ../../../include \
            ../source

HEADERS += ../source/analyzer.h \
            ../source/displaymodel.h \
            ../source/displayview.h \
            ../source/ps_slip.h \
            ../source/slipframe.h \
            ../source/slipparser.h
            
            
                    
SOURCES += ../source/analyzer.cpp \
            ../source/displaymodel.cpp \
            ../source/displayview.cpp \
            ../source/ps_slip.cpp \
            ../source/slipframe.cpp \
            ../source/slipparser.cpp
