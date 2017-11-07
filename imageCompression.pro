TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.c \
    dataGeneration.c \
    imageCompression.c

HEADERS += \
    datageneration.h \
    imagecompression.h
