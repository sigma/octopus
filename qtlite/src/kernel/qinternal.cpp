/****************************************************************************
** $Id: qinternal.cpp,v 1.1 2003/11/04 10:14:21 Sigma Exp $
**
** Implementation of some internal classes
**
** Created : 010427
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
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

#include "private/qinternal_p.h"
#include "qcleanuphandler.h"

// *******************************************************************
// QMembuf declaration and implementation
// *******************************************************************

/*  \internal
    This class implements an efficient buffering of data that is often used by
    asynchronous IO classes like QSocket, QHttp and QProcess.
*/

QMembuf::QMembuf() : _size(0), _index(0)
{
    buf = new QPtrList<QByteArray>;
    buf->setAutoDelete( TRUE );
}

QMembuf::~QMembuf()
{
    delete buf;
}

/*! \internal
    This function consumes \a nbytes bytes of data from the
    buffer and copies it into \a sink. If \a sink is a 0 pointer
    the data goes into the nirvana.
*/
bool QMembuf::consumeBytes( Q_ULONG nbytes, char *sink )
{
    if ( nbytes <= 0 || nbytes > _size )
	return FALSE;
    _size -= nbytes;
    for ( ;; ) {
	QByteArray *a = buf->first();
	if ( _index + nbytes >= a->size() ) {
	    // Here we skip the whole byte array and get the next later
	    int len = a->size() - _index;
	    if ( sink ) {
		memcpy( sink, a->data()+_index, len );
		sink += len;
	    }
	    nbytes -= len;
	    buf->remove();
	    _index = 0;
	    if ( nbytes == 0 )
		break;
	} else {
	    // Here we skip only a part of the first byte array
	    if ( sink )
		memcpy( sink, a->data()+_index, nbytes );
	    _index += nbytes;
	    break;
	}
    }
    return TRUE;
}

/*! \internal
    Scans for any occurrence of '\n' in the buffer. If \a store
    is not 0 the text up to the first '\n' (or terminating 0) is
    written to \a store, and a terminating 0 is appended to \a store
    if necessary. Returns TRUE if a '\n' was found; otherwise returns
    FALSE.
*/
bool QMembuf::scanNewline( QByteArray *store )
{
    if ( _size == 0 )
	return FALSE;
    int i = 0; // index into 'store'
    QByteArray *a = 0;
    char *p;
    int n;
    for ( ;; ) {
	if ( !a ) {
	    a = buf->first();
	    if ( !a || a->size() == 0 )
		return FALSE;
	    p = a->data() + _index;
	    n = a->size() - _index;
	} else {
	    a = buf->next();
	    if ( !a || a->size() == 0 )
		return FALSE;
	    p = a->data();
	    n = a->size();
	}
	if ( store ) {
	    while ( n-- > 0 ) {
		*(store->data()+i) = *p;
		if ( ++i == (int)store->size() )
		    store->resize( store->size() < 256
				   ? 1024 : store->size()*4 );
		switch ( *p ) {
		    case '\0':
			store->resize( i );
			return FALSE;
		    case '\n':
			*(store->data()+i) = '\0';
			store->resize( i );
			return TRUE;
		}
		p++;
	    }
	} else {
	    while ( n-- > 0 ) {
		switch ( *p++ ) {
		    case '\0':
			return FALSE;
		    case '\n':
			return TRUE;
		}
	    }
	}
    }
}

int QMembuf::ungetch( int ch )
{
    if ( buf->isEmpty() || _index==0 ) {
	// we need a new QByteArray
	QByteArray *ba = new QByteArray( 1 );
	buf->insert( 0, ba );
	_size++;
	ba->at( 0 ) = ch;
    } else {
	// we can reuse a place in the buffer
	QByteArray *ba = buf->first();
	_index--;
	_size++;
	ba->at( _index ) = ch;
    }
    return ch;
}
