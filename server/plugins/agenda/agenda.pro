TEMPLATE = lib
CONFIG -= debug
CONFIG += plugin qt release link_pkgconfig
QT_CONFIG -= no-pkg-config
PKGCONFIG += pocketsphinx sphinxbase sqlite3
QT += network
QT -= gui
INCLUDEPATH += . ../../server ../../lib
TARGET = plugin_agenda
DESTDIR = ../../bin/plugins
DEPENDPATH += . ../../server ../../lib
LIBS += -L../../bin/ -lcommon
MOC_DIR = ./tmp/moc
OBJECTS_DIR = ./tmp/obj
win32 {
	QMAKE_CXXFLAGS_WARN_ON += -WX
}
unix {
	QMAKE_LFLAGS += -Wl,-rpath,\'\$$ORIGIN\'
	QMAKE_CXXFLAGS += -Werror
}

# Input
HEADERS += plugin_agenda.h
SOURCES += plugin_agenda.cpp
