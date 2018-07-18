TEMPLATE = app
CONFIG += console gnu++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    ZBluetoothServer.cpp \
    agent.cpp

HEADERS += \
    ZBluetoothServer.h

target.path=/opt
INSTALLS+=target

unix:!macx: LIBS += -lbluetooth -pthread -ldbus-1
QMAKE_CXXFLAGS += -std=gnu++11

