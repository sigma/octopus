/****************************************************************************
** $Id: qinternal_p.h,v 1.1 2003/11/04 10:14:21 Sigma Exp $
**
** Definition of some shared interal classes
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

#ifndef QINTERNAL_P_H
#define QINTERNAL_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of a number of Qt sources files.  This header file may change from
// version to version without notice, or even be removed.
//
// We mean it.
//
//
#ifndef QT_H
#include "qnamespace.h"
#include "qptrlist.h"
#include "qcstring.h"
#include "qiodevice.h"
#endif // QT_H


class QVirtualDestructor {
public:
    virtual ~QVirtualDestructor() {}
};

template <class T>
class QAutoDeleter : public QVirtualDestructor {
public:
    QAutoDeleter( T* p ) : ptr( p ) {}
    ~QAutoDeleter() { delete ptr; }
    T* data() const { return ptr; }
private:
    T* ptr;
};

template <class T>
T* qAutoDeleterData( QAutoDeleter<T>* ad )
{
    if ( !ad )
	return 0;
    return ad->data();
}

template <class T>
QAutoDeleter<T>* qAutoDeleter( T* p )
{
    return new QAutoDeleter<T>( p );
}

class Q_EXPORT QMembuf
{
public:
    QMembuf();
    ~QMembuf();

    void append( QByteArray *ba );
    void clear();

    bool consumeBytes( Q_ULONG nbytes, char *sink );
    QByteArray readAll();
    bool scanNewline( QByteArray *store );
    bool canReadLine() const;

    int ungetch( int ch );

    QIODevice::Offset size() const;

private:

    QPtrList<QByteArray> *buf;
    QIODevice::Offset _size;
    QIODevice::Offset _index;
};

inline void QMembuf::append( QByteArray *ba )
{ buf->append( ba ); _size += ba->size(); }

inline void QMembuf::clear()
{ buf->clear(); _size=0; _index=0; }

inline QByteArray QMembuf::readAll()
{ QByteArray ba(_size); consumeBytes(_size,ba.data()); return ba; }

inline bool QMembuf::canReadLine() const
{ return ((QMembuf*)this)->scanNewline( 0 ); }

inline QIODevice::Offset QMembuf::size() const
{ return _size; }

#endif // QINTERNAL_P_H
