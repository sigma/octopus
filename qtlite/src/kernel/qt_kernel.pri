# Qt kernel module

kernel {
	KERNEL_P	= kernel
	HEADERS += $$KERNEL_P/qucomextra_p.h \
		  $$KERNEL_H/qapplication.h \
		  $$KERNEL_P/qapplication_p.h \
		  $$KERNEL_P/qcomplextext_p.h \
		  $$KERNEL_H/qconnection.h \
		  $$KERNEL_H/qevent.h \
		  $$KERNEL_H/qeventloop.h\
		  $$KERNEL_P/qeventloop_p.h \
		  $$KERNEL_H/qguardedptr.h \
		  $$KERNEL_P/qinputcontext_p.h \
		  $$KERNEL_H/qkeycode.h \
		  $$KERNEL_H/qtranslator.h \
		  $$KERNEL_H/qmetaobject.h \
		  $$KERNEL_H/qnamespace.h \
		  $$KERNEL_H/qnetworkprotocol.h \
		  $$KERNEL_H/qobject.h \
 		  $$KERNEL_H/qobjectcleanuphandler.h \
		  $$KERNEL_H/qobjectdefs.h \
		  $$KERNEL_H/qobjectdict.h \
		  $$KERNEL_H/qobjectlist.h \
		  $$KERNEL_H/qprocess.h \
		  $$KERNEL_H/qsignal.h \
		  $$KERNEL_H/qsignalmapper.h \
		  $$KERNEL_H/qsignalslotimp.h \
		  $$KERNEL_H/qsocketnotifier.h \
		  $$KERNEL_H/qthread.h \
		  $$KERNEL_H/qtimer.h \
		  $$KERNEL_H/qurl.h \
		  $$KERNEL_H/qlocalfs.h \
		  $$KERNEL_H/qurloperator.h \
		  $$KERNEL_H/qurlinfo.h \
		  $$KERNEL_H/qvariant.h \
		  $$KERNEL_P/qinternal_p.h \
		  $$KERNEL_H/qgplugin.h

	win32:SOURCES += $$KERNEL_CPP/qapplication_win.cpp \
		  $$KERNEL_CPP/qclipboard_win.cpp \
		  $$KERNEL_CPP/qcolor_win.cpp \
		  $$KERNEL_CPP/qcursor_win.cpp \
		  $$KERNEL_CPP/qdesktopwidget_win.cpp \
		  $$KERNEL_CPP/qdnd_win.cpp \
		  $$KERNEL_CPP/qeventloop_win.cpp \
		  $$KERNEL_CPP/qfont_win.cpp \
		  $$KERNEL_CPP/qinputcontext_win.cpp \
		  $$KERNEL_CPP/qpixmap_win.cpp \
		  $$KERNEL_CPP/qprocess_win.cpp \
		  $$KERNEL_CPP/qpaintdevice_win.cpp \
		  $$KERNEL_CPP/qpainter_win.cpp \
		  $$KERNEL_CPP/qregion_win.cpp \
		  $$KERNEL_CPP/qsound_win.cpp \
		  $$KERNEL_CPP/qthread_win.cpp \
		  $$KERNEL_CPP/qwidget_win.cpp \
		  $$KERNEL_CPP/qole_win.c \
		  $$KERNEL_CPP/qfontengine_win.cpp

	wince-* {
		SOURCES -= $$KERNEL_CPP/qfontengine_win.cpp
		SOURCES += $$KERNEL_CPP/qfontengine_wce.cpp
		}

	!x11:mac {
	    exists(qsound_mac.cpp):SOURCES += $$KERNEL_CPP/qsound_mac.cpp
	    else:SOURCES += $$KERNEL_CPP/qsound_qws.cpp
	}
        !embedded:!x11:mac {
	      SOURCES += $$KERNEL_CPP/qapplication_mac.cpp \
		          $$KERNEL_CPP/qclipboard_mac.cpp \
			  $$KERNEL_CPP/qcolor_mac.cpp \
			  $$KERNEL_CPP/qcursor_mac.cpp \
			  $$KERNEL_CPP/qdnd_mac.cpp \
			  $$KERNEL_CPP/qdesktopwidget_mac.cpp \
			  $$KERNEL_CPP/qpixmap_mac.cpp \
			  $$KERNEL_CPP/qpaintdevice_mac.cpp \
			  $$KERNEL_CPP/qpainter_mac.cpp \
			  $$KERNEL_CPP/qregion_mac.cpp \
			  $$KERNEL_CPP/qwidget_mac.cpp \
			  $$KERNEL_CPP/qeventloop_mac.cpp \
			  $$KERNEL_CPP/qfont_mac.cpp \
			  $$KERNEL_CPP/qfontengine_mac.cpp
             DEFINES += QMAC_ONE_PIXEL_LOCK
        }
	   
           SOURCES += $$KERNEL_CPP/qeventloop_unix.cpp
        
	SOURCES += $$KERNEL_CPP/qprocess_unix.cpp \
		        $$KERNEL_CPP/qthread_unix.cpp

	SOURCES += $$KERNEL_CPP/qucomextra.cpp \
		  $$KERNEL_CPP/qapplication.cpp \
                  $$KERNEL_CPP/qapplication_x11.cpp \
		  $$KERNEL_CPP/qcomplextext.cpp \
		  $$KERNEL_CPP/qconnection.cpp \
		  $$KERNEL_CPP/qevent.cpp \
		  $$KERNEL_CPP/qeventloop.cpp \
		  $$KERNEL_CPP/qeventloop_x11.cpp \
		  $$KERNEL_CPP/qguardedptr.cpp \
		  $$KERNEL_CPP/qtranslator.cpp \
		  $$KERNEL_CPP/qmetaobject.cpp \
		  $$KERNEL_CPP/qnetworkprotocol.cpp \
		  $$KERNEL_CPP/qobject.cpp \
		  $$KERNEL_CPP/qobjectcleanuphandler.cpp \
		  $$KERNEL_CPP/qprocess.cpp \
		  $$KERNEL_CPP/qsignal.cpp \
		  $$KERNEL_CPP/qsignalmapper.cpp \
		  $$KERNEL_CPP/qsocketnotifier.cpp \
		  $$KERNEL_CPP/qthread.cpp \
		  $$KERNEL_CPP/qtimer.cpp \
		  $$KERNEL_CPP/qurl.cpp \
		  $$KERNEL_CPP/qlocalfs.cpp \
		  $$KERNEL_CPP/qurloperator.cpp \
		  $$KERNEL_CPP/qurlinfo.cpp \
		  $$KERNEL_CPP/qvariant.cpp \
		  $$KERNEL_CPP/qinternal.cpp \
		  $$KERNEL_CPP/qgplugin.cpp

	embedded:SOURCES += $$KERNEL_CPP/qsharedmemory_p.cpp \
		  	    $$KERNEL_CPP/qfontengine_qws.cpp

	wince-* {
	      HEADERS += $$KERNEL_H/qfunctions_wce.h
	      SOURCES += $$KERNEL_CPP/qfunctions_wce.cpp
	}

	accessibility {
	      HEADERS += $$KERNEL_H/qaccessible.h
	      SOURCES += $$KERNEL_CPP/qaccessible.cpp

	      win32:SOURCES += $$KERNEL_CPP/qaccessible_win.cpp
	}
}
