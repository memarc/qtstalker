TEMPLATE = lib
CONFIG += plugin
MOC_DIR += build
OBJECTS_DIR += build
INCLUDEPATH += ../../src


HEADERS += Scanner.h
SOURCES += Scanner.cpp
HEADERS += ScannerObject.h
SOURCES += ScannerObject.cpp
HEADERS += ScannerThread.h
SOURCES += ScannerThread.cpp
HEADERS += ScannerWidget.h
SOURCES += ScannerWidget.cpp
HEADERS += ../../src/Util.h
HEADERS += ../../src/Object.h
HEADERS += ../../src/PluginCommand.h
HEADERS += ../../src/ObjectCommand.h
HEADERS += ../../src/Plugin.h
HEADERS += ../../src/Bars.h

OBJECTS += ../../src/build/Object.o
OBJECTS += ../../src/build/ObjectCommand.o
OBJECTS += ../../src/build/Util.o
OBJECTS += ../../src/build/Bars.o


HOME=$$system(echo $HOME) 
target.path =  $${HOME}/OTA/lib
INSTALLS += target

QT += core
QT += gui
