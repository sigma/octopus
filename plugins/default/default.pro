# File generated by kdevelop's qmake manager.
# -------------------------------------------
# Subdir relative project main directory: ./plugins/default
# Target is a library:

SOURCES += default.cpp \
           channel.cpp
HEADERS += default.h \
           channel.h
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