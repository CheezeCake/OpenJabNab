TEMPLATE = app
QT_CONFIG -= no-pkg-config
CONFIG += qt release console link_pkgconfig
CONFIG -= debug
PKGCONFIG += pocketsphinx sphinxbase
QT += network
QT -= gui
TARGET = openjabnab
DESTDIR = ../bin/
DEPENDPATH += . ../lib/
INCLUDEPATH += ../lib/
LIBS += -L../bin/ -lcommon
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
HEADERS += openjabnab.h
SOURCES += main.cpp openjabnab.cpp
