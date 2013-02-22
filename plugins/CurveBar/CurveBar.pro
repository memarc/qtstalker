TEMPLATE = lib
CONFIG += plugin
MOC_DIR += build
OBJECTS_DIR += build
INCLUDEPATH += ../../src


HEADERS += CurveBar.h
SOURCES += CurveBar.cpp
HEADERS += CurveBarDialog.h
SOURCES += CurveBarDialog.cpp
HEADERS += CurveBarObject.h
SOURCES += CurveBarObject.cpp
HEADERS += ../../src/ColorButton.h
SOURCES += ../../src/ColorButton.cpp
SOURCES += ../../src/Dialog.cpp
HEADERS += ../../src/Dialog.h
HEADERS += ../../src/Object.h
SOURCES += ../../src/Object.cpp
HEADERS += ../../src/Util.h
SOURCES += ../../src/Util.cpp
HEADERS += ../../src/Plugin.h
HEADERS += ../../src/PluginCommand.h
HEADERS += ../../src/ObjectCommand.h
SOURCES += ../../src/ObjectCommand.cpp
SOURCES += ../../src/InputObjectWidget.cpp
HEADERS += ../../src/InputObjectWidget.h


HOME=$$system(echo $HOME) 
target.path =  $${HOME}/OTA/lib
INSTALLS += target

QT += core
QT += gui
