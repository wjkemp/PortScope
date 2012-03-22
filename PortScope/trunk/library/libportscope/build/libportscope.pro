Release:DESTDIR = ../lib
Release:OBJECTS_DIR = ../lib/release/.obj
Release:MOC_DIR = ../lib/release/.moc
Release:RCC_DIR = ../lib/release/.rcc
Release:UI_DIR = ../lib/release/.ui

Debug:DESTDIR = ../lib/debug
Debug:OBJECTS_DIR = ../lib/debug/.obj
Debug:MOC_DIR = ../lib/debug/.moc
Debug:RCC_DIR = ../lib/debug/.rcc
Debug:UI_DIR = ../lib/debug/.ui


TEMPLATE = lib
TARGET = libportscope
CONFIG -= QT
QT -= core gui

INCLUDEPATH += ../include \
            ../source

DEFINES += LIBPORTSCOPE_EXPORTS            

HEADERS += ../include/libps.h
                    
SOURCES += ../source/libps.c
