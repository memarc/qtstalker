######################################################
# FILES
#####################################################

HEADERS += ColorButton.h
SOURCES += ColorButton.cpp
HEADERS += Dialog.h
SOURCES += Dialog.cpp
HEADERS += FileButton.h
SOURCES += FileButton.cpp
HEADERS += FontButton.h
SOURCES += FontButton.cpp
HEADERS += InputObjectWidget.h
SOURCES += InputObjectWidget.cpp
SOURCES += main.cpp
HEADERS += Object.h
SOURCES += Object.cpp
HEADERS += ObjectCommand.h
SOURCES += ObjectCommand.cpp
HEADERS += OTA.h
SOURCES += OTA.cpp
HEADERS += Plugin.h
HEADERS += PluginCommand.h
HEADERS += Util.h
SOURCES += Util.cpp


######################################################
# QMAKE SETTINGS
#####################################################

TEMPLATE = app

CONFIG += qt thread warn_on debug
#CONFIG(debug, debug|release): DEFINES += DEBUG _DEBUG

QMAKE_CXXFLAGS += -fPIC -O2

#include(../.qmake.cache)

TARGET = ota

message("Using INCLUDEPATH=$$INCLUDEPATH")
message("Using LIBS=$$LIBS")

HOME=$$system(echo $HOME) 
target.path =  $${HOME}/OTA/bin
INSTALLS += target

MOC_DIR += build
OBJECTS_DIR += build

QT += core
QT += gui
