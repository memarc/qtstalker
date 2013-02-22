TEMPLATE = lib
CONFIG += plugin
INCLUDEPATH += ../../lib
LIBS += -L../../lib -lOTA
MOC_DIR += build
OBJECTS_DIR += build

HEADERS += CommandParse.h
SOURCES += CommandParse.cpp
HEADERS += Script.h
SOURCES += Script.cpp
HEADERS += ScriptType.h
SOURCES += ScriptType.cpp
HEADERS += ScriptDialog.h
SOURCES += ScriptDialog.cpp

target.path = /usr/local/lib/OTA/plugins
INSTALLS += target

QT += core
QT += gui
QT += sql
QT += network
