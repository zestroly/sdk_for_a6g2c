TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.c

unix:!macx: LIBS += -ldbus-1

target.path=/opt
INSTALLS+=target
