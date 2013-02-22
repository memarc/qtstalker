TEMPLATE = lib
CONFIG += plugin
MOC_DIR += build
OBJECTS_DIR += build
INCLUDEPATH += ../../src
QMAKE_CXXFLAGS += -fPIC -O2


HEADERS += Symbol.h
SOURCES += Symbol.cpp
HEADERS += SymbolDialog.h
SOURCES += SymbolDialog.cpp
HEADERS += SymbolObject.h
SOURCES += SymbolObject.cpp
HEADERS += SymbolSelectDialog.h
SOURCES += SymbolSelectDialog.cpp
HEADERS += ../../src/Object.h
#SOURCES += ../../src/Object.cpp
HEADERS += ../../src/Util.h
#SOURCES += ../../src/Util.cpp
HEADERS += ../../src/PluginCommand.h
HEADERS += ../../src/ObjectCommand.h
#SOURCES += ../../src/ObjectCommand.cpp
HEADERS += ../../src/Plugin.h
#SOURCES += ../../src/Dialog.cpp
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
