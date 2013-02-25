TEMPLATE = lib
CONFIG += plugin
MOC_DIR += build
OBJECTS_DIR += build
INCLUDEPATH += ../../src


HEADERS += CSV.h
SOURCES += CSV.cpp
HEADERS += CSVFormat.h
SOURCES += CSVFormat.cpp
HEADERS += CSVObject.h
SOURCES += CSVObject.cpp
HEADERS += CSVThread.h
SOURCES += CSVThread.cpp
HEADERS += CSVWidget.h
SOURCES += CSVWidget.cpp
HEADERS += Delimiter.h
SOURCES += Delimiter.cpp
HEADERS += Quote.h
SOURCES += Quote.cpp
HEADERS += ../../src/Util.h
HEADERS += ../../src/Object.h
HEADERS += ../../src/PluginCommand.h
HEADERS += ../../src/ObjectCommand.h
HEADERS += ../../src/Plugin.h

OBJECTS += ../../src/build/Object.o
OBJECTS += ../../src/build/ObjectCommand.o
OBJECTS += ../../src/build/Util.o


HOME=$$system(echo $HOME) 
target.path =  $${HOME}/OTA/lib
INSTALLS += target

QT += core
QT += gui
