TEMPLATE = lib
CONFIG += plugin
MOC_DIR += build
OBJECTS_DIR += build
INCLUDEPATH += ../../src


HEADERS += YahooHistory.h
SOURCES += YahooHistory.cpp
HEADERS += YahooHistoryObject.h
SOURCES += YahooHistoryObject.cpp
HEADERS += YahooHistoryThread.h
SOURCES += YahooHistoryThread.cpp
HEADERS += YahooHistoryWidget.h
SOURCES += YahooHistoryWidget.cpp
HEADERS += ../../src/FileButton.h
SOURCES += ../../src/FileButton.cpp
HEADERS += ../../src/Util.h
SOURCES += ../../src/Util.cpp
HEADERS += ../../src/Object.h
SOURCES += ../../src/Object.cpp
HEADERS += ../../src/PluginCommand.h
HEADERS += ../../src/ObjectCommand.h
SOURCES += ../../src/ObjectCommand.cpp
HEADERS += ../../src/Plugin.h


HOME=$$system(echo $HOME) 
target.path =  $${HOME}/OTA/lib
INSTALLS += target

QT += core
QT += gui
QT += network
