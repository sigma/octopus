/****************************************************************************
** $Id: qeventloop_x11.cpp,v 1.1 2003/11/04 10:14:21 Sigma Exp $
**
** Implementation of QEventLoop class
**
** Copyright (C) 2000-2002 Trolltech AS.  All rights reserved.
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

#include "qeventloop_p.h" // includes qplatformdefs.h
#include "qeventloop.h"
#include "qapplication.h"
#include "qbitarray.h"

#if defined(QT_THREAD_SUPPORT)
#  include "qmutex.h"
#endif // QT_THREAD_SUPPORT

#include <errno.h>


// from qeventloop_unix.cpp
extern timeval *qt_wait_timer();
extern void cleanupTimers();

// ### this needs to go away at some point...
typedef void (*VFPTR)();
typedef QValueList<VFPTR> QVFuncList;
void qt_install_preselect_handler( VFPTR );
void qt_remove_preselect_handler( VFPTR );
static QVFuncList *qt_preselect_handler = 0;
void qt_install_postselect_handler( VFPTR );
void qt_remove_postselect_handler( VFPTR );
static QVFuncList *qt_postselect_handler = 0;

void qt_install_preselect_handler( VFPTR handler )
{
    if ( !qt_preselect_handler )
	qt_preselect_handler = new QVFuncList;
    qt_preselect_handler->append( handler );
}
void qt_remove_preselect_handler( VFPTR handler )
{
    if ( qt_preselect_handler ) {
	QVFuncList::Iterator it = qt_preselect_handler->find( handler );
	if ( it != qt_preselect_handler->end() )
		qt_preselect_handler->remove( it );
    }
}
void qt_install_postselect_handler( VFPTR handler )
{
    if ( !qt_postselect_handler )
	qt_postselect_handler = new QVFuncList;
    qt_postselect_handler->prepend( handler );
}
void qt_remove_postselect_handler( VFPTR handler )
{
    if ( qt_postselect_handler ) {
	QVFuncList::Iterator it = qt_postselect_handler->find( handler );
	if ( it != qt_postselect_handler->end() )
		qt_postselect_handler->remove( it );
    }
}


void QEventLoop::init()
{
    // initialize the common parts of the event loop
    pipe( d->thread_pipe );
    fcntl(d->thread_pipe[0], F_SETFD, FD_CLOEXEC);
    fcntl(d->thread_pipe[1], F_SETFD, FD_CLOEXEC);

    d->sn_highest = -1;

    // intitialize the X11 parts of the event loop
    d->xfd = -1;
}

void QEventLoop::cleanup()
{
    // cleanup the common parts of the event loop
    close( d->thread_pipe[0] );
    close( d->thread_pipe[1] );
    cleanupTimers();

    // cleanup the X11 parts of the event loop
    d->xfd = -1;
}

bool QEventLoop::processEvents( ProcessEventsFlags flags )
{
    // process events from the X server
    int	   nevents = 0;

#if defined(QT_THREAD_SUPPORT)
    QMutexLocker locker( QApplication::qt_mutex );
#endif

        if ( d->shortcut ) {
	return FALSE;
    }

    QApplication::sendPostedEvents();

    const uint exclude_all = ExcludeSocketNotifiers | 0x08;
    // 0x08 == ExcludeTimers for X11 only
    if ( nevents > 0 && ( flags & exclude_all ) == exclude_all &&
	 ( flags & WaitForMore ) ) {
	return TRUE;
    }

    // don't block if exitLoop() or exit()/quit() has been called.
    bool canWait = d->exitloop || d->quitnow ? FALSE : (flags & WaitForMore);

    // Process timers and socket notifiers - the common UNIX stuff

    // return the maximum time we can wait for an event.
    static timeval zerotm;
    timeval *tm = 0;
    if ( ! ( flags & 0x08 ) ) {			// 0x08 == ExcludeTimers for X11 only
	tm = qt_wait_timer();			// wait for timer or X event
	if ( !canWait ) {
	    if ( !tm )
		tm = &zerotm;
	    tm->tv_sec  = 0;			// no time to wait
	    tm->tv_usec = 0;
	}
    }

    int highest = 0;
    if ( ! ( flags & ExcludeSocketNotifiers ) ) {
	// return the highest fd we can wait for input on
	if ( d->sn_highest >= 0 ) {			// has socket notifier(s)
	    if ( d->sn_vec[0].list && ! d->sn_vec[0].list->isEmpty() )
		d->sn_vec[0].select_fds = d->sn_vec[0].enabled_fds;
	    else
		FD_ZERO( &d->sn_vec[0].select_fds );

	    if ( d->sn_vec[1].list && ! d->sn_vec[1].list->isEmpty() )
		d->sn_vec[1].select_fds = d->sn_vec[1].enabled_fds;
	    else
		FD_ZERO( &d->sn_vec[1].select_fds );

	    if ( d->sn_vec[2].list && ! d->sn_vec[2].list->isEmpty() )
		d->sn_vec[2].select_fds = d->sn_vec[2].enabled_fds;
	    else
		FD_ZERO( &d->sn_vec[2].select_fds );
	} else {
	    FD_ZERO( &d->sn_vec[0].select_fds );

	    FD_ZERO( &d->sn_vec[1].select_fds );
	    FD_ZERO( &d->sn_vec[2].select_fds );
	}

	highest = d->sn_highest;
    } else {
        FD_ZERO( &d->sn_vec[0].select_fds );
	FD_ZERO( &d->sn_vec[1].select_fds );
	FD_ZERO( &d->sn_vec[2].select_fds );
    }

    FD_SET( d->thread_pipe[0], &d->sn_vec[0].select_fds );
    highest = QMAX( highest, d->thread_pipe[0] );

    if ( canWait )
	emit aboutToBlock();

    if ( qt_preselect_handler ) {
	QVFuncList::Iterator it, end = qt_preselect_handler->end();
	for ( it = qt_preselect_handler->begin(); it != end; ++it )
	    (**it)();
    }

    // unlock the GUI mutex and select.  when we return from this function, there is
    // something for us to do
#if defined(QT_THREAD_SUPPORT)
    locker.mutex()->unlock();
#endif

    int nsel;
    nsel = select( highest + 1,
		   &d->sn_vec[0].select_fds,
		   &d->sn_vec[1].select_fds,
		   &d->sn_vec[2].select_fds,
		   tm );

    // relock the GUI mutex before processing any pending events
#if defined(QT_THREAD_SUPPORT)
    locker.mutex()->lock();
#endif

    // we are awake, broadcast it
    emit awake();
    emit qApp->guiThreadAwake();

    if ( nsel == -1 ) {
	if ( errno != EINTR && errno != EAGAIN )
	    perror( "select" );
	return (nevents > 0);
    }

#undef FDCAST

    // some other thread woke us up... consume the data on the thread pipe so that
    // select doesn't immediately return next time
    if ( nsel > 0 && FD_ISSET( d->thread_pipe[0], &d->sn_vec[0].select_fds ) ) {
	char c;
	::read( d->thread_pipe[0], &c, 1 );
    }

    if ( qt_postselect_handler ) {
	QVFuncList::Iterator it, end = qt_postselect_handler->end();
	for ( it = qt_postselect_handler->begin(); it != end; ++it )
	    (**it)();
    }

    // activate socket notifiers
    if ( ! ( flags & ExcludeSocketNotifiers ) && nsel > 0 && d->sn_highest >= 0 ) {
	// if select says data is ready on any socket, then set the socket notifier
	// to pending
	int i;
	for ( i=0; i<3; i++ ) {
	    if ( ! d->sn_vec[i].list )
		continue;

	    QPtrList<QSockNot> *list = d->sn_vec[i].list;
	    QSockNot *sn = list->first();
	    while ( sn ) {
		if ( FD_ISSET( sn->fd, &d->sn_vec[i].select_fds ) )
		    setSocketNotifierPending( sn->obj );
		sn = list->next();
	    }
	}

	nevents += activateSocketNotifiers();
    }

    // activate timers
    if ( ! ( flags & 0x08 ) ) {
	// 0x08 == ExcludeTimers for X11 only
	nevents += activateTimers();
    }


    // return true if we handled events, false otherwise
    return (nevents > 0);
}

bool QEventLoop::hasPendingEvents() const
{
    extern uint qGlobalPostedEventsCount(); // from qapplication.cpp
    return ( qGlobalPostedEventsCount() );
}

