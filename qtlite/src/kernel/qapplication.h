/****************************************************************************
** $Id: qapplication.h,v 1.1 2003/11/04 10:14:21 Sigma Exp $
**
** Definition of QApplication class
**
** Created : 931107
**
** Copyright (C) 1992-2002 Trolltech AS.  All rights reserved.
**
** This file is part of the kernel module of the Qt GUI Toolkit.
**
** This file may be distributed under the terms of the Q Public License
** as defined by Trolltech AS of Norway and appearing in the file
** LICENSE.QPL included in the packaging of this file.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** Licensees holding valid Qt Enterprise Edition or Qt Professional Edition
** licenses may use this file in accordance with the Qt Commercial License
** Agreement provided with the Software.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/pricing.html or email sales@trolltech.com for
**   information about Qt Commercial License Agreements.
** See http://www.trolltech.com/qpl/ for QPL licensing information.
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#ifndef QAPPLICATION_H
#define QAPPLICATION_H

#ifndef QT_H
#include "qasciidict.h"
#include "qtranslator.h"
#include "qstrlist.h"
#include "qstringlist.h"
#endif // QT_H

class QTranslator;
class QEventLoop;

#ifdef QT_THREAD_SUPPORT
class QMutex;
#endif // QT_THREAD_SUPPORT


class QApplication;
extern Q_EXPORT QApplication *qApp;		// global application object


class Q_EXPORT QApplication : public QObject
{
    Q_OBJECT
public:
    QApplication( int &argc, char **argv );
    QApplication( int &argc, char **argv, bool GUIenabled );
    enum Type { Tty, GuiClient, GuiServer };
    QApplication( int &argc, char **argv, Type );
    virtual ~QApplication();

    int		    argc()	const;
    char	  **argv()	const;

    Type type() const;


    static QEventLoop *eventLoop();

    int		     exec();
    void	     processEvents();
    void	     processEvents( int maxtime );
    void	     processOneEvent();
    bool	     hasPendingEvents();
    int		     enter_loop();
    void	     exit_loop();
    int		     loopLevel() const;
    static void	     exit( int retcode=0 );

    static bool	     sendEvent( QObject *receiver, QEvent *event );
    static void	     postEvent( QObject *receiver, QEvent *event );
    static void	     sendPostedEvents( QObject *receiver, int event_type );
    static void	     sendPostedEvents();

    static void      removePostedEvents( QObject *receiver );

    virtual bool     notify( QObject *, QEvent * );

    static bool	     startingUp();
    static bool	     closingDown();

    static void flush();
    static void	     syncX();

    static void	     beep();

#ifndef QT_NO_TRANSLATION
# ifndef QT_NO_TEXTCODEC
    void	     setDefaultCodec( QTextCodec * );
    QTextCodec*	     defaultCodec() const;
# endif
    void	     installTranslator( QTranslator * );
    void	     removeTranslator( QTranslator * );
#endif
    enum Encoding { DefaultCodec, UnicodeUTF8 };
    QString	     translate( const char * context,
				const char * key,
				const char * comment = 0,
				Encoding encoding = DefaultCodec ) const;
#ifndef QT_NO_DIR
    QString   applicationDirPath();
    QString   applicationFilePath();
#endif

#ifndef QT_NO_COMPONENT
    static void      setLibraryPaths( const QStringList & );
    static QStringList libraryPaths();
    static void      addLibraryPath( const QString & );
    static void      removeLibraryPath( const QString & );
#endif // QT_NO_COMPONENT
    static void setStartDragTime( int ms );
    static int startDragTime();
    static void setStartDragDistance( int l );
    static int startDragDistance();

    static bool reverseLayout();

    static int horizontalAlignment( int align );

    static bool	    isEffectEnabled( Qt::UIEffect );
    static void	    setEffectEnabled( Qt::UIEffect, bool enable = TRUE );

#if defined(Q_WS_MAC)
    virtual bool     macEventFilter( EventHandlerCallRef, EventRef );
#endif
#if defined(Q_WS_WIN)
    virtual bool     winEventFilter( MSG * );
#endif

#if defined(Q_OS_WIN32) || defined(Q_OS_CYGWIN)
    static WindowsVersion winVersion();
#elif defined(Q_OS_MAC)
    static MacintoshVersion macVersion();
#endif
#if defined(Q_WS_WIN)
    void	     winFocus( QWidget *, bool );
    static void	     winMouseButtonUp();
#endif

#if defined(QT_THREAD_SUPPORT)
    void	     lock();
    void	     unlock(bool wakeUpGui = TRUE);
    bool	     locked();
    bool             tryLock();
#endif

signals:
    void	     lastWindowClosed();
    void	     aboutToQuit();
    void	     guiThreadAwake();

public slots:
    void	     quit();

#if defined(Q_WS_QWS)
protected:
    void setArgs(int, char **);
#endif

protected:
    bool event(QEvent *);

private:
    void construct( int &argc, char **argv, Type );
    void initialize( int, char ** );
    void init_precmdline();
    bool internalNotify( QObject *, QEvent * );
#if defined(Q_WS_QWS)
    static QWidget *findChildWidget( const QWidget *p, const QPoint &pos );
    static QWidget *findWidget( const QObjectList&, const QPoint &, bool rec );
    static void updateMouseState( QWSMouseEvent * );
#endif

#if defined(Q_WS_MAC)
    bool do_mouse_down(Point *, bool *);
    static QMAC_PASCAL OSStatus globalEventProcessor(EventHandlerCallRef,  EventRef, void *);
    static QMAC_PASCAL void qt_context_timer_callbk(EventLoopTimerRef, void *);
    static QMAC_PASCAL void qt_select_timer_callbk(EventLoopTimerRef, void *);
    static bool qt_mac_apply_settings();
    friend class QMacInputMethod;
    friend QMAC_PASCAL OSStatus qt_window_event(EventHandlerCallRef, EventRef, void *);
    friend void qt_mac_update_os_settings();
    friend bool qt_set_socket_handler( int, int, QObject *, bool);
    friend void qt_mac_destroy_widget(QWidget *);
    friend void qt_init(int *, char **, QApplication::Type);
#endif

#ifdef QT_THREAD_SUPPORT
    static QMutex   *qt_mutex;
#endif // QT_THREAD_SUPPORT

    int		     app_argc;
    char	   **app_argv;
    bool	     quit_now;
    int		     quit_code;
    static QEventLoop* eventloop;
    static int	     app_tracking;
    static bool	     is_app_running;
    static bool	     is_app_closing;
    static bool	     app_exit_loop;
    static int	     loop_level;
    static int	     composedUnicode; // Value, meta-composed character

    static bool	     metaComposeUnicode;

    QValueList<QTranslator*> *translators;
#ifndef QT_NO_COMPONENT
    static QStringList *app_libpaths;
#endif

    static bool      sendSpontaneousEvent( QObject *receiver, QEvent *event );
    static void      removePostedEvent( QEvent * );

    friend class QEvent;
    friend class QTranslator;
    friend class QEventLoop;
    friend Q_EXPORT void qt_ucm_initialize( QApplication * );
#if defined(Q_WS_WIN)
    friend bool qt_sendSpontaneousEvent( QObject*, QEvent* );
#endif
#if defined(Q_WS_QWS)
    friend class QInputContext;
#endif
private: // Disabled copy constructor and operator=
#if defined(Q_DISABLE_COPY)
    QApplication( const QApplication & );
    QApplication &operator=( const QApplication & );
#endif
};

inline int QApplication::argc() const
{
    return app_argc;
}

inline char **QApplication::argv() const
{
    return app_argv;
}

#if defined(Q_WS_QWS)
inline void QApplication::setArgs(int c, char **v)
{
        app_argc = c;
	    app_argv = v;
}
#endif


inline bool QApplication::sendEvent( QObject *receiver, QEvent *event )
{  if ( event ) event->spont = FALSE; return qApp ? qApp->notify( receiver, event ) : FALSE; }

inline bool QApplication::sendSpontaneousEvent( QObject *receiver, QEvent *event )
{ if ( event ) event->spont = TRUE; return qApp ? qApp->notify( receiver, event ) : FALSE; }

#ifdef QT_NO_TRANSLATION
// Simple versions
inline QString QApplication::translate( const char *, const char *sourceText,
					const char *, Encoding encoding ) const
{
#ifndef QT_NO_TEXTCODEC
    if ( encoding == UnicodeUTF8 )
	return QString::fromUtf8( sourceText );
    else
#endif
	return QString::fromLatin1( sourceText );
}
#endif

inline int QApplication::horizontalAlignment( int align )
{
    align &= AlignHorizontal_Mask;
    if ( align == AlignAuto ) {
	if ( reverseLayout() )
	    align = AlignRight;
	else
	    align = AlignLeft;
    }
    return align;
}

#endif // QAPPLICATION_H

