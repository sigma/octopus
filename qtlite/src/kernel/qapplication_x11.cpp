/****************************************************************************
** $Id: qapplication_x11.cpp,v 1.1 2003/11/04 10:14:21 Sigma Exp $
**
** Implementation of X11 startup routines and event handling
**
** Created : 931029
**
** Copyright (C) 1992-2003 Trolltech AS.  All rights reserved.
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
** licenses for Unix/X11 may use this file in accordance with the Qt Commercial
** License Agreement provided with the Software.
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

// ### 4.0: examine Q_EXPORT's below. The respective symbols had all
// been in use (e.g. in the KDE wm ) before the introduction of a version
// map. One might want to turn some of them into propert public API and
// provide a proper alternative for others. See also the exports in
// qapplication_win.cpp which suggest a unification.

// ### needed for solaris-g++ in beta5
#define QT_CLEAN_NAMESPACE

#include "qplatformdefs.h"

// POSIX Large File Support redefines open -> open64
#if defined(open)
# undef open
#endif

// Solaris redefines connect -> __xnet_connect with _XOPEN_SOURCE_EXTENDED.
#if defined(connect)
# undef connect
#endif

// POSIX Large File Support redefines truncate -> truncate64
#if defined(truncate)
# undef truncate
#endif

#include "qapplication.h"
#include "qapplication_p.h"
#include "qobjectlist.h"
#include "qbitarray.h"
#include "qdatetime.h"
#include "qtextcodec.h"
#include "qdatastream.h"
#include "qbuffer.h"
#include "qsocketnotifier.h"
#include "qvaluelist.h"
#include "qdict.h"
#include "qguardedptr.h"
#include "qsettings.h"
#include "qfileinfo.h"

// Input method stuff - UNFINISHED
#include "qinputcontext_p.h"
#include "qinternal_p.h" // shared double buffer cleanup

#if defined(QT_THREAD_SUPPORT)
# include "qthread.h"
#endif

#if defined(QT_DEBUG) && defined(Q_OS_LINUX)
# include "qfile.h"
#endif




#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <locale.h>

//#define X_NOT_BROKEN
#ifdef X_NOT_BROKEN
// Some X libraries are built with setlocale #defined to _Xsetlocale,
// even though library users are then built WITHOUT such a definition.
// This creates a problem - Qt might setlocale() one value, but then
// X looks and doesn't see the value Qt set. The solution here is to
// implement _Xsetlocale just in case X calls it - redirecting it to
// the real libC version.
//
# ifndef setlocale
extern "C" char *_Xsetlocale(int category, const char *locale);
char *_Xsetlocale(int category, const char *locale)
{
    //qDebug("_Xsetlocale(%d,%s),category,locale");
    return setlocale(category,locale);
}
# endif // setlocale
#endif // X_NOT_BROKEN




// Fix old X libraries
#ifndef XK_KP_Home
#define XK_KP_Home              0xFF95
#endif
#ifndef XK_KP_Left
#define XK_KP_Left              0xFF96
#endif
#ifndef XK_KP_Up
#define XK_KP_Up                0xFF97
#endif
#ifndef XK_KP_Right
#define XK_KP_Right             0xFF98
#endif
#ifndef XK_KP_Down
#define XK_KP_Down              0xFF99
#endif
#ifndef XK_KP_Prior
#define XK_KP_Prior             0xFF9A
#endif
#ifndef XK_KP_Next
#define XK_KP_Next              0xFF9B
#endif
#ifndef XK_KP_End
#define XK_KP_End               0xFF9C
#endif
#ifndef XK_KP_Insert
#define XK_KP_Insert            0xFF9E
#endif
#ifndef XK_KP_Delete
#define XK_KP_Delete            0xFF9F
#endif


/*****************************************************************************
  Internal variables and functions
 *****************************************************************************/
static const char *appName;			// application name
static const char *appFont	= 0;		// application font
static const char *appBGCol	= 0;		// application bg color
static const char *appFGCol	= 0;		// application fg color
static const char *appBTNCol	= 0;		// application btn color
static const char *mwGeometry	= 0;		// main widget geometry
static const char *mwTitle	= 0;		// main widget title
//Ming-Che 10/10
static char    *ximServer	= 0;		// XIM Server will connect to
static bool	mwIconic	= FALSE;	// main widget iconified
//Ming-Che 10/10
static bool	noxim		= FALSE;	// connect to xim or not
static char    *appDpyName	= 0;		// X11 display name
#if defined(QT_DEBUG)
static bool	appNoGrab	= FALSE;	// X11 grabbing enabled
static bool	appDoGrab	= FALSE;	// X11 grabbing override (gdb)
#endif

// last timestamp read from QSettings
static uint appliedstamp = 0;



/*****************************************************************************
  qt_init() - initializes Qt for X11
 *****************************************************************************/

// need to get default font?
extern bool qt_app_has_font;

#define XK_MISCELLANY
#define XK_LATIN1
#include <X11/keysymdef.h>

// ### This should be static but it isn't because of the friend declaration
// ### in qpaintdevice.h which then should have a static too but can't have
// ### it because "storage class specifiers invalid in friend function
// ### declarations" :-) Ideas anyone?
void qt_init_internal( int *argcptr, char **argv)
{
    if(0) ;
    else {
	// Qt controls everything (default)

	const char *p;
	int argc = *argcptr;
	int j;

	// Install default error handlers


	// Set application name

	p = strrchr( argv[0], '/' );
	appName = p ? p + 1 : argv[0];

	// Get command line params
	j = 1;
	for ( int i=1; i<argc; i++ ) {
	    if ( argv[i] && *argv[i] != '-' ) {
		argv[j++] = argv[i];
		continue;
	    }
	    QCString arg = argv[i];
	    if ( arg == "-display" ) {
		if ( ++i < argc )
		    appDpyName = argv[i];
	    } else if ( arg == "-fn" || arg == "-font" ) {
		if ( ++i < argc )
		    appFont = argv[i];
	    } else if ( arg == "-bg" || arg == "-background" ) {
		if ( ++i < argc )
		    appBGCol = argv[i];
	    } else if ( arg == "-btn" || arg == "-button" ) {
		if ( ++i < argc )
		    appBTNCol = argv[i];
	    } else if ( arg == "-fg" || arg == "-foreground" ) {
		if ( ++i < argc )
		    appFGCol = argv[i];
	    } else if ( arg == "-name" ) {
		if ( ++i < argc )
		    appName = argv[i];
	    } else if ( arg == "-title" ) {
		if ( ++i < argc )
		    mwTitle = argv[i];
	    } else if ( arg == "-geometry" ) {
		if ( ++i < argc )
		    mwGeometry = argv[i];
		//Ming-Che 10/10
	    } else if ( arg == "-im" ) {
		if ( ++i < argc )
		    ximServer = argv[i];
	    } else if ( arg == "-noxim" ) {
		noxim=TRUE;
		//
	    } else if ( arg == "-iconic" ) {
		mwIconic = !mwIconic;
	    } else if ( arg == "-ncols" ) {   // xv and netscape use this name
	    } else if ( arg == "-visual" ) {  // xv and netscape use this name
		if ( ++i < argc ) {
		}
	    } else if ( arg == "-cmap" ) {    // xv uses this name
	    }
	    else
		argv[j++] = argv[i];
	}

	*argcptr = j;

	// Connect to X server


    }
    // Common code, regardless of whether display is foreign.

    // Get X parameters

    

#if defined(QT_THREAD_SUPPORT)
    QThread::initialize();
#endif

    

    setlocale( LC_ALL, "" );		// use correct char set mapping
    setlocale( LC_NUMERIC, "C" );	// make sprintf()/scanf() work

}



void qt_init( int *argcptr, char **argv, QApplication::Type )
{
    qt_init_internal( argcptr, argv);
}



/*****************************************************************************
  qt_cleanup() - cleans up when the application is finished
 *****************************************************************************/

void qt_cleanup()
{
    appliedstamp = 0;
#if defined(QT_THREAD_SUPPORT)
    QThread::cleanup();
#endif
    
}


/*****************************************************************************
  Platform specific global and internal functions
 *****************************************************************************/


const char *qAppName()				// get application name
{
    return appName;
}
