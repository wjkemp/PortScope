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
TARGET = ps_raw

INCLUDEPATH += ../../../include \
            ../source

HEADERS += ../source/analyzer.h \
            ../source/display.h \
            ../source/ps_raw.h
                    
SOURCES += ../source/analyzer.cpp \
            ../source/display.cpp \
            ../source/ps_raw.cpp
