# File generated by kdevelop's qmake manager.
# -------------------------------------------
# Subdir relative project main directory: ./src
# Target is an application:  ../bin/qoctopus

SOURCES += Main.cpp \
           PluginManager.cpp
HEADERS += PluginManager.h
TEMPLATE = app
CONFIG += release \
          warn_on \
          qt \
          thread
TARGET = ../bin/qoctopus
QMAKE_CXXFLAGS_DEBUG += -g3 \
			-gdwarf-2 \
                        -Wall \
                        -Werror \
                        -Wno-long-long \
                        -ansi
QMAKE_CXXFLAGS_RELEASE += -g3 \
			  -ggdb \
			  -Wall \
                          -Werror \
                          -Wno-long-long \
                          -ansi
OBJECTS_DIR = .obj
MOC_DIR = .moc
INCLUDEPATH = ../interfaces
LIBS += ../interfaces/libinterfaces.a
TARGETDEPS += ../interfaces/libinterfaces.a
QT -= gui

debug {
	DEFINES += OCT_DEBUG
}
