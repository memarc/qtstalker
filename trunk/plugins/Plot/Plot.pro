TEMPLATE = lib
CONFIG += plugin
MOC_DIR += build
OBJECTS_DIR += build
INCLUDEPATH += ../../src
QMAKE_CXXFLAGS += -fPIC -O2


HEADERS += Plot.h
SOURCES += Plot.cpp
HEADERS += PlotDialog.h
SOURCES += PlotDialog.cpp
HEADERS += PlotObject.h
SOURCES += PlotObject.cpp
HEADERS += PlotWidget.h
SOURCES += PlotWidget.cpp
HEADERS += PlotWidgetArea.h
SOURCES += PlotWidgetArea.cpp
HEADERS += PlotWidgetCursorInfo.h
SOURCES += PlotWidgetCursorInfo.cpp
HEADERS += PlotWidgetDate.h
SOURCES += PlotWidgetDate.cpp
HEADERS += PlotWidgetDateTick.h
SOURCES += PlotWidgetDateTick.cpp
HEADERS += PlotWidgetGrid.h
SOURCES += PlotWidgetGrid.cpp
HEADERS += PlotWidgetScale.h
SOURCES += PlotWidgetScale.cpp
HEADERS += PlotWidgetStatus.h
HEADERS += XMap.h
SOURCES += XMap.cpp
HEADERS += YMap.h
SOURCES += YMap.cpp
HEADERS += ../../src/Object.h
#SOURCES += ../../src/Object.cpp
HEADERS += ../../src/Util.h
#SOURCES += ../../src/Util.cpp
HEADERS += ../../src/Plugin.h
HEADERS += ../../src/PluginCommand.h
#SOURCES += ../../src/ObjectCommand.cpp
HEADERS += ../../src/ObjectCommand.h
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
