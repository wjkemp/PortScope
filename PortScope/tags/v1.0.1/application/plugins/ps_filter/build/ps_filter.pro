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
TARGET = ps_filter

INCLUDEPATH += ../../../include \
            ../source

HEADERS += ../source/analyzer.h \
            ../source/ps_filter.h
                    
SOURCES += ../source/analyzer.cpp \
            ../source/ps_filter.cpp
