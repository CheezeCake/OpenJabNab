TEMPLATE = lib
CONFIG -= debug
CONFIG += plugin qt release link_pkgconfig
QT_CONFIG -= no-pkg-config
PKGCONFIG += pocketsphinx sphinxbase
QT += network
QT -= gui
INCLUDEPATH += . ../../server ../../lib
TARGET = plugin_shutdown
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
HEADERS += plugin_shutdown.h
SOURCES += plugin_shutdown.cpp
