# File generated by kdevelop's qmake manager.
# -------------------------------------------
# Subdir relative project main directory: ./plugins/tell
# Target is a library:

SOURCES += tell.cpp
HEADERS += tell.h
TEMPLATE = lib
CONFIG += release \
          warn_on \
          qt \
          thread \
          plugin
DESTDIR = ../../datas/plugins/
OBJECTS_DIR = .obj
MOC_DIR = .moc
INCLUDEPATH = ../../interfaces
LIBS += ../../interfaces/libinterfaces.a
TARGETDEPS += ../../interfaces/libinterfaces.a
QT -= gui