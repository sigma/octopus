/****************************************************************************
** $Id: qsignalslotimp.h,v 1.1 2003/11/04 10:14:21 Sigma Exp $
**
** Definition of signal/slot collections etc.
**
** Created : 980821
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

#ifndef QSIGNALSLOTIMP_H
#define QSIGNALSLOTIMP_H

#ifndef QT_H
#include "qconnection.h"
#include "qptrlist.h"
#include "qptrvector.h"
#endif // QT_H

class Q_EXPORT QConnectionList : public QPtrList<QConnection>
{
public:
    QConnectionList() : QPtrList<QConnection>() {}
    QConnectionList( const QConnectionList &list ) : QPtrList<QConnection>(list) {}
   ~QConnectionList() { clear(); }
    QConnectionList &operator=(const QConnectionList &list)
	{ return (QConnectionList&)QPtrList<QConnection>::operator=(list); }
};

class Q_EXPORT QConnectionListIt : public QPtrListIterator<QConnection>
{
public:
    QConnectionListIt( const QConnectionList &l ) : QPtrListIterator<QConnection>(l) {}
    QConnectionListIt &operator=(const QConnectionListIt &i)
	{ return (QConnectionListIt&)QPtrListIterator<QConnection>::operator=(i); }
};

class Q_EXPORT QSignalVec : public QPtrVector<QConnectionList>
{
public:
    QSignalVec(int size=17 )
	: QPtrVector<QConnectionList>(size) {}
    QSignalVec( const QSignalVec &dict )
	: QPtrVector<QConnectionList>(dict) {}
   ~QSignalVec() { clear(); }
    QSignalVec &operator=(const QSignalVec &dict)
	{ return (QSignalVec&)QPtrVector<QConnectionList>::operator=(dict); }
    QConnectionList* at( uint index ) const  {
	return index >= size()? 0 : QPtrVector<QConnectionList>::at(index);
    }
    bool  insert( uint index, const QConnectionList* d ) {
	if (index >= size() )
	    resize( 2*index + 1);
	return QPtrVector<QConnectionList>::insert(index, d);
    }
};

#define Q_DEFINED_QCONNECTION_LIST
#include "qwinexport.h"
#endif // QSIGNALSLOTIMP_H
