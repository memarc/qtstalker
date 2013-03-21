TEMPLATE = lib
CONFIG += plugin
MOC_DIR += build
OBJECTS_DIR += build
INCLUDEPATH += ../../src


HEADERS += MA.h
SOURCES += MA.cpp
HEADERS += MAObject.h
SOURCES += MAObject.cpp
HEADERS += MADialog.h
SOURCES += MADialog.cpp
HEADERS += ../../src/Object.h
HEADERS += ../../src/Util.h
HEADERS += ../../src/Plugin.h
HEADERS += ../../src/PluginCommand.h
HEADERS += ../../src/ObjectCommand.h
HEADERS += ../../src/Dialog.h

OBJECTS += ../../src/build/Dialog.o
OBJECTS += ../../src/build/Object.o
OBJECTS += ../../src/build/ObjectCommand.o
OBJECTS += ../../src/build/Util.o



#INCLUDEPATH += /usr/local/include/gtk-1.2
#INCLUDEPATH += /usr/local/include/glib-1.2
#INCLUDEPATH += /usr/local/lib/glib/include

#LIBS += -L/usr/local/lib -lgtk -lgdk -rdynamic -lgmodule -lglib -ldl -lXext -lX11 -lm



HOME=$$system(echo $HOME) 
target.path =  $${HOME}/OTA/lib
INSTALLS += target

QT += core
QT += gui
