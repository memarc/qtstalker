TEMPLATE = lib
CONFIG += plugin
MOC_DIR += build
OBJECTS_DIR += build
INCLUDEPATH += ../../src


HEADERS += MarkerVLine.h
SOURCES += MarkerVLine.cpp
HEADERS += MarkerVLineDialog.h
SOURCES += MarkerVLineDialog.cpp
HEADERS += MarkerVLineObject.h
SOURCES += MarkerVLineObject.cpp
HEADERS += ../../src/ColorButton.h
SOURCES += ../../src/ColorButton.cpp
HEADERS += ../../src/Object.h
SOURCES += ../../src/Object.cpp
HEADERS += ../../src/Util.h
SOURCES += ../../src/Util.cpp
HEADERS += ../../src/Plugin.h
HEADERS += ../../src/PluginCommand.h
HEADERS += ../../src/ObjectCommand.h
SOURCES += ../../src/ObjectCommand.cpp
HEADERS += ../../src/Dialog.h
SOURCES += ../../src/Dialog.cpp


HOME=$$system(echo $HOME) 
target.path =  $${HOME}/OTA/lib
INSTALLS += target

QT += core
QT += gui
