/****************************************************************************
** $Id: qapplication.cpp,v 1.1 2003/11/04 10:14:21 Sigma Exp $
**
** Implementation of QApplication class
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
#include "qthread.h"
#include "qobjectlist.h"
#include "qapplication.h"
#include "qeventloop.h"
#include "qeventloop_p.h"
#include "qptrdict.h"
#include "qcleanuphandler.h"
#include "qwindowdefs.h"
#include "qtranslator.h"
#include "qtextcodec.h"
#include "qfile.h"
#include "qdir.h"
#include "qfileinfo.h"
#ifdef Q_WS_WIN
#include "qinputcontext_p.h"
#endif

#if defined(QT_THREAD_SUPPORT)
#  include "qmutex.h"
#endif // QT_THREAD_SUPPORT

#include <stdlib.h>

/*!
  \class QApplication qapplication.h
  \brief The QApplication class manages the GUI application's control
  flow and main settings.

  \ingroup application
  \mainclass

  It contains the main event loop, where all events from the window
  system and other sources are processed and dispatched. It also
  handles the application's initialization and finalization, and
  provides session management. It also handles most system-wide and
  application-wide settings.

  For any GUI application that uses Qt, there is precisely one
  QApplication object, no matter whether the application has 0, 1, 2
  or more windows at any time.

  The QApplication object is accessible through the global pointer \c
  qApp. Its main areas of responsibility are:
  \list

  \i It initializes the application with the user's desktop settings
  such as palette(), font() and doubleClickInterval(). It keeps track
  of these properties in case the user changes the desktop globally, for
  example through some kind of control panel.

  \i It performs event handling, meaning that it receives events
  from the underlying window system and dispatches them to the relevant
  widgets. By using sendEvent() and postEvent() you can send your own
  events to widgets.

  \i It parses common command line arguments and sets its internal
  state accordingly. See the \link QApplication::QApplication()
  constructor documentation\endlink below for more details about this.

  \i It defines the application's look and feel, which is
  encapsulated in a QStyle object. This can be changed at runtime
  with setStyle().

  \i It specifies how the application is to allocate colors.
  See setColorSpec() for details.

  \i It provides localization of strings that are visible to the user
  via translate().

  \i It provides some magical objects like the desktop() and the
  clipboard().

  \i It knows about the application's windows. You can ask which
  widget is at a certain position using widgetAt(), get a list of
  topLevelWidgets() and closeAllWindows(), etc.

  \i It manages the application's mouse cursor handling,
  see setOverrideCursor() and setGlobalMouseTracking().

  \i On the X window system, it provides functions to flush and sync
  the communication stream, see flushX() and syncX().

  \i It provides support for sophisticated \link
  session.html session management \endlink. This makes it possible
  for applications to terminate gracefully when the user logs out, to
  cancel a shutdown process if termination isn't possible and even to
  preserve the entire application's state for a future session. See
  isSessionRestored(), sessionId() and commitData() and saveState()
  for details.

  \endlist

  The <a href="simple-application.html">Application walk-through
  example</a> contains a typical complete main() that does the usual
  things with QApplication.

  Since the QApplication object does so much initialization, it
  <b>must</b> be created before any other objects related to the user
  interface are created.

  Since it also deals with common command line arguments, it is
  usually a good idea to create it \e before any interpretation or
  modification of \c argv is done in the application itself. (Note
  also that for X11, setMainWidget() may change the main widget
  according to the \c -geometry option. To preserve this
  functionality, you must set your defaults before setMainWidget() and
  any overrides after.)

  \table
    \header \i21 Groups of functions
    \row
     \i System settings
     \i
	desktopSettingsAware(),
	setDesktopSettingsAware(),
	cursorFlashTime(),
	setCursorFlashTime(),
	doubleClickInterval(),
	setDoubleClickInterval(),
	wheelScrollLines(),
	setWheelScrollLines(),
	palette(),
	setPalette(),
	font(),
	setFont(),
	fontMetrics().

    \row
     \i Event handling
     \i
	exec(),
	processEvents(),
	enter_loop(),
	exit_loop(),
	exit(),
	quit().
	sendEvent(),
	postEvent(),
	sendPostedEvents(),
	removePostedEvents(),
	hasPendingEvents(),
	notify(),
	macEventFilter(),
	qwsEventFilter(),
	x11EventFilter(),
	x11ProcessEvent(),
	winEventFilter().

    \row
     \i GUI Styles
     \i
	style(),
	setStyle(),
	polish().

    \row
     \i Color usage
     \i
	colorSpec(),
	setColorSpec(),
	qwsSetCustomColors().

    \row
     \i Text handling
     \i
	installTranslator(),
	removeTranslator()
	translate().

    \row
     \i Widgets
     \i
	mainWidget(),
	setMainWidget(),
	allWidgets(),
	topLevelWidgets(),
	desktop(),
	activePopupWidget(),
	activeModalWidget(),
	clipboard(),
	focusWidget(),
	winFocus(),
	activeWindow(),
	widgetAt().

    \row
     \i Advanced cursor handling
     \i
	hasGlobalMouseTracking(),
	setGlobalMouseTracking(),
	overrideCursor(),
	setOverrideCursor(),
	restoreOverrideCursor().

    \row
     \i X Window System synchronization
     \i
	flushX(),
	syncX().

    \row
     \i Session management
     \i
	isSessionRestored(),
	sessionId(),
	commitData(),
	saveState().

    \row
    \i Threading
    \i
	lock(), unlock(), locked(), tryLock(),
	wakeUpGuiThread()

    \row
     \i Miscellaneous
     \i
	closeAllWindows(),
	startingUp(),
	closingDown(),
	type().
  \endtable

  \e {Non-GUI programs:} While Qt is not optimized or
  designed for writing non-GUI programs, it's possible to use
  \link tools.html some of its classes \endlink without creating a
  QApplication. This can be useful if you wish to share code between
  a non-GUI server and a GUI client.

  \headerfile qnamespace.h
  \headerfile qwindowdefs.h
  \headerfile qglobal.h
*/

/*! \enum Qt::HANDLE
    \internal
*/

/*!
    \enum QApplication::Type

    \value Tty a console application
    \value GuiClient a GUI client application
    \value GuiServer a GUI server application
*/

/*!
    \enum QApplication::ColorSpec

    \value NormalColor the default color allocation policy
    \value CustomColor the same as NormalColor for X11; allocates colors
    to a palette on demand under Windows
    \value ManyColor the right choice for applications that use thousands of
    colors

    See setColorSpec() for full details.
*/

/*
  The qt_init() and qt_cleanup() functions are implemented in the
  qapplication_xyz.cpp file.
*/

void qt_init( int *, char **, QApplication::Type );
void qt_cleanup();

QApplication *qApp = 0;			// global application object

int	  QApplication::app_tracking   = 0;	// global mouse tracking
bool	  QApplication::is_app_running = FALSE;	// app starting up if FALSE
bool	  QApplication::is_app_closing = FALSE;	// app closing down if TRUE
int	  QApplication::loop_level     = 0;	// event loop level
bool      Q_EXPORT qt_resolve_symlinks = TRUE;
QApplication::Type qt_appType=QApplication::Tty;
#ifndef QT_NO_COMPONENT
QStringList *QApplication::app_libpaths = 0;
#endif
bool	  QApplication::metaComposeUnicode = FALSE;
int	  QApplication::composedUnicode   = 0;

#ifdef QT_THREAD_SUPPORT
QMutex *QApplication::qt_mutex		= 0;
static QMutex *postevent_mutex		= 0;
#endif // QT_THREAD_SUPPORT

QEventLoop *QApplication::eventloop = 0;	// application event loop

typedef void (*VFPTR)();
typedef QValueList<VFPTR> QVFuncList;
static QVFuncList *postRList = 0;		// list of post routines

/*!
  \relates QApplication

  Adds a global routine that will be called from the QApplication
  destructor. This function is normally used to add cleanup routines
  for program-wide functionality.

  The function given by \a p should take no arguments and return
  nothing, like this:
  \code
    static int *global_ptr = 0;

    static void cleanup_ptr()
    {
	delete [] global_ptr;
	global_ptr = 0;
    }

    void init_ptr()
    {
	global_ptr = new int[100];	// allocate data
	qAddPostRoutine( cleanup_ptr );	// delete later
    }
  \endcode

  Note that for an application- or module-wide cleanup,
  qAddPostRoutine() is often not suitable. People have a tendency to
  make such modules dynamically loaded, and then unload those modules
  long before the QApplication destructor is called, for example.

  For modules and libraries, using a reference-counted initialization
  manager or Qt' parent-child delete mechanism may be better. Here is
  an example of a private class which uses the parent-child mechanism
  to call a cleanup function at the right time:

  \code
    class MyPrivateInitStuff: public QObject {
    private:
	MyPrivateInitStuff( QObject * parent ): QObject( parent) {
	    // initialization goes here
	}
	MyPrivateInitStuff * p;

    public:
	static MyPrivateInitStuff * initStuff( QObject * parent ) {
	    if ( !p )
		p = new MyPrivateInitStuff( parent );
	    return p;
	}

	~MyPrivateInitStuff() {
	    // cleanup (the "post routine") goes here
	}
    }
  \endcode

  By selecting the right parent widget/object, this can often be made
  to clean up the module's data at the exact right moment.
*/

Q_EXPORT void qAddPostRoutine( QtCleanUpFunction p)
{
    if ( !postRList ) {
	postRList = new QVFuncList;
	Q_CHECK_PTR( postRList );
    }
    postRList->prepend( p );
}


Q_EXPORT void qRemovePostRoutine( QtCleanUpFunction p )
{
    if ( !postRList ) return;
    QVFuncList::Iterator it = postRList->begin();
    while ( it != postRList->end() ) {
	if ( *it == p ) {
	    postRList->remove( it );
	    it = postRList->begin();
	} else {
	    ++it;
	}
    }
}


// Definitions for posted events
struct QPostEvent {
    QPostEvent( QObject *r, QEvent *e ): receiver( r ), event( e ) {}
   ~QPostEvent()			{ delete event; }
    QObject  *receiver;
    QEvent   *event;
};

class Q_EXPORT QPostEventList : public QPtrList<QPostEvent>
{
public:
    QPostEventList() : QPtrList<QPostEvent>() {}
    QPostEventList( const QPostEventList &list ) : QPtrList<QPostEvent>(list) {}
   ~QPostEventList() { clear(); }
    QPostEventList &operator=(const QPostEventList &list)
	{ return (QPostEventList&)QPtrList<QPostEvent>::operator=(list); }
};
class Q_EXPORT QPostEventListIt : public QPtrListIterator<QPostEvent>
{
public:
    QPostEventListIt( const QPostEventList &l ) : QPtrListIterator<QPostEvent>(l) {}
    QPostEventListIt &operator=(const QPostEventListIt &i)
{ return (QPostEventListIt&)QPtrListIterator<QPostEvent>::operator=(i); }
};

static QPostEventList *globalPostedEvents = 0;	// list of posted events

uint qGlobalPostedEventsCount()
{
    if (!globalPostedEvents)
	return 0;
    return globalPostedEvents->count();
}

static QSingleCleanupHandler<QPostEventList> qapp_cleanup_events;


/*!
  Initializes the window system and constructs an application object
  with \a argc command line arguments in \a argv.

  The global \c qApp pointer refers to this application object. Only
  one application object should be created.

  This application object must be constructed before any \link
  QPaintDevice paint devices\endlink (including widgets, pixmaps, bitmaps
  etc.).

  Note that \a argc and \a argv might be changed. Qt removes command
  line arguments that it recognizes. The original \a argc and \a argv
  can be accessed later with \c qApp->argc() and \c qApp->argv().
  The documentation for argv() contains a detailed description of how
  to process command line arguments.

  Qt debugging options (not available if Qt was compiled with the
  QT_NO_DEBUG flag defined):
  \list
  \i -nograb, tells Qt that it must never grab the mouse or the keyboard.
  \i -dograb (only under X11), running under a debugger can cause
  an implicit -nograb, use -dograb to override.
  \i -sync (only under X11), switches to synchronous mode for
	debugging.
  \endlist

  See \link debug.html Debugging Techniques \endlink for a more
  detailed explanation.

  All Qt programs automatically support the following command line options:
  \list
  \i -style= \e style, sets the application GUI style. Possible values
       are \c motif, \c windows, and \c platinum. If you compiled Qt
       with additional styles or have additional styles as plugins these
       will be available to the \c -style command line option.
  \i -style \e style, is the same as listed above.
  \i -session= \e session, restores the application from an earlier
       \link session.html session \endlink.
  \i -session \e session, is the same as listed above.
  \i -widgetcount, prints debug message at the end about number of widgets left
       undestroyed and maximum number of widgets existed at the same time
  \endlist

  The X11 version of Qt also supports some traditional X11
  command line options:
  \list
  \i -display \e display, sets the X display (default is $DISPLAY).
  \i -geometry \e geometry, sets the client geometry of the
	\link setMainWidget() main widget\endlink.
  \i -fn or \c -font \e font, defines the application font. The
  font should be specified using an X logical font description.
  \i -bg or \c -background \e color, sets the default background color
	and an application palette (light and dark shades are calculated).
  \i -fg or \c -foreground \e color, sets the default foreground color.
  \i -btn or \c -button \e color, sets the default button color.
  \i -name \e name, sets the application name.
  \i -title \e title, sets the application title (caption).
  \i -visual \c TrueColor, forces the application to use a TrueColor visual
       on an 8-bit display.
  \i -ncols \e count, limits the number of colors allocated in the
       color cube on an 8-bit display, if the application is using the
       \c QApplication::ManyColor color specification. If \e count is
       216 then a 6x6x6 color cube is used (i.e. 6 levels of red, 6 of green,
       and 6 of blue); for other values, a cube
       approximately proportional to a 2x3x1 cube is used.
  \i -cmap, causes the application to install a private color map
       on an 8-bit display.
  \endlist

  \sa argc(), argv()
*/

//######### BINARY COMPATIBILITY constructor
QApplication::QApplication( int &argc, char **argv )
{
    construct( argc, argv, GuiClient );
}


/*!
  Constructs an application object with \a argc command line arguments
  in \a argv. If \a GUIenabled is TRUE, a GUI application is
  constructed, otherwise a non-GUI (console) application is created.

  Set \a GUIenabled to FALSE for programs without a graphical user
  interface that should be able to run without a window system.

  On X11, the window system is initialized if \a GUIenabled is TRUE.
  If \a GUIenabled is FALSE, the application does not connect to the
  X-server.
  On Windows and Macintosh, currently the window system is always
  initialized, regardless of the value of GUIenabled. This may change in
  future versions of Qt.

  The following example shows how to create an application that
  uses a graphical interface when available.
  \code
  int main( int argc, char **argv )
  {
#ifdef Q_WS_X11
    bool useGUI = getenv( "DISPLAY" ) != 0;
#else
    bool useGUI = TRUE;
#endif
    QApplication app(argc, argv, useGUI);

    if ( useGUI ) {
       //start GUI version
       ...
    } else {
       //start non-GUI version
       ...
    }
    return app.exec();
  }
\endcode
*/

QApplication::QApplication( int &argc, char **argv, bool GUIenabled  )
{
    construct( argc, argv, GUIenabled ? GuiClient : Tty );
}

/*!
  Constructs an application object with \a argc command line arguments
  in \a argv.

  For Qt/Embedded, passing \c QApplication::GuiServer for \a type
  makes this application the server (equivalent to running with the
  -qws option).
*/
QApplication::QApplication( int &argc, char **argv, Type type )
{
    construct( argc, argv, type );
}

Q_EXPORT void qt_ucm_initialize( QApplication *theApp )
{
    if ( qApp )
	return;
    int argc = theApp->argc();
    char **argv = theApp->argv();
    theApp->construct( argc, argv, qApp->type() );

    Q_ASSERT( qApp == theApp );
}

void QApplication::construct( int &argc, char **argv, Type type )
{
    qt_appType = type;
    init_precmdline();
    static const char *empty = "";
    if ( argc == 0 || argv == 0 ) {
	argc = 0;
	argv = (char **)&empty; // ouch! careful with QApplication::argv()!
    }
    qt_init( &argc, argv, type );   // Must be called before initialize()
    initialize( argc, argv );
    if ( eventloop )
	eventloop->appStartingUp();
}

/*!
    Returns the type of application, Tty, GuiClient or GuiServer.
*/

QApplication::Type QApplication::type() const
{
    return qt_appType;
}

void QApplication::init_precmdline()
{
    translators = 0;
    is_app_closing = FALSE;
#if defined(QT_CHECK_STATE)
    if ( qApp )
	qWarning( "QApplication: There should be max one application object" );
#endif
    qApp = (QApplication*)this;
}

/*!
  Initializes the QApplication object, called from the constructors.
*/

void QApplication::initialize( int argc, char **argv )
{
#ifdef QT_THREAD_SUPPORT
    qt_mutex = new QMutex( TRUE );
    postevent_mutex = new QMutex( TRUE );
#endif // QT_THREAD_SUPPORT

    app_argc = argc;
    app_argv = argv;
    quit_now = FALSE;
    quit_code = 0;
    is_app_running = TRUE; // no longer starting up


}

/*!
  Cleans up any window system resources that were allocated by this
  application. Sets the global variable \c qApp to 0.
*/

QApplication::~QApplication()
{

    if ( eventloop )
	eventloop->appClosingDown();
    if ( postRList ) {
	QVFuncList::Iterator it = postRList->begin();
	while ( it != postRList->end() ) {	// call post routines
	    (**it)();
	    postRList->remove( it );
	    it = postRList->begin();
	}
	delete postRList;
	postRList = 0;
    }

    is_app_closing = TRUE;

    qt_cleanup();

#ifndef QT_NO_COMPONENT
    delete app_libpaths;
    app_libpaths = 0;
#endif

#ifdef QT_THREAD_SUPPORT
    delete qt_mutex;
    qt_mutex = 0;
    delete postevent_mutex;
    postevent_mutex = 0;
#endif // QT_THREAD_SUPPORT

    if( qApp == this ) {
	if ( postedEvents )
	    removePostedEvents( this );
	qApp = 0;
    }
    is_app_running = FALSE;
    app_tracking = 0;
}


/*!
    \fn int QApplication::argc() const

    Returns the number of command line arguments.

    The documentation for argv() describes how to process command line
    arguments.

    \sa argv(), QApplication::QApplication()
*/

/*!
    \fn char **QApplication::argv() const

    Returns the command line argument vector.

    \c argv()[0] is the program name, \c argv()[1] is the first
    argument and \c argv()[argc()-1] is the last argument.

    A QApplication object is constructed by passing \e argc and \e
    argv from the \c main() function. Some of the arguments may be
    recognized as Qt options and removed from the argument vector. For
    example, the X11 version of Qt knows about \c -display, \c -font
    and a few more options.

    Example:
    \code
	// showargs.cpp - displays program arguments in a list box

	#include <qapplication.h>
	#include <qlistbox.h>

	int main( int argc, char **argv )
	{
	    QApplication a( argc, argv );
	    QListBox b;
	    a.setMainWidget( &b );
	    for ( int i = 0; i < a.argc(); i++ )  // a.argc() == argc
		b.insertItem( a.argv()[i] );      // a.argv()[i] == argv[i]
	    b.show();
	    return a.exec();
	}
    \endcode

    If you run \c{showargs -display unix:0 -font 9x15bold hello world}
    under X11, the list box contains the three strings "showargs",
    "hello" and "world".

    Qt provides a global pointer, \c qApp, that points to the
    QApplication object, and through which you can access argc() and
    argv() in functions other than main().

    \sa argc(), QApplication::QApplication()
*/

/*!
    \fn void QApplication::setArgs( int argc, char **argv )
    \internal
*/



#ifdef Q_WS_WIN
extern const char *qAppFileName();
#endif

#ifndef QT_NO_DIR
#ifndef Q_WS_WIN
static QString resolveSymlinks( const QString& path, int depth = 0 )
{
    bool foundLink = FALSE;
    QString linkTarget;
    QString part = path;
    int slashPos = path.length();

    // too deep; we give up
    if ( depth == 128 )
	return QString::null;

    do {
	part = part.left( slashPos );
	QFileInfo fileInfo( part );
	if ( fileInfo.isSymLink() ) {
	    foundLink = TRUE;
	    linkTarget = fileInfo.readLink();
	    break;
	}
    } while ( (slashPos = part.findRev('/')) != -1 );

    if ( foundLink ) {
	QString path2;
	if ( linkTarget[0] == '/' ) {
	    path2 = linkTarget;
	    if ( slashPos < (int) path.length() )
		path2 += "/" + path.right( path.length() - slashPos - 1 );
	} else {
	    QString relPath;
	    relPath = part.left( part.findRev('/') + 1 ) + linkTarget;
	    if ( slashPos < (int) path.length() ) {
		if ( !linkTarget.endsWith( "/" ) )
		    relPath += "/";
		relPath += path.right( path.length() - slashPos - 1 );
	    }
	    path2 = QDir::current().absFilePath( relPath );
	}
	path2 = QDir::cleanDirPath( path2 );
	return resolveSymlinks( path2, depth + 1 );
    } else {
	return path;
    }
}
#endif // Q_WS_WIN

/*!
    Returns the directory that contains the application executable.

    For example, if you have installed Qt in the \c{C:\Trolltech\Qt}
    directory, and you run the \c{demo} example, this function will
    return "C:/Trolltech/Qt/examples/demo".

    \warning On Unix and Mac OS X, this function assumes that argv[0]
    contains the file name of the executable (which it normally
    does). It also assumes that the current directory hasn't been
    changed by the application.

    \sa applicationFilePath()
*/
QString QApplication::applicationDirPath()
{
    return QFileInfo( applicationFilePath() ).dirPath();
}

/*!
    Returns the file path of the application executable.

    For example, if you have installed Qt in the \c{C:\Trolltech\Qt}
    directory, and you run the \c{demo} example, this function will
    return "C:/Trolltech/Qt/examples/demo/demo.exe".

    \warning On Unix and Mac OS X, this function assumes that argv[0]
    contains the file name of the executable (which it normally
    does). It also assumes that the current directory hasn't been
    changed by the application.

    \sa applicationDirPath()
*/
QString QApplication::applicationFilePath()
{
#ifdef Q_WS_WIN
    return QDir::cleanDirPath( QFile::decodeName( qAppFileName() ) );
#else
    QString argv0 = QFile::decodeName( argv()[0] );
    QString absPath;

    if ( argv0[0] == '/' ) {
	/*
	  If argv0 starts with a slash, it is already an absolute
	  file path.
	*/
	absPath = argv0;
    } else if ( argv0.find('/') != -1 ) {
	/*
	  If argv0 contains one or more slashes, it is a file path
	  relative to the current directory.
	*/
	absPath = QDir::current().absFilePath( argv0 );
    } else {
	/*
	  Otherwise, the file path has to be determined using the
	  PATH environment variable.
	*/
	char *pEnv = getenv( "PATH" );
	QStringList paths( QStringList::split(QChar(':'), pEnv) );
	QStringList::const_iterator p = paths.begin();
	while ( p != paths.end() ) {
	    QString candidate = QDir::current().absFilePath( *p + "/" + argv0 );
	    if ( QFile::exists(candidate) ) {
		absPath = candidate;
		break;
	    }
	    ++p;
	}
    }

    absPath = QDir::cleanDirPath( absPath );
    if ( QFile::exists(absPath) ) {
	return resolveSymlinks( absPath );
    } else {
	return QString::null;
    }
#endif
}
#endif // QT_NO_DIR

#ifndef QT_NO_COMPONENT

/*!
  Returns a list of paths that the application will search when
  dynamically loading libraries.
  The installation directory for plugins is the only entry if no
  paths have been set.  The default installation directory for plugins
  is \c INSTALL/plugins, where \c INSTALL is the directory where Qt was
  installed. On Windows, the directory of the application executable (NOT the
  working directory) is also added to the plugin paths.

  If you want to iterate over the list, you should iterate over a
  copy, e.g.
    \code
    QStringList list = app.libraryPaths();
    QStringList::Iterator it = list.begin();
    while( it != list.end() ) {
	myProcessing( *it );
	++it;
    }
    \endcode

  See the \link plugins-howto.html plugins documentation\endlink for a
  description of how the library paths are used.

  \sa setLibraryPaths(), addLibraryPath(), removeLibraryPath(), QLibrary
*/
QStringList QApplication::libraryPaths()
{
    if ( !app_libpaths ) {
	app_libpaths = new QStringList;
	if ( QFile::exists( qInstallPathPlugins() ) )
	    app_libpaths->append( qInstallPathPlugins() );
#ifdef Q_WS_WIN
	QString app_location = qAppFileName();
	app_location.truncate( app_location.findRev( '\\' ) );
	if ( app_location != qInstallPathPlugins() && QFile::exists( app_location ) )
	    app_libpaths->append( app_location );
#endif
    }
    return *app_libpaths;
}


/*!
  Sets the list of directories to search when loading libraries to \a paths.
  All existing paths will be deleted and the path list will consist of the
  paths given in \a paths.

  \sa libraryPaths(), addLibraryPath(), removeLibraryPath(), QLibrary
 */
void QApplication::setLibraryPaths( const QStringList &paths )
{
    delete app_libpaths;
    app_libpaths = new QStringList( paths );
}

/*!
  Append \a path to the end of the library path list. If \a path is
  empty or already in the path list, the path list is not changed.

  The default path list consists of a single entry, the installation
  directory for plugins.  The default installation directory for plugins
  is \c INSTALL/plugins, where \c INSTALL is the directory where Qt was
  installed.

  \sa removeLibraryPath(), libraryPaths(), setLibraryPaths()
 */
void QApplication::addLibraryPath( const QString &path )
{
    if ( path.isEmpty() )
	return;

    // make sure that library paths is initialized
    libraryPaths();

    if ( !app_libpaths->contains( path ) )
	app_libpaths->prepend( path );
}

/*!
  Removes \a path from the library path list. If \a path is empty or not
  in the path list, the list is not changed.

  \sa addLibraryPath(), libraryPaths(), setLibraryPaths()
*/
void QApplication::removeLibraryPath( const QString &path )
{
    if ( path.isEmpty() )
	return;

    // make sure that library paths is initialized
    libraryPaths();

    if ( app_libpaths->contains( path ) )
	app_libpaths->remove( path );
}
#endif //QT_NO_COMPONENT


/*!
  Tells the application to exit with return code 0 (success).
  Equivalent to calling QApplication::exit( 0 ).

  It's common to connect the lastWindowClosed() signal to quit(), and
  you also often connect e.g. QButton::clicked() or signals in
  QAction, QPopupMenu or QMenuBar to it.

  Example:
  \code
    QPushButton *quitButton = new QPushButton( "Quit" );
    connect( quitButton, SIGNAL(clicked()), qApp, SLOT(quit()) );
  \endcode

  \sa exit() aboutToQuit() lastWindowClosed() QAction
*/

void QApplication::quit()
{
    QApplication::exit( 0 );
}



/*!
  Sends event \a e to \a receiver: \a {receiver}->event(\a e).
  Returns the value that is returned from the receiver's event handler.

  For certain types of events (e.g. mouse and key events),
  the event will be propagated to the receiver's parent and so on up to
  the top-level object if the receiver is not interested in the event
  (i.e., it returns FALSE).

  There are five different ways that events can be processed;
  reimplementing this virtual function is just one of them. All five
  approaches are listed below:
  \list 1
  \i Reimplementing this function. This is very powerful, providing
  complete control; but only one subclass can be qApp.

  \i Installing an event filter on qApp. Such an event filter is able
  to process all events for all widgets, so it's just as powerful as
  reimplementing notify(); furthermore, it's possible to have more
  than one application-global event filter. Global event filters even
  see mouse events for \link QWidget::isEnabled() disabled
  widgets, \endlink and if \link setGlobalMouseTracking() global mouse
  tracking \endlink is enabled, as well as mouse move events for all
  widgets.

  \i Reimplementing QObject::event() (as QWidget does). If you do
  this you get Tab key presses, and you get to see the events before
  any widget-specific event filters.

  \i Installing an event filter on the object. Such an event filter
  gets all the events except Tab and Shift-Tab key presses.

  \i Reimplementing paintEvent(), mousePressEvent() and so
  on. This is the commonest, easiest and least powerful way.
  \endlist

  \sa QObject::event(), installEventFilter()
*/

bool QApplication::notify( QObject *receiver, QEvent *e )
{
    // no events are delivered after ~QApplication() has started
    if ( is_app_closing )
	return FALSE;

    if ( receiver == 0 ) {			// serious error
#if defined(QT_CHECK_NULL)
	qWarning( "QApplication::notify: Unexpected null receiver" );
#endif
	return FALSE;
    }

    if ( e->type() == QEvent::ChildRemoved && receiver->postedEvents ) {

#ifdef QT_THREAD_SUPPORT
	QMutexLocker locker( postevent_mutex );
#endif // QT_THREAD_SUPPORT

	// the QObject destructor calls QObject::removeChild, which calls
	// QApplication::sendEvent() directly.  this can happen while the event
	// loop is in the middle of posting events, and when we get here, we may
	// not have any more posted events for this object.
	if ( receiver->postedEvents ) {
	    // if this is a child remove event and the child insert
	    // hasn't been dispatched yet, kill that insert
	    QPostEventList * l = receiver->postedEvents;
	    QObject * c = ((QChildEvent*)e)->child();
	    QPostEvent * pe;
	    l->first();
	    while( ( pe = l->current()) != 0 ) {
		if ( pe->event && pe->receiver == receiver &&
		     pe->event->type() == QEvent::ChildInserted &&
		     ((QChildEvent*)pe->event)->child() == c ) {
		    pe->event->posted = FALSE;
		    delete pe->event;
		    pe->event = 0;
		    l->remove();
		    continue;
		}
		l->next();
	    }
	}
    }

    bool res = FALSE;
    if ( !receiver->isWidgetType() )
	res = internalNotify( receiver, e );
    else switch ( e->type() ) {
    default:
	res = internalNotify( receiver, e );
	break;
    }

    return res;
}

/*!\reimp

*/
bool QApplication::event( QEvent *e )
{
    if (e->type() == QEvent::Quit) {
	quit();
	return TRUE;
    }
    return QObject::event(e);
}

/*!\internal

  Helper function called by notify()
 */
bool QApplication::internalNotify( QObject *receiver, QEvent * e)
{
    if ( eventFilters ) {
	QObjectListIt it( *eventFilters );
	register QObject *obj;
	while ( (obj=it.current()) != 0 ) {	// send to all filters
	    ++it;				//   until one returns TRUE
	    if ( obj->eventFilter(receiver,e) )
		return TRUE;
	}
    }

    bool consumed = FALSE;
    bool handled = FALSE;


    if (!handled)
	consumed = receiver->event( e );
    e->spont = FALSE;
    return consumed;
}

/*!
  Returns TRUE if an application object has not been created yet;
  otherwise returns FALSE.

  \sa closingDown()
*/

bool QApplication::startingUp()
{
    return !is_app_running;
}

/*!
  Returns TRUE if the application objects are being destroyed;
  otherwise returns FALSE.

  \sa startingUp()
*/

bool QApplication::closingDown()
{
    return is_app_closing;
}


/*!
    Processes pending events, for 3 seconds or until there are no more
    events to process, whichever is shorter.

    You can call this function occasionally when your program is busy
    performing a long operation (e.g. copying a file).

    \sa exec(), QTimer, QEventLoop::processEvents()
*/

void QApplication::processEvents()
{
    processEvents( 3000 );
}

/*!
    \overload

    Processes pending events for \a maxtime milliseconds or until
    there are no more events to process, whichever is shorter.

    You can call this function occasionally when you program is busy
    doing a long operation (e.g. copying a file).

    \sa exec(), QTimer, QEventLoop::processEvents()
*/
void QApplication::processEvents( int maxtime )
{
    eventLoop()->processEvents( QEventLoop::AllEvents, maxtime );
}

/*! \obsolete
  Waits for an event to occur, processes it, then returns.

  This function is useful for adapting Qt to situations where the
  event processing must be grafted onto existing program loops.

  Using this function in new applications may be an indication of design
  problems.

  \sa processEvents(), exec(), QTimer
*/

void QApplication::processOneEvent()
{
    eventLoop()->processEvents( QEventLoop::AllEvents |
				QEventLoop::WaitForMore );
}

/*****************************************************************************
  Main event loop wrappers
 *****************************************************************************/

/*!
    Returns the application event loop. This function will return
    zero if called during and after destroying QApplication.

    To create your own instance of QEventLoop or QEventLoop subclass create
    it before you create the QApplication object.

    \sa QEventLoop
*/
QEventLoop *QApplication::eventLoop()
{
    if ( !eventloop && !is_app_closing )
	(void) new QEventLoop( qApp, "default event loop" );
    return eventloop;
}


/*!
    Enters the main event loop and waits until exit() is called or the
    main widget is destroyed, and returns the value that was set to
    exit() (which is 0 if exit() is called via quit()).

    It is necessary to call this function to start event handling. The
    main event loop receives events from the window system and
    dispatches these to the application widgets.

    Generally speaking, no user interaction can take place before
    calling exec(). As a special case, modal widgets like QMessageBox
    can be used before calling exec(), because modal widgets call
    exec() to start a local event loop.

    To make your application perform idle processing, i.e. executing a
    special function whenever there are no pending events, use a
    QTimer with 0 timeout. More advanced idle processing schemes can
    be achieved using processEvents().

    \sa quit(), exit(), processEvents(), setMainWidget()
*/
int QApplication::exec()
{
    return eventLoop()->exec();
}

/*!
  Tells the application to exit with a return code.

  After this function has been called, the application leaves the main
  event loop and returns from the call to exec(). The exec() function
  returns \a retcode.

  By convention, a \a retcode of 0 means success, and any non-zero
  value indicates an error.

  Note that unlike the C library function of the same name, this
  function \e does return to the caller -- it is event processing that
  stops.

  \sa quit(), exec()
*/
void QApplication::exit( int retcode )
{
    qApp->eventLoop()->exit( retcode );
}

/*!
    \obsolete

    This function enters the main event loop (recursively). Do not call
    it unless you really know what you are doing.
*/
int QApplication::enter_loop()
{
    return eventLoop()->enterLoop();
}

/*!
    \obsolete

    This function exits from a recursive call to the main event loop.
    Do not call it unless you are an expert.
*/
void QApplication::exit_loop()
{
    eventLoop()->exitLoop();
}

/*!
    \obsolete

    Returns the current loop level.
*/
int QApplication::loopLevel() const
{
    return eventLoop()->loopLevel();
}


/*!
    This function returns TRUE if there are pending events; otherwise
    returns FALSE. Pending events can be either from the window system
    or posted events using QApplication::postEvent().
*/
bool QApplication::hasPendingEvents()
{
    return eventLoop()->hasPendingEvents();
}

/*!
  Returns the version of the Windows operating system that is running:

  \list
  \i Qt::WV_95 - Windows 95
  \i Qt::WV_98 - Windows 98
  \i Qt::WV_Me - Windows Me
  \i Qt::WV_NT - Windows NT 4.x
  \i Qt::WV_2000 - Windows 2000 (NT5)
  \i Qt::WV_XP - Windows XP
  \endlist

  Note that this function is implemented for the Windows version
  of Qt only.
*/

#if defined(Q_OS_CYGWIN)
Qt::WindowsVersion QApplication::winVersion()
{
    return qt_winver;
}
#endif

#ifndef QT_NO_TRANSLATION

bool qt_detectRTLLanguage()
{
    return QApplication::tr( "QT_LAYOUT_DIRECTION",
	    "Translate this string to the string 'LTR' in left-to-right"
	    " languages or to 'RTL' in right-to-left languages (such as Hebrew"
	    " and Arabic) to get proper widget layout." ) == "RTL";
}

/*!
  Adds the message file \a mf to the list of message files to be used
  for translations.

  Multiple message files can be installed. Translations are searched
  for in the last installed message file, then the one from last, and
  so on, back to the first installed message file. The search stops as
  soon as a matching translation is found.

  \sa removeTranslator() translate() QTranslator::load()
*/

void QApplication::installTranslator( QTranslator * mf )
{
    if ( !mf )
	return;
    if ( !translators )
	translators = new QValueList<QTranslator*>;

    translators->prepend( mf );

#ifndef QT_NO_TRANSLATION_BUILDER
    if ( mf->isEmpty() )
	return;
#endif
}

/*!
  Removes the message file \a mf from the list of message files used by
  this application. (It does not delete the message file from the file
  system.)

  \sa installTranslator() translate(), QObject::tr()
*/

void QApplication::removeTranslator( QTranslator * mf )
{
    if ( !translators || !mf )
	return;
}

#ifndef QT_NO_TEXTCODEC
/*! \obsolete
  This is the same as QTextCodec::setCodecForTr().
*/
void QApplication::setDefaultCodec( QTextCodec* codec )
{
    QTextCodec::setCodecForTr( codec );
}

/*! \obsolete
  Returns QTextCodec::codecForTr().
*/
QTextCodec* QApplication::defaultCodec() const
{
    return QTextCodec::codecForTr();
}
#endif //QT_NO_TEXTCODEC

/*! \enum QApplication::Encoding

  This enum type defines the 8-bit encoding of character string
  arguments to translate():

  \value DefaultCodec - the encoding specified by
  QTextCodec::codecForTr() (Latin-1 if none has been set)
  \value UnicodeUTF8 - UTF-8

  \sa QObject::tr(), QObject::trUtf8(), QString::fromUtf8()
*/

/*! \reentrant
  Returns the translation text for \a sourceText, by querying the
  installed messages files. The message files are searched from the most
  recently installed message file back to the first installed message
  file.

  QObject::tr() and QObject::trUtf8() provide this functionality more
  conveniently.

  \a context is typically a class name (e.g., "MyDialog") and
  \a sourceText is either English text or a short identifying text, if
  the output text will be very long (as for help texts).

  \a comment is a disambiguating comment, for when the same \a
  sourceText is used in different roles within the same context. By
  default, it is null. \a encoding indicates the 8-bit encoding of
  character stings

  See the \l QTranslator documentation for more information about
  contexts and comments.

  If none of the message files contain a translation for \a
  sourceText in \a context, this function returns a QString
  equivalent of \a sourceText. The encoding of \a sourceText is
  specified by \e encoding; it defaults to \c DefaultCodec.

  This function is not virtual. You can use alternative translation
  techniques by subclassing \l QTranslator.

  \warning This method is reentrant only if all translators are
  installed \e before calling this method.  Installing or removing
  translators while performing translations is not supported.  Doing
  so will most likely result in crashes or other undesirable behavior.

  \sa QObject::tr() installTranslator() defaultCodec()
*/

QString QApplication::translate( const char * context, const char * sourceText,
				 const char * comment, Encoding encoding ) const
{
    if ( !sourceText )
	return QString::null;

    if ( translators ) {
	QValueList<QTranslator*>::iterator it;
	QTranslator * mf;
	QString result;
	for ( it = translators->begin(); it != translators->end(); ++it ) {
	    mf = *it;
	    result = mf->findMessage( context, sourceText, comment ).translation();
	    if ( !result.isNull() )
		return result;
	}
    }
#ifndef QT_NO_TEXTCODEC
    if ( encoding == UnicodeUTF8 )
	return QString::fromUtf8( sourceText );
    else if ( QTextCodec::codecForTr() != 0 )
	return QTextCodec::codecForTr()->toUnicode( sourceText );
    else
#endif
	return QString::fromLatin1( sourceText );
}

#endif

/*****************************************************************************
  QApplication management of posted events
 *****************************************************************************/

//see also notify(), which does the removal of ChildInserted when ChildRemoved.

/*!
  Adds the event \a event with the object \a receiver as the receiver of the
  event, to an event queue and returns immediately.

  The event must be allocated on the heap since the post event queue
  will take ownership of the event and delete it once it has been posted.

  When control returns to the main event loop, all events that are
  stored in the queue will be sent using the notify() function.

  \threadsafe

  \sa sendEvent(), notify()
*/

void QApplication::postEvent( QObject *receiver, QEvent *event )
{
    if ( receiver == 0 ) {
#if defined(QT_CHECK_NULL)
	qWarning( "QApplication::postEvent: Unexpected null receiver" );
#endif
	delete event;
	return;
    }

#ifdef QT_THREAD_SUPPORT
    QMutexLocker locker( postevent_mutex );
#endif // QT_THREAD_SUPPORT

    if ( !globalPostedEvents ) {			// create list
	globalPostedEvents = new QPostEventList;
	Q_CHECK_PTR( globalPostedEvents );
	globalPostedEvents->setAutoDelete( TRUE );
	qapp_cleanup_events.set( &globalPostedEvents );
    }

    if ( !receiver->postedEvents )
	receiver->postedEvents = new QPostEventList;
    QPostEventList * l = receiver->postedEvents;

    // if this is one of the compressible events, do compression
    if ( event->type() == QEvent::Paint ||
	 event->type() == QEvent::LayoutHint ||
	 event->type() == QEvent::Resize ||
	 event->type() == QEvent::Move ||
	 event->type() == QEvent::LanguageChange ) {
	l->first();
	QPostEvent * cur = 0;
	for ( ;; ) {
	    while ( (cur=l->current()) != 0 &&
		    ( cur->receiver != receiver ||
		      cur->event == 0 ||
		      cur->event->type() != event->type() ) )
		l->next();
	    if ( l->current() != 0 ) {
            if ( cur->event->type() == QEvent::LanguageChange ) {
		    delete event;
		    return;
            }
	    }
	    break;
	};
    }

    // if no compression could be done, just append something
    event->posted = TRUE;
    QPostEvent * pe = new QPostEvent( receiver, event );
    l->append( pe );
    globalPostedEvents->append( pe );

    if (eventloop)
	eventloop->wakeUp();
}


/*! \overload

    Dispatches all posted events, i.e. empties the event queue.
*/
void QApplication::sendPostedEvents()
{
    sendPostedEvents( 0, 0 );
}



/*!
  Immediately dispatches all events which have been previously queued
  with QApplication::postEvent() and which are for the object \a receiver
  and have the event type \a event_type.

  Note that events from the window system are \e not dispatched by this
  function, but by processEvents().

  If \a receiver is null, the events of \a event_type are sent for all
  objects. If \a event_type is 0, all the events are sent for \a receiver.
*/

void QApplication::sendPostedEvents( QObject *receiver, int event_type )
{
    // Make sure the object hierarchy is stable before processing events
    // to avoid endless loops
    if ( receiver == 0 && event_type == 0 )
	sendPostedEvents( 0, QEvent::ChildInserted );

    if ( !globalPostedEvents || ( receiver && !receiver->postedEvents ) )
	return;

#ifdef QT_THREAD_SUPPORT
    QMutexLocker locker( postevent_mutex );
#endif

    bool sent = TRUE;
    while ( sent ) {
	sent = FALSE;

	if ( !globalPostedEvents || ( receiver && !receiver->postedEvents ) )
	    return;

	// if we have a receiver, use the local list. Otherwise, use the
	// global list
	QPostEventList * l = receiver ? receiver->postedEvents : globalPostedEvents;

	// okay. here is the tricky loop. be careful about optimizing
	// this, it looks the way it does for good reasons.
	QPostEventListIt it( *l );
	QPostEvent *pe;
	while ( (pe=it.current()) != 0 ) {
	    ++it;
	    if ( pe->event // hasn't been sent yet
		 && ( receiver == 0 // we send to all receivers
		      || receiver == pe->receiver ) // we send to THAT receiver
		 && ( event_type == 0 // we send all types
		      || event_type == pe->event->type() ) ) { // we send THAT type
		// first, we diddle the event so that we can deliver
		// it, and that noone will try to touch it later.
		pe->event->posted = FALSE;
		QEvent * e = pe->event;
		QObject * r = pe->receiver;
		pe->event = 0;

		// next, update the data structure so that we're ready
		// for the next event.

		// look for the local list, and take whatever we're
		// delivering out of it. r->postedEvents maybe *l
		if ( r->postedEvents ) {
		    r->postedEvents->removeRef( pe );
		    // if possible, get rid of that list. this is not
		    // ideal - we will create and delete a list for
		    // each update() call. it would be better if we'd
		    // leave the list empty here, and delete it
		    // somewhere else if it isn't being used.
		    if ( r->postedEvents->isEmpty() ) {
			delete r->postedEvents;
			r->postedEvents = 0;
		    }
		}

#ifdef QT_THREAD_SUPPORT
		if ( locker.mutex() ) locker.mutex()->unlock();
#endif // QT_THREAD_SUPPORT
		// after all that work, it's time to deliver the event.

		    sent = TRUE;
		    QApplication::sendEvent( r, e );

#ifdef QT_THREAD_SUPPORT
		if ( locker.mutex() ) locker.mutex()->lock();
#endif // QT_THREAD_SUPPORT

		delete e;
		// careful when adding anything below this point - the
		// sendEvent() call might invalidate any invariants this
		// function depends on.
	    }
	}

	// clear the global list, i.e. remove everything that was
	// delivered.
	if ( l == globalPostedEvents ) {
	    globalPostedEvents->first();
	    while( (pe=globalPostedEvents->current()) != 0 ) {
		if ( pe->event )
		    globalPostedEvents->next();
		else
		    globalPostedEvents->remove();
	    }
	}
    }
}

/*!
  Removes all events posted using postEvent() for \a receiver.

  The events are \e not dispatched, instead they are removed from the
  queue. You should never need to call this function. If you do call it,
  be aware that killing events may cause \a receiver to break one or
  more invariants.

  \threadsafe
*/

void QApplication::removePostedEvents( QObject *receiver )
{
    if ( !receiver )
	return;

#ifdef QT_THREAD_SUPPORT
    QMutexLocker locker( postevent_mutex );
#endif // QT_THREAD_SUPPORT

    // the QObject destructor calls this function directly.  this can
    // happen while the event loop is in the middle of posting events,
    // and when we get here, we may not have any more posted events
    // for this object.
    if ( !receiver->postedEvents )
	return;

    // iterate over the object-specifc list and delete the events.
    // leave the QPostEvent objects; they'll be deleted by
    // sendPostedEvents().
    QPostEventList * l = receiver->postedEvents;
    receiver->postedEvents = 0;
    l->first();
    QPostEvent * pe;
    while( (pe=l->current()) != 0 ) {
	if ( pe->event ) {
	    pe->event->posted = FALSE;
	    delete pe->event;
	    pe->event = 0;
	}
	l->remove();
    }
    delete l;
}


/*!
  Removes \a event from the queue of posted events, and emits a
  warning message if appropriate.

  \warning This function can be \e really slow. Avoid using it, if
  possible.

  \threadsafe
*/

void QApplication::removePostedEvent( QEvent *  event )
{
    if ( !event || !event->posted )
	return;

    if ( !globalPostedEvents ) {
#if defined(QT_DEBUG)
	qDebug( "QApplication::removePostedEvent: %p %d is posted: impossible",
		(void*)event, event->type() );
	return;
#endif
    }

#ifdef QT_THREAD_SUPPORT
    QMutexLocker locker( postevent_mutex );
#endif // QT_THREAD_SUPPORT

    QPostEventListIt it( *globalPostedEvents );
    QPostEvent * pe;
    while( (pe = it.current()) != 0 ) {
	++it;
	if ( pe->event == event ) {
#if defined(QT_DEBUG)
	    const char *n;
	    switch ( event->type() ) {
	    case QEvent::Timer:
		n = "Timer";
		break;
	    case QEvent::MouseButtonPress:
		n = "MouseButtonPress";
		break;
	    case QEvent::MouseButtonRelease:
		n = "MouseButtonRelease";
		break;
	    case QEvent::MouseButtonDblClick:
		n = "MouseButtonDblClick";
		break;
	    case QEvent::MouseMove:
		n = "MouseMove";
		break;
#ifndef QT_NO_WHEELEVENT
	    case QEvent::Wheel:
		n = "Wheel";
		break;
#endif
	    case QEvent::KeyPress:
		n = "KeyPress";
		break;
	    case QEvent::KeyRelease:
		n = "KeyRelease";
		break;
	    case QEvent::FocusIn:
		n = "FocusIn";
		break;
	    case QEvent::FocusOut:
		n = "FocusOut";
		break;
	    case QEvent::Enter:
		n = "Enter";
		break;
	    case QEvent::Leave:
		n = "Leave";
		break;
	    case QEvent::Paint:
		n = "Paint";
		break;
	    case QEvent::Move:
		n = "Move";
		break;
	    case QEvent::Resize:
		n = "Resize";
		break;
	    case QEvent::Create:
		n = "Create";
		break;
	    case QEvent::Destroy:
		n = "Destroy";
		break;
	    case QEvent::Close:
		n = "Close";
		break;
	    case QEvent::Quit:
		n = "Quit";
		break;
	    default:
		n = "<other>";
		break;
	    }
	    qWarning("QEvent: Warning: %s event deleted while posted to %s %s",
		     n,
		     pe->receiver ? pe->receiver->className() : "null",
		     pe->receiver ? pe->receiver->name() : "object" );
	    // note the beautiful uglehack if !pe->receiver :)
#endif
	    event->posted = FALSE;
	    delete pe->event;
	    pe->event = 0;
	    return;
	}
    }
}




/*! \fn void QApplication::lock()

  Lock the Qt Library Mutex. If another thread has already locked the
  mutex, the calling thread will block until the other thread has
  unlocked the mutex.

  \sa unlock() locked() \link threads.html Thread Support in Qt\endlink
*/


/*! \fn void QApplication::unlock(bool wakeUpGui)

  Unlock the Qt Library Mutex. If \a wakeUpGui is TRUE (the default),
  then the GUI thread will be woken with QApplication::wakeUpGuiThread().

  \sa lock(), locked() \link threads.html Thread Support in Qt\endlink
*/


/*! \fn bool QApplication::locked()

  Returns TRUE if the Qt Library Mutex is locked by a different thread;
  otherwise returns FALSE.

  \warning Due to different implementations of recursive mutexes on
  the supported platforms, calling this function from the same thread
  that previously locked the mutex will give undefined results.

  \sa lock() unlock() \link threads.html Thread Support in Qt\endlink
*/

/*! \fn bool QApplication::tryLock()

  Attempts to lock the Qt Library Mutex, and returns immediately. If
  the lock was obtained, this function returns TRUE. If another thread
  has locked the mutex, this function returns FALSE, instead of
  waiting for the lock to become available.

  The mutex must be unlocked with unlock() before another thread can
  successfully lock it.

  \sa lock(), unlock() \link threads.html Thread Support in Qt\endlink
*/

#if defined(QT_THREAD_SUPPORT)
void QApplication::lock()
{
    qt_mutex->lock();
}

void QApplication::unlock(bool // wakeUpGui
                          )
{
    qt_mutex->unlock();

}

bool QApplication::locked()
{
    return qt_mutex->locked();
}

bool QApplication::tryLock()
{
    return qt_mutex->tryLock();
}
#endif


/*!
  \fn bool QApplication::isSessionRestored() const

  Returns TRUE if the application has been restored from an earlier
  \link session.html session\endlink; otherwise returns FALSE.

  \sa sessionId(), commitData(), saveState()
*/


/*!
  \fn QString QApplication::sessionId() const

  Returns the current \link session.html session's\endlink identifier.

  If the application has been restored from an earlier session, this
  identifier is the same as it was in that previous session.

  The session identifier is guaranteed to be unique both for different
  applications and for different instances of the same application.

  \sa isSessionRestored(), sessionKey(), commitData(), saveState()
 */

/*!
  \fn QString QApplication::sessionKey() const

  Returns the session key in the current \link session.html
  session\endlink.

  If the application has been restored from an earlier session, this
  key is the same as it was when the previous session ended.

  The session key changes with every call of commitData() or
  saveState().

  \sa isSessionRestored(), sessionId(), commitData(), saveState()
 */


/*!
  \fn void QApplication::commitData( QSessionManager& sm )

  This function deals with \link session.html session
  management\endlink. It is invoked when the QSessionManager wants the
  application to commit all its data.

  Usually this means saving all open files, after getting
  permission from the user. Furthermore you may want to provide a means
  by which the user can cancel the shutdown.

  Note that you should not exit the application within this function.
  Instead, the session manager may or may not do this afterwards,
  depending on the context.

  \warning Within this function, no user interaction is possible, \e
  unless you ask the session manager \a sm for explicit permission.
  See QSessionManager::allowsInteraction() and
  QSessionManager::allowsErrorInteraction() for details and example
  usage.

  The default implementation requests interaction and sends a close
  event to all visible top level widgets. If any event was
  rejected, the shutdown is canceled.

  \sa isSessionRestored(), sessionId(), saveState(), \link session.html the Session Management overview\endlink
*/
