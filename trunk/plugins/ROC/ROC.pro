TEMPLATE = lib
CONFIG += plugin
MOC_DIR += build
OBJECTS_DIR += build
INCLUDEPATH += ../../src


HEADERS += ROC.h
SOURCES += ROC.cpp
HEADERS += ROCObject.h
SOURCES += ROCObject.cpp
HEADERS += ROCDialog.h
SOURCES += ROCDialog.cpp
HEADERS += ROCType.h
SOURCES += ROCType.cpp
HEADERS += ../../src/Object.h
SOURCES += ../../src/Object.cpp
HEADERS += ../../src/Util.h
SOURCES += ../../src/Util.cpp
HEADERS += ../../src/Plugin.h
HEADERS += ../../src/PluginCommand.h
HEADERS += ../../src/ObjectCommand.h
SOURCES += ../../src/ObjectCommand.cpp
SOURCES += ../../src/Dialog.cpp
HEADERS += ../../src/Dialog.h
SOURCES += ../../src/InputObjectWidget.cpp
HEADERS += ../../src/InputObjectWidget.h


HOME=$$system(echo $HOME) 
target.path =  $${HOME}/OTA/lib
INSTALLS += target

QT += core
QT += gui
