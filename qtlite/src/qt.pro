# Qt project file
TEMPLATE	= lib
TARGET		= qt
VERSION		= 3.2.3
DESTDIR		= $$QMAKE_LIBDIR_QT
DLLDESTDIR	= ../bin

CONFIG		+= qt warn_on depend_includepath
CONFIG          += qmake_cache

unix:CONFIG     += create_libtool create_pc

win32:!shared:CONFIG += staticlib

win32-borland {
	mng:QMAKE_CFLAGS_WARN_ON	+= -w-par
	mng:QMAKE_CXXFLAGS_WARN_ON	+= -w-par
	# Keep the size of the .tds file for the Qt library smaller than
	# 34 Mbytes to avoid linking problems
	QMAKE_CFLAGS_DEBUG += -vi -y-
	QMAKE_CXXFLAGS_DEBUG += -vi -y-
}

linux-*:version_script {
   QMAKE_LFLAGS += -Wl,--version-script=libqt.map
   TARGETDEPS += libqt.map
}

KERNEL_CPP	= kernel
NETWORK_CPP	= network
TOOLS_CPP	= tools
CODECS_CPP	= codecs
XML_CPP	        = xml

win32 {
	contains(QT_PRODUCT,qt-internal) {
		KERNEL_H	= $$KERNEL_CPP
		NETWORK_H	= $$NETWORK_CPP
		TOOLS_H		= $$TOOLS_CPP
		CODECS_H	= $$CODECS_CPP
		XML_H		= $$XML_CPP
	} else {
		WIN_ALL_H = ../include
		KERNEL_H	= $$WIN_ALL_H
		NETWORK_H	= $$WIN_ALL_H
		TOOLS_H		= $$WIN_ALL_H
		CODECS_H	= $$WIN_ALL_H
		XML_H		= $$WIN_ALL_H
		CONFIG 		-= incremental
	}

	CONFIG	+= zlib
	INCLUDEPATH += tmp
	!staticlib {
	    DEFINES+=QT_MAKEDLL
	    exists(qt.rc):RC_FILE = qt.rc
	}
}
win32-borland:INCLUDEPATH += kernel

unix {
	KERNEL_H	= $$KERNEL_CPP
	NETWORK_H	= $$NETWORK_CPP
	TOOLS_H		= $$TOOLS_CPP
	CODECS_H	= $$CODECS_CPP
	XML_H		= $$XML_CPP
}

aix-g++ {
	QMAKE_CFLAGS   += -mminimal-toc
	QMAKE_CXXFLAGS += -mminimal-toc
}


DEPENDPATH += ;$$NETWORK_H;$$KERNEL_H;
DEPENDPATH += $$TOOLS_H;$$CODECS_H;$$XML_H;

thread {
	!win32-borland:TARGET = qt-mt
	win32-borland:TARGET = qtmt
	DEFINES += QT_THREAD_SUPPORT
}

!cups:DEFINES += QT_NO_CUPS

!nis:DEFINES += QT_NO_NIS

largefile {
	unix:!darwin:DEFINES += _LARGEFILE_SOURCE _FILE_OFFSET_BITS=64
}

#here for compatability, should go away ####
include($$KERNEL_CPP/qt_compat.pri)

#platforms
mac:include($$KERNEL_CPP/qt_mac.pri)

#modules
include($$KERNEL_CPP/qt_kernel.pri)
include($$NETWORK_CPP/qt_network.pri)
include($$XML_CPP/qt_xml.pri)
include($$TOOLS_CPP/qt_tools.pri)
include($$CODECS_CPP/qt_codecs.pri)

# qconfig.cpp
exists($$QT_BUILD_TREE/src/tools/qconfig.cpp) {
    SOURCES += $$QT_BUILD_TREE/src/tools/qconfig.cpp
}

#install directives
include(qt_install.pri)
!staticlib:PRL_EXPORT_DEFINES += QT_SHARED
