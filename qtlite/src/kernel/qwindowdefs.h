/****************************************************************************
** $Id: qwindowdefs.h,v 1.1 2003/11/04 10:14:21 Sigma Exp $
**
** Definition of general window system dependent functions, types and
** constants
**
** Created : 931029
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

#ifndef QWINDOWDEFS_H
#define QWINDOWDEFS_H

#ifndef QT_H
#include "qobjectdefs.h"
#include "qstring.h"
#include "qnamespace.h"
#endif // QT_H

// Class forward definitions

class QPrinter;
class QTimer;
class QTime;


// Window system dependent definitions

#if defined(Q_WS_MAC)
#ifdef MACOSX_101
typedef struct OpaqueEventLoopTimerRef* EventLoopTimerRef;
typedef struct OpaqueMenuHandle *MenuRef;
#else
typedef struct __EventLoopTimer*        EventLoopTimerRef;
typedef struct OpaqueMenuRef*        MenuRef;
#endif

#ifndef Q_WS_MACX
typedef struct CGContext *CGContextRef;
#endif
typedef struct OpaqueControlRef *ControlRef;
typedef struct OpaqueWindowGroupRef *WindowGroupRef;
typedef struct OpaqueGrafPtr *CGrafPtr;
typedef struct OpaquePMPrintSession *PMPrintSession;
typedef struct OpaquePMPrintSettings *PMPrintSettings;
typedef struct OpaquePMPageFormat *PMPageFormat;
typedef struct Point Point;
typedef struct OpaqueEventHandlerRef*   EventHandlerRef;
typedef struct OpaqueEventHandlerCallRef*  EventHandlerCallRef;
typedef struct OpaqueEventRef*          EventRef;
typedef long int OSStatus;
typedef struct OpaqueScrapRef *ScrapRef;
typedef struct OpaqueRgnHandle *RgnHandle;
typedef struct OpaqueWindowPtr *WindowPtr;
typedef WindowPtr WindowRef;
typedef struct OpaqueGrafPtr *GWorldPtr;
typedef GWorldPtr GrafPtr;
typedef struct GDevice **GDHandle;
typedef struct ColorTable ColorTable;
typedef struct BitMap BitMap;
typedef struct EventRecord EventRecord;
typedef void * MSG;
typedef int WId;

#endif // Q_WS_MAC

#if defined(Q_WS_WIN)
#include "qwindowdefs_win.h"
#endif // Q_WS_WIN


#if defined(Q_OS_TEMP)
#include "qwinfunctions.h"
#endif // Q_OS_TEMP

class QApplication;

#if defined(NEEDS_QMAIN)
#define main qMain
#endif

// Global platform-independent types and functions

typedef Q_INT32 QCOORD;				// coordinate type
const QCOORD QCOORD_MAX =  2147483647;
const QCOORD QCOORD_MIN = -QCOORD_MAX - 1;

typedef unsigned int QRgb;			// RGB triplet

Q_EXPORT const char *qAppName();		// get application name

// Misc functions

typedef void (*QtCleanUpFunction)();
Q_EXPORT void qAddPostRoutine( QtCleanUpFunction );
Q_EXPORT void qRemovePostRoutine( QtCleanUpFunction );

#if !defined(QT_CLEAN_NAMESPACE)
// source compatibility with Qt 2.x
typedef QtCleanUpFunction Q_CleanUpFunction;
#endif


#endif // QWINDOWDEFS_H
