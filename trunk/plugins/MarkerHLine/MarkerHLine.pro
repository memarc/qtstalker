TEMPLATE = lib
CONFIG += plugin
MOC_DIR += build
OBJECTS_DIR += build
INCLUDEPATH += ../../src


HEADERS += MarkerHLine.h
SOURCES += MarkerHLine.cpp
HEADERS += MarkerHLineDialog.h
SOURCES += MarkerHLineDialog.cpp
HEADERS += MarkerHLineObject.h
SOURCES += MarkerHLineObject.cpp
HEADERS += ../../src/Object.h
HEADERS += ../../src/Util.h
HEADERS += ../../src/Plugin.h
HEADERS += ../../src/PluginCommand.h
HEADERS += ../../src/ObjectCommand.h
HEADERS += ../../src/Dialog.h

OBJECTS += ../../src/build/Dialog.o
OBJECTS += ../../src/build/Object.o
OBJECTS += ../../src/build/ObjectCommand.o
OBJECTS += ../../src/build/Util.o


HOME=$$system(echo $HOME) 
target.path =  $${HOME}/OTA/lib
INSTALLS += target

QT += core
QT += gui
