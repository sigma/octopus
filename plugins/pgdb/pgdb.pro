# File generated by kdevelop's qmake manager.
# -------------------------------------------
# Subdir relative project main directory: ./plugins/pgdb
# Target is a library:

SOURCES += pgdb.cpp
HEADERS += pgdb.h
TEMPLATE = lib
CONFIG += release \
          warn_on \
          qt \
          thread \
          plugin
DESTDIR = ../../datas/plugins
OBJECTS_DIR = .obj
MOC_DIR = .moc
INCLUDEPATH = ../../interfaces
LIBS += ../../interfaces/libinterfaces.a
TARGETDEPS += ../../interfaces/libinterfaces.a
QT -= gui

debug {
	DEFINES += OCT_DEBUG
}
