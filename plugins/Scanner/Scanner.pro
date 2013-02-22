TEMPLATE = lib
CONFIG += plugin
MOC_DIR += build
OBJECTS_DIR += build
INCLUDEPATH += ../../src
QMAKE_CXXFLAGS += -fPIC -O2


HEADERS += BarLengthButton.h
SOURCES += BarLengthButton.cpp
HEADERS += DateRangeDialog.h
SOURCES += DateRangeDialog.cpp
HEADERS += RangeButton.h
SOURCES += RangeButton.cpp
HEADERS += Scanner.h
SOURCES += Scanner.cpp
HEADERS += ScannerObject.h
SOURCES += ScannerObject.cpp
HEADERS += ScannerThread.h
SOURCES += ScannerThread.cpp
HEADERS += ScannerWidget.h
SOURCES += ScannerWidget.cpp
HEADERS += ../../src/Util.h
#SOURCES += ../../src/Util.cpp
HEADERS += ../../src/Object.h
#SOURCES += ../../src/Object.cpp
HEADERS += ../../src/PluginCommand.h
HEADERS += ../../src/ObjectCommand.h
#SOURCES += ../../src/ObjectCommand.cpp
HEADERS += ../../src/Plugin.h
HEADERS += ../../src/Dialog.h
#SOURCES += ../../src/Dialog.cpp

OBJECTS += ../../src/build/Dialog.o
OBJECTS += ../../src/build/Object.o
OBJECTS += ../../src/build/ObjectCommand.o
OBJECTS += ../../src/build/Util.o


HOME=$$system(echo $HOME) 
target.path =  $${HOME}/OTA/lib
INSTALLS += target

QT += core
QT += gui
