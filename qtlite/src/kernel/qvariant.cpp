/****************************************************************************
** $Id: qvariant.cpp,v 1.1 2003/11/04 10:14:21 Sigma Exp $
**
** Implementation of QVariant class
**
** Created : 990414
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

#include <float.h>

#include "qvariant.h"
#ifndef QT_NO_VARIANT
#include "qstring.h"
#include "qcstring.h"
#include "qdatastream.h"
#include "qdatetime.h"
#include "qshared.h"
#include "qbitarray.h"

#ifndef DBL_DIG
#define DBL_DIG 10
#endif //DBL_DIG

// Uncomment to test for memory leaks or to run qt/test/qvariant/main.cpp
// #define QVARIANT_DEBUG


#ifdef QVARIANT_DEBUG
int qv_count = 0;
int get_qv_count() { return qv_count; }
#endif

QVariant::Private::Private()
{
#ifdef QVARIANT_DEBUG
    qv_count++;
#endif
    typ = QVariant::Invalid;
    is_null = TRUE;
}

QVariant::Private::Private( Private* d )
{
#ifdef QVARIANT_DEBUG
    qv_count++;
#endif

    switch( d->typ )
	{
	case QVariant::Invalid:
	    break;
	case QVariant::String:
	    value.ptr = new QString( *((QString*)d->value.ptr) );
	    break;
	case QVariant::CString:
	    // QCString is explicit shared
	    value.ptr = new QCString( *((QCString*)d->value.ptr) );
	    break;
#ifndef QT_NO_STRINGLIST
	case QVariant::StringList:
	    value.ptr = new QStringList( *((QStringList*)d->value.ptr) );
	    break;
#endif //QT_NO_STRINGLIST
#ifndef QT_NO_TEMPLATE_VARIANT
	case QVariant::Map:
	    value.ptr = new QMap<QString,QVariant>( *((QMap<QString,QVariant>*)d->value.ptr) );
	    break;
	case QVariant::List:
	    value.ptr = new QValueList<QVariant>( *((QValueList<QVariant>*)d->value.ptr) );
	    break;
#endif
	case QVariant::Date:
	    value.ptr = new QDate( *((QDate*)d->value.ptr) );
	    break;
	case QVariant::Time:
	    value.ptr = new QTime( *((QTime*)d->value.ptr) );
	    break;
	case QVariant::DateTime:
	    value.ptr = new QDateTime( *((QDateTime*)d->value.ptr) );
	    break;
	case QVariant::ByteArray:
	    value.ptr = new QByteArray( *((QByteArray*)d->value.ptr) );
	    break;
	case QVariant::BitArray:
	    value.ptr = new QBitArray( *((QBitArray*)d->value.ptr) );
	    break;
	case QVariant::Int:
	    value.i = d->value.i;
	    break;
	case QVariant::UInt:
	    value.u = d->value.u;
	    break;
	case QVariant::Bool:
	    value.b = d->value.b;
	    break;
	case QVariant::Double:
	    value.d = d->value.d;
	    break;
	default:
	    Q_ASSERT( 0 );
	}

    typ = d->typ;
    is_null = d->is_null;
}

QVariant::Private::~Private()
{
#ifdef QVARIANT_DEBUG
    qv_count--;
#endif
    clear();
}

void QVariant::Private::clear()
{
    switch( typ )
	{
	case QVariant::String:
	    delete (QString*)value.ptr;
	    break;
	case QVariant::CString:
	    delete (QCString*)value.ptr;
	    break;
#ifndef QT_NO_STRINGLIST
	case QVariant::StringList:
	    delete (QStringList*)value.ptr;
	    break;
#endif //QT_NO_STRINGLIST
#ifndef QT_NO_TEMPLATE_VARIANT
	case QVariant::Map:
	    delete (QMap<QString,QVariant>*)value.ptr;
	    break;
	case QVariant::List:
	    delete (QValueList<QVariant>*)value.ptr;
	    break;
#endif
	case QVariant::Date:
	    delete (QDate*)value.ptr;
	    break;
	case QVariant::Time:
	    delete (QTime*)value.ptr;
	    break;
	case QVariant::DateTime:
	    delete (QDateTime*)value.ptr;
	    break;
	case QVariant::ByteArray:
	    delete (QByteArray*)value.ptr;
	    break;
	case QVariant::BitArray:
	    delete (QBitArray*)value.ptr;
	    break;
	case QVariant::Invalid:
	case QVariant::Int:
	case QVariant::UInt:
	case QVariant::LongLong:
	case QVariant::ULongLong:
	case QVariant::Bool:
	case QVariant::Double:
	    break;
	}

    typ = QVariant::Invalid;
    is_null = TRUE;
}

/*!
    \class QVariant qvariant.h
    \brief The QVariant class acts like a union for the most common Qt data types.

    \ingroup objectmodel
    \ingroup misc
    \mainclass

    Because C++ forbids unions from including types that have
    non-default constructors or destructors, most interesting Qt
    classes cannot be used in unions. Without QVariant, this would be
    a problem for QObject::property() and for database work, etc.

    A QVariant object holds a single value of a single type() at a
    time. (Some type()s are multi-valued, for example a string list.)
    You can find out what type, T, the variant holds, convert it to a
    different type using one of the asT() functions, e.g. asSize(),
    get its value using one of the toT() functions, e.g. toSize(), and
    check whether the type can be converted to a particular type using
    canCast().

    The methods named toT() (for any supported T, see the \c Type
    documentation for a list) are const. If you ask for the stored
    type, they return a copy of the stored object. If you ask for a
    type that can be generated from the stored type, toT() copies and
    converts and leaves the object itself unchanged. If you ask for a
    type that cannot be generated from the stored type, the result
    depends on the type (see the function documentation for details).

    Note that three data types supported by QVariant are explicitly
    shared, namely QImage, QPointArray, and QCString, and in these
    cases the toT() methods return a shallow copy. In almost all cases
    you must make a deep copy of the returned values before modifying
    them.

    The asT() functions are not const. They do conversion like the
    toT() methods, set the variant to hold the converted value, and
    return a reference to the new contents of the variant.

    Here is some example code to demonstrate the use of QVariant:

    \code
    QDataStream out(...);
    QVariant v(123);          // The variant now contains an int
    int x = v.toInt();        // x = 123
    out << v;                 // Writes a type tag and an int to out
    v = QVariant("hello");    // The variant now contains a QCString
    v = QVariant(tr("hello"));// The variant now contains a QString
    int y = v.toInt();        // y = 0 since v cannot be converted to an int
    QString s = v.toString(); // s = tr("hello")  (see QObject::tr())
    out << v;                 // Writes a type tag and a QString to out
    ...
    QDataStream in(...);      // (opening the previously written stream)
    in >> v;                  // Reads an Int variant
    int z = v.toInt();        // z = 123
    qDebug("Type is %s",      // prints "Type is int"
	    v.typeName());
    v.asInt() += 100;	      // The variant now hold the value 223.
    v = QVariant( QStringList() );
    v.asStringList().append( "Hello" );
    \endcode

    You can even store QValueList<QVariant>s and
    QMap<QString,QVariant>s in a variant, so you can easily construct
    arbitrarily complex data structures of arbitrary types. This is
    very powerful and versatile, but may prove less memory and speed
    efficient than storing specific types in standard data structures.

    QVariant also supports the notion of NULL values, where you have a
    defined type with no value set.
    \code
    QVariant x, y( QString() ), z( QString("") );
    x.asInt();
    // x.isNull() == TRUE, y.isNull() == TRUE, z.isNull() == FALSE
    \endcode

    See the \link collection.html Collection Classes\endlink.
*/

/*!
    \enum QVariant::Type

    This enum type defines the types of variable that a QVariant can
    contain.

    \value Invalid  no type
    \value BitArray  a QBitArray
    \value ByteArray  a QByteArray
    \value Bitmap  a QBitmap
    \value Bool  a bool
    \value Brush  a QBrush
    \value Color  a QColor
    \value ColorGroup  a QColorGroup
    \value Cursor  a QCursor
    \value Date  a QDate
    \value DateTime  a QDateTime
    \value Double  a double
    \value Font  a QFont
    \value IconSet  a QIconSet
    \value Image  a QImage
    \value Int  an int
    \value KeySequence  a QKeySequence
    \value List  a QValueList<QVariant>
    \value LongLong a long long
    \value ULongLong an unsigned long long
    \value Map  a QMap<QString,QVariant>
    \value Palette  a QPalette
    \value Pen  a QPen
    \value Pixmap  a QPixmap
    \value Point  a QPoint
    \value PointArray  a QPointArray
    \value Rect  a QRect
    \value Region  a QRegion
    \value Size  a QSize
    \value SizePolicy  a QSizePolicy
    \value String  a QString
    \value CString  a QCString
    \value StringList  a QStringList
    \value Time  a QTime
    \value UInt  an unsigned int

    Note that Qt's definition of bool depends on the compiler.
    \c qglobal.h has the system-dependent definition of bool.
*/

/*!
    Constructs an invalid variant.
*/
QVariant::QVariant()
{
    d = new Private;
}

/*!
    Destroys the QVariant and the contained object.

    Note that subclasses that reimplement clear() should reimplement
    the destructor to call clear(). This destructor calls clear(), but
    because it is the destructor, QVariant::clear() is called rather
    than a subclass's clear().
*/
QVariant::~QVariant()
{
    if ( d->deref() )
	delete d;
}

/*!
    Constructs a copy of the variant, \a p, passed as the argument to
    this constructor. Usually this is a deep copy, but a shallow copy
    is made if the stored data type is explicitly shared, as e.g.
    QImage is.
*/
QVariant::QVariant( const QVariant& p )
{
    p.d->ref();
    d = p.d;
}

#ifndef QT_NO_DATASTREAM
/*!
    Reads the variant from the data stream, \a s.
*/
QVariant::QVariant( QDataStream& s )
{
    d = new Private;
    s >> *this;
}
#endif //QT_NO_DATASTREAM

/*!
    Constructs a new variant with a string value, \a val.
*/
QVariant::QVariant( const QString& val )
{
    d = new Private;
    d->typ = String;
    d->value.ptr = new QString( val );
}

/*!
    Constructs a new variant with a C-string value, \a val.

    If you want to modify the QCString after you've passed it to this
    constructor, we recommend passing a deep copy (see
    QCString::copy()).
*/
QVariant::QVariant( const QCString& val )
{
    d = new Private;
    d->typ = CString;
    d->value.ptr = new QCString( val );
}

/*!
    Constructs a new variant with a C-string value of \a val if \a val
    is non-null. The variant creates a deep copy of \a val.

    If \a val is null, the resulting variant has type Invalid.
*/
QVariant::QVariant( const char* val )
{
    d = new Private;
    if ( val == 0 )
	return;
    d->typ = CString;
    d->value.ptr = new QCString( val );
}

#ifndef QT_NO_STRINGLIST
/*!
    Constructs a new variant with a string list value, \a val.
*/
QVariant::QVariant( const QStringList& val )
{
    d = new Private;
    d->typ = StringList;
    d->value.ptr = new QStringList( val );
    d->is_null = FALSE;
}
#endif // QT_NO_STRINGLIST

#ifndef QT_NO_TEMPLATE_VARIANT
/*!
    Constructs a new variant with a map of QVariants, \a val.
*/
QVariant::QVariant( const QMap<QString,QVariant>& val )
{
    d = new Private;
    d->typ = Map;
    d->value.ptr = new QMap<QString,QVariant>( val );
    d->is_null = FALSE;
}
#endif

/*!
    Constructs a new variant with a date value, \a val.
*/
QVariant::QVariant( const QDate& val )
{
    d = new Private;
    d->typ = Date;
    d->value.ptr = new QDate( val );
}

/*!
    Constructs a new variant with a time value, \a val.
*/
QVariant::QVariant( const QTime& val )
{
    d = new Private;
    d->typ = Time;
    d->value.ptr = new QTime( val );
}

/*!
    Constructs a new variant with a date/time value, \a val.
*/
QVariant::QVariant( const QDateTime& val )
{
    d = new Private;
    d->typ = DateTime;
    d->value.ptr = new QDateTime( val );
}

/*!
    Constructs a new variant with a bytearray value, \a val.
*/
QVariant::QVariant( const QByteArray& val )
{
    d = new Private;
    d->typ = ByteArray;
    d->value.ptr = new QByteArray( val );
}

/*!
    Constructs a new variant with a bitarray value, \a val.
*/
QVariant::QVariant( const QBitArray& val )
{
    d = new Private;
    d->typ = BitArray;
    d->value.ptr = new QBitArray( val );
}


/*!
    Constructs a new variant with an integer value, \a val.
*/
QVariant::QVariant( int val )
{
    d = new Private;
    d->typ = Int;
    d->value.i = val;
    d->is_null = FALSE;
}

/*!
    Constructs a new variant with an unsigned integer value, \a val.
*/
QVariant::QVariant( uint val )
{
    d = new Private;
    d->typ = UInt;
    d->value.u = val;
    d->is_null = FALSE;
}

/*!
    Constructs a new variant with a long long integer value, \a val.
*/
QVariant::QVariant( Q_LLONG val )
{
    d = new Private;
    d->typ = LongLong;
    d->value.ll = val;
    d->is_null = FALSE;
}

/*!
    Constructs a new variant with an unsigned long long integer value, \a val.
*/

QVariant::QVariant( Q_ULLONG val )
{
    d = new Private;
    d->typ = ULongLong;
    d->value.ull = val;
    d->is_null = FALSE;
}

/*!
    Constructs a new variant with a boolean value, \a val. The integer
    argument is a dummy, necessary for compatibility with some
    compilers.
*/
QVariant::QVariant( bool val, int )
{ // this is the comment that does NOT name said compiler.
    d = new Private;
    d->typ = Bool;
    d->value.b = val;
    d->is_null = FALSE;
}


/*!
    Constructs a new variant with a floating point value, \a val.
*/
QVariant::QVariant( double val )
{
    d = new Private;
    d->typ = Double;
    d->value.d = val;
    d->is_null = FALSE;
}

#ifndef QT_NO_TEMPLATE_VARIANT
/*!
    Constructs a new variant with a list value, \a val.
*/
QVariant::QVariant( const QValueList<QVariant>& val )
{
    d = new Private;
    d->typ = List;
    d->value.ptr = new QValueList<QVariant>( val );
    d->is_null = FALSE;
}
#endif


/*!
    Assigns the value of the variant \a variant to this variant.

    This is a deep copy of the variant, but note that if the variant
    holds an explicitly shared type such as QImage, a shallow copy is
    performed.
*/
QVariant& QVariant::operator= ( const QVariant& variant )
{
    QVariant& other = (QVariant&)variant;

    other.d->ref();
    if ( d->deref() )
	delete d;

    d = other.d;

    return *this;
}

/*!
    \internal
*/
void QVariant::detach()
{
    if ( d->count == 1 )
	return;

    d->deref();
    d = new Private( d );
}

/*!
    Returns the name of the type stored in the variant. The returned
    strings describe the C++ datatype used to store the data: for
    example, "QFont", "QString", or "QValueList<QVariant>". An Invalid
    variant returns 0.
*/
const char* QVariant::typeName() const
{
    return typeToName( (Type) d->typ );
}

/*!
    Convert this variant to type Invalid and free up any resources
    used.
*/
void QVariant::clear()
{
    if ( d->count > 1 )
    {
	d->deref();
	d = new Private;
	return;
    }

    d->clear();
}

/* Attention!

   For dependency reasons, this table is duplicated in moc.y. If you
   change one, change both.

   (Search for the word 'Attention' in moc.y.)
*/
static const int ntypes = 17;
static const char* const type_map[ntypes] =
{
    0,
    "QMap<QString,QVariant>",
    "QValueList<QVariant>",
    "QString",
    "QStringList",
    "int",
    "uint",
    "bool",
    "double",
    "QCString",
    "QDate",
    "QTime",
    "QDateTime",
    "QByteArray",
    "QBitArray",
    "Q_LLONG",
    "Q_ULLONG"
};


/*!
    Converts the enum representation of the storage type, \a typ, to
    its string representation.
*/
const char* QVariant::typeToName( Type typ )
{
    if ( typ >= ntypes )
	return 0;
    return type_map[typ];
}


/*!
    Converts the string representation of the storage type gven in \a
    name, to its enum representation.

    If the string representation cannot be converted to any enum
    representation, the variant is set to \c Invalid.
*/
QVariant::Type QVariant::nameToType( const char* name )
{
    for ( int i = 0; i < ntypes; i++ ) {
	if ( !qstrcmp( type_map[i], name ) )
	    return (Type) i;
    }
    return Invalid;
}

#ifndef QT_NO_DATASTREAM
/*!
    Internal function for loading a variant from stream \a s. Use the
    stream operators instead.

    \internal
*/
void QVariant::load( QDataStream& s )
{
    Q_UINT32 u;
    s >> u;
    Type t = (Type)u;

    switch( t ) {
    case Invalid:
	{
	    // Since we wrote something, we should read something
	    QString x;
	    s >> x;
	    d->typ = t;
	    d->is_null = TRUE;
	}
	break;
#ifndef QT_NO_TEMPLATE_VARIANT
    case Map:
	{
	    QMap<QString,QVariant>* x = new QMap<QString,QVariant>;
	    s >> *x;
	    d->value.ptr = x;
	    d->is_null = FALSE;
	}
	break;
    case List:
	{
	    QValueList<QVariant>* x = new QValueList<QVariant>;
	    s >> *x;
	    d->value.ptr = x;
	    d->is_null = FALSE;
	}
	break;
#endif
    case String:
	{
	    QString* x = new QString;
	    s >> *x;
	    d->value.ptr = x;
	}
	break;
    case CString:
	{
	    QCString* x = new QCString;
	    s >> *x;
	    d->value.ptr = x;
	}
	break;
#ifndef QT_NO_STRINGLIST
    case StringList:
	{
	    QStringList* x = new QStringList;
	    s >> *x;
	    d->value.ptr = x;
	    d->is_null = FALSE;
	}
	break;
#endif // QT_NO_STRINGLIST
    case Int:
	{
	    int x;
	    s >> x;
	    d->value.i = x;
	    d->is_null = FALSE;
	}
	break;
    case UInt:
	{
	    uint x;
	    s >> x;
	    d->value.u = x;
	    d->is_null = FALSE;
	}
	break;
    case LongLong:
	{
	    Q_LLONG x;
	    s >> x;
	    d->value.ll = x;
	}
	break;
    case ULongLong:
	{
	    Q_ULLONG x;
	    s >> x;
	    d->value.ull = x;
	}
	break;
    case Bool:
	{
	    Q_INT8 x;
	    s >> x;
	    d->value.b = x;
	    d->is_null = FALSE;
	}
	break;
    case Double:
	{
	    double x;
	    s >> x;
	    d->value.d = x;
	    d->is_null = FALSE;
	}
	break;
    case Date:
	{
	    QDate* x = new QDate;
	    s >> *x;
	    d->value.ptr = x;
	}
	break;
    case Time:
	{
	    QTime* x = new QTime;
	    s >> *x;
	    d->value.ptr = x;
	}
	break;
    case DateTime:
	{
	    QDateTime* x = new QDateTime;
	    s >> *x;
	    d->value.ptr = x;
	}
	break;
    case ByteArray:
	{
	    QByteArray* x = new QByteArray;
	    s >> *x;
	    d->value.ptr = x;
	}
	break;
    case BitArray:
	{
	    QBitArray* x = new QBitArray;
	    s >> *x;
	    d->value.ptr = x;
	}
	break;
    }
    d->typ = t;
}

/*!
    Internal function for saving a variant to the stream \a s. Use the
    stream operators instead.

    \internal
*/
void QVariant::save( QDataStream& s ) const
{
    s << (Q_UINT32)type();

    switch( d->typ ) {
#ifndef QT_NO_TEMPLATE_VARIANT
    case List:
	s << *((QValueList<QVariant>*)d->value.ptr);
	break;
    case Map:
	s << *((QMap<QString,QVariant>*)d->value.ptr);
	break;
#endif
    case String:
	s << *((QString*)d->value.ptr);
	break;
    case CString:
	s << *((QCString*)d->value.ptr);
	break;
#ifndef QT_NO_STRINGLIST
    case StringList:
	s << *((QStringList*)d->value.ptr);
	break;
#endif
    case Int:
	s << d->value.i;
	break;
    case UInt:
	s << d->value.u;
	break;
    case LongLong:
	s << d->value.ll;
	break;
    case ULongLong:
	s << d->value.ull;
	break;
    case Bool:
	s << (Q_INT8)d->value.b;
	break;
    case Double:
	s << d->value.d;
	break;
    case Date:
	s << *((QDate*)d->value.ptr);
	break;
    case Time:
	s << *((QTime*)d->value.ptr);
	break;
    case DateTime:
	s << *((QDateTime*)d->value.ptr);
	break;
    case ByteArray:
	s << *((QByteArray*)d->value.ptr);
	break;
    case BitArray:
	s << *((QBitArray*)d->value.ptr);
	break;
    case Invalid:
	s << QString(); // ### looks wrong.
	break;
    }
}

/*!
    Reads a variant \a p from the stream \a s.

    \sa \link datastreamformat.html Format of the QDataStream
    operators \endlink
*/
QDataStream& operator>> ( QDataStream& s, QVariant& p )
{
    p.load( s );
    return s;
}

/*!
    Writes a variant \a p to the stream \a s.

    \sa \link datastreamformat.html Format of the QDataStream
    operators \endlink
*/
QDataStream& operator<< ( QDataStream& s, const QVariant& p )
{
    p.save( s );
    return s;
}

/*!
    Reads a variant type \a p in enum representation from the stream \a s.
*/
QDataStream& operator>> ( QDataStream& s, QVariant::Type& p )
{
    Q_UINT32 u;
    s >> u;
    p = (QVariant::Type) u;

    return s;
}

/*!
    Writes a variant type \a p to the stream \a s.
*/
QDataStream& operator<< ( QDataStream& s, const QVariant::Type p )
{
    s << (Q_UINT32)p;

    return s;
}

#endif //QT_NO_DATASTREAM

/*!
    \fn Type QVariant::type() const

    Returns the storage type of the value stored in the variant.
    Usually it's best to test with canCast() whether the variant can
    deliver the data type you are interested in.
*/

/*!
    \fn bool QVariant::isValid() const

    Returns TRUE if the storage type of this variant is not
    QVariant::Invalid; otherwise returns FALSE.
*/

/*!
    \fn QValueListConstIterator<QString> QVariant::stringListBegin() const
    \obsolete

    Returns an iterator to the first string in the list if the
    variant's type is StringList; otherwise returns a null iterator.
*/

/*!
    \fn QValueListConstIterator<QString> QVariant::stringListEnd() const
    \obsolete

    Returns the end iterator for the list if the variant's type is
    StringList; otherwise returns a null iterator.
*/

/*!
    \fn QValueListConstIterator<QVariant> QVariant::listBegin() const
    \obsolete

    Returns an iterator to the first item in the list if the variant's
    type is appropriate; otherwise returns a null iterator.
*/

/*!
    \fn QValueListConstIterator<QVariant> QVariant::listEnd() const
    \obsolete

    Returns the end iterator for the list if the variant's type is
    appropriate; otherwise returns a null iterator.
*/

/*!
    \fn QMapConstIterator<QString, QVariant> QVariant::mapBegin() const
    \obsolete

    Returns an iterator to the first item in the map, if the variant's
    type is appropriate; otherwise returns a null iterator.
*/

/*!
    \fn QMapConstIterator<QString, QVariant> QVariant::mapEnd() const
    \obsolete

    Returns the end iterator for the map, if the variant's type is
    appropriate; otherwise returns a null iterator.
*/

/*!
    \fn QMapConstIterator<QString, QVariant> QVariant::mapFind( const QString& key ) const
    \obsolete

    Returns an iterator to the item in the map with \a key as key, if
    the variant's type is appropriate and \a key is a valid key;
    otherwise returns a null iterator.
*/

/*!
    Returns the variant as a QString if the variant has type() String,
    CString, ByteArray, Int, Uint, Bool, Double, Date, Time, DateTime,
    KeySequence, Font or Color; otherwise returns QString::null.

    \sa asString()
*/
const QString QVariant::toString() const
{
    switch( d->typ ) {
    case CString:
	return QString::fromLatin1( toCString() );
    case Int:
	return QString::number( toInt() );
    case UInt:
	return QString::number( toUInt() );
    case LongLong:
	return QString::number( toLongLong() );
    case ULongLong:
	return QString::number( toULongLong() );
    case Double:
	return QString::number( toDouble(), 'g', DBL_DIG );
#if !defined(QT_NO_SPRINTF) && !defined(QT_NO_DATESTRING)
    case Date:
	return toDate().toString( Qt::ISODate );
    case Time:
	return toTime().toString( Qt::ISODate );
    case DateTime:
	return toDateTime().toString( Qt::ISODate );
#endif
    case Bool:
	return toInt() ? "true" : "false";
    case ByteArray:
	return QString( *((QByteArray*)d->value.ptr) );
    case String:
	return *((QString*)d->value.ptr);
    default:
	return QString::null;
    }
}
/*!
    Returns the variant as a QCString if the variant has type()
    CString or String; otherwise returns 0.

    \sa asCString()
*/
const QCString QVariant::toCString() const
{
    switch ( d->typ ) {
    case CString:
	return *((QCString*)d->value.ptr);
    case String:
	return ((QString*)d->value.ptr)->latin1();
    default:
	return 0;
    }
}


#ifndef QT_NO_STRINGLIST
/*!
    Returns the variant as a QStringList if the variant has type()
    StringList or List of a type that can be converted to QString;
    otherwise returns an empty list.

    Note that if you want to iterate over the list, you should iterate
    over a copy, e.g.
    \code
    QStringList list = myVariant.toStringList();
    QStringList::Iterator it = list.begin();
    while( it != list.end() ) {
	myProcessing( *it );
	++it;
    }
    \endcode

    \sa asStringList()
*/
const QStringList QVariant::toStringList() const
{
    switch ( d->typ ) {
    case StringList:
	return *((QStringList*)d->value.ptr);
#ifndef QT_NO_TEMPLATE_VARIANT
    case List:
	{
	    QStringList lst;
	    QValueList<QVariant>::ConstIterator it = listBegin();
	    QValueList<QVariant>::ConstIterator end = listEnd();
	    while( it != end ) {
		QString tmp = (*it).toString();
		++it;
		lst.append( tmp );
	    }
	    return lst;
	}
#endif
    default:
	return QStringList();
    }
}
#endif //QT_NO_STRINGLIST

#ifndef QT_NO_TEMPLATE_VARIANT
/*!
    Returns the variant as a QMap<QString,QVariant> if the variant has
    type() Map; otherwise returns an empty map.

    Note that if you want to iterate over the map, you should iterate
    over a copy, e.g.
    \code
    QMap<QString, QVariant> map = myVariant.toMap();
    QMap<QString, QVariant>::Iterator it = map.begin();
    while( it != map.end() ) {
	myProcessing( *it );
	++it;
    }
    \endcode

    \sa asMap()
*/
const QMap<QString, QVariant> QVariant::toMap() const
{
    if ( d->typ != Map )
	return QMap<QString,QVariant>();

    return *((QMap<QString,QVariant>*)d->value.ptr);
}
#endif

/*!
    Returns the variant as a QDate if the variant has type() Date,
    DateTime or String; otherwise returns an invalid date.

    Note that if the type() is String an invalid date will be returned
    if the string cannot be parsed as a Qt::ISODate format date.

    \sa asDate()
*/
const QDate QVariant::toDate() const
{
    switch ( d->typ ) {
    case Date:
	return *((QDate*)d->value.ptr);
    case DateTime:
	return ((QDateTime*)d->value.ptr)->date();
#ifndef QT_NO_DATESTRING
    case String:
	return QDate::fromString( *((QString*)d->value.ptr), Qt::ISODate );
#endif
    default:
	return QDate();
    }
}

/*!
    Returns the variant as a QTime if the variant has type() Time,
    DateTime or String; otherwise returns an invalid time.

    Note that if the type() is String an invalid time will be returned
    if the string cannot be parsed as a Qt::ISODate format time.

    \sa asTime()
*/
const QTime QVariant::toTime() const
{
    switch ( d->typ ) {
    case Time:
	return *((QTime*)d->value.ptr);
    case DateTime:
	return ((QDateTime*)d->value.ptr)->time();
#ifndef QT_NO_DATESTRING
    case String:
	return QTime::fromString( *((QString*)d->value.ptr), Qt::ISODate );
#endif
    default:
	return QTime();
    }
}

/*!
    Returns the variant as a QDateTime if the variant has type()
    DateTime, Date or String; otherwise returns an invalid date/time.

    Note that if the type() is String an invalid date/time will be
    returned if the string cannot be parsed as a Qt::ISODate format
    date/time.

    \sa asDateTime()
*/
const QDateTime QVariant::toDateTime() const
{
    switch ( d->typ ) {
    case DateTime:
	return *((QDateTime*)d->value.ptr);
#ifndef QT_NO_DATESTRING
    case String:
	return QDateTime::fromString( *((QString*)d->value.ptr), Qt::ISODate );
#endif
    case Date:
	return QDateTime( *((QDate*)d->value.ptr) );
    default:
	return QDateTime();
    }
}

/*!
    Returns the variant as a QByteArray if the variant has type()
    ByteArray or CString; otherwise returns an empty bytearray.

    \sa asByteArray()
*/
const QByteArray QVariant::toByteArray() const
{
    if ( d->typ == ByteArray )
	return *((QByteArray*)d->value.ptr);
    if ( d->typ == CString )
	return *((QByteArray*)d->value.ptr);
    return QByteArray();
}

/*!
    Returns the variant as a QBitArray if the variant has type()
    BitArray; otherwise returns an empty bitarray.

    \sa asBitArray()
*/
const QBitArray QVariant::toBitArray() const
{
    if ( d->typ == BitArray )
	return *((QBitArray*)d->value.ptr);
    return QBitArray();
}


/*!
    Returns the variant as an int if the variant has type() String,
    CString, Int, UInt, Double, Bool or KeySequence; otherwise returns
    0.

    If \a ok is non-null: \a *ok is set to TRUE if the value could be
    converted to an int; otherwise \a *ok is set to FALSE.

    \sa asInt() canCast()
*/
int QVariant::toInt( bool * ok ) const
{
    if ( ok )
	*ok = canCast( Int );

    switch ( d->typ ) {
    case String:
	return ((QString*)d->value.ptr)->toInt( ok );
    case CString:
	return ((QCString*)d->value.ptr)->toInt( ok );
    case Int:
	return d->value.i;
    case UInt:
	return (int)d->value.u;
    case LongLong:
        return (int)d->value.ll;
    case ULongLong:
        return (int)d->value.ull;
    case Double:
	return (int)d->value.d;
    case Bool:
	return (int)d->value.b;
    default:
	return 0;
    }
}

/*!
    Returns the variant as an unsigned int if the variant has type()
    String, CString, UInt, Int, Double, or Bool; otherwise returns 0.

    If \a ok is non-null: \a *ok is set to TRUE if the value could be
    converted to an unsigned int; otherwise \a *ok is set to FALSE.

    \sa asUInt()
*/
uint QVariant::toUInt( bool * ok ) const
{
    if ( ok )
	*ok = canCast( UInt );

    switch( d->typ ) {
    case String:
	return ((QString*)d->value.ptr)->toUInt( ok );
    case CString:
	return ((QCString*)d->value.ptr)->toUInt( ok );
    case Int:
	return (uint)d->value.i;
    case UInt:
	return d->value.u;
    case LongLong:
        return (uint)d->value.ll;
    case ULongLong:
        return (uint)d->value.ull;
    case Double:
	return (uint)d->value.d;
    case Bool:
	return (uint)d->value.b;
    default:
	return 0;
    }
}

/*!
    Returns the variant as a long long int if the variant has type()
    LongLong, ULongLong, any type allowing a toInt() conversion;
    otherwise returns 0.

    If \a ok is non-null: \a *ok is set to TRUE if the value could be
    converted to an int; otherwise \a *ok is set to FALSE.

    \sa asLongLong() canCast()
*/
Q_LLONG QVariant::toLongLong( bool * ok ) const
{
    if ( ok )
	*ok = canCast( LongLong );

    switch ( d->typ ) {
    case String:
	return ((QString*)d->value.ptr)->toLongLong( ok );
    case CString:
	return ((QCString*)d->value.ptr)->toInt( ok ); // ### LongLong
    case Int:
	return (Q_LLONG)d->value.i;
    case UInt:
	return (Q_LLONG)d->value.u;
    case LongLong:
	return d->value.ll;
    case ULongLong:
	return (Q_LLONG)d->value.ull;
    case Double:
	return (Q_LLONG)d->value.d;
    case Bool:
	return (Q_LLONG)d->value.b;
    default:
	return 0;
    }
}

/*!
    Returns the variant as as an unsigned long long int if the variant
    has type() LongLong, ULongLong, any type allowing a toUInt()
    conversion; otherwise returns 0.

    If \a ok is non-null: \a *ok is set to TRUE if the value could be
    converted to an int; otherwise \a *ok is set to FALSE.

    \sa asULongLong() canCast()
*/
Q_ULLONG QVariant::toULongLong( bool * ok ) const
{
    if ( ok )
	*ok = canCast( ULongLong );

    switch ( d->typ ) {
    case Int:
	return (Q_ULLONG)d->value.i;
    case UInt:
	return (Q_ULLONG)d->value.u;
    case LongLong:
	return (Q_ULLONG)d->value.ll;
    case ULongLong:
	return d->value.ull;
    case Double:
	return (Q_ULLONG)d->value.d;
    case Bool:
	return (Q_ULLONG)d->value.b;
    case String:
	return ((QString*)d->value.ptr)->toULongLong( ok );
    case CString:
	// ###	return ((QCString*)d->value.ptr)->toULongLong( ok );
    default:
	return 0;
    }
}

/*!
    Returns the variant as a bool if the variant has type() Bool.

    Returns TRUE if the variant has type Int, UInt or Double and its
    value is non-zero, or if the variant has type String and its lower-case
    content is not empty, "0" or "false"; otherwise returns FALSE.

    \sa asBool()
*/
bool QVariant::toBool() const
{
    switch( d->typ ) {
    case Bool:
	return d->value.b;
    case Double:
	return d->value.d != 0.0;
    case Int:
	return d->value.i != 0;
    case UInt:
	return d->value.u != 0;
    case LongLong:
	return d->value.ll != 0;
    case ULongLong:
	return d->value.ull != 0;
    case String:
	{
	    QString str = toString().lower();
	    return !(str == "0" || str == "false" || str.isEmpty() );
	}
    default:
	return FALSE;
    }
}

/*!
    Returns the variant as a double if the variant has type() String,
    CString, Double, Int, UInt, LongLong, ULongLong or Bool; otherwise
    returns 0.0.

    If \a ok is non-null: \a *ok is set to TRUE if the value could be
    converted to a double; otherwise \a *ok is set to FALSE.

    \sa asDouble()
*/
double QVariant::toDouble( bool * ok ) const
{
    if ( ok )
	*ok = canCast( Double );

    switch ( d->typ ) {
    case String:
	return ((QString*)d->value.ptr)->toDouble( ok );
    case CString:
	return ((QCString*)d->value.ptr)->toDouble( ok );
    case Double:
	return d->value.d;
    case Int:
	return (double)d->value.i;
    case Bool:
	return (double)d->value.b;
    case UInt:
	return (double)d->value.u;
    case LongLong:
	return (double)d->value.ll;
    case ULongLong:
#if defined(Q_CC_MSVC) && !defined(Q_CC_MSVC_NET)
	return (double)(Q_LLONG)d->value.ull;
#else
	return (double)d->value.ull;
#endif
    default:
	return 0.0;
    }
}

#ifndef QT_NO_TEMPLATE_VARIANT
/*!
    Returns the variant as a QValueList<QVariant> if the variant has
    type() List or StringList; otherwise returns an empty list.

    Note that if you want to iterate over the list, you should iterate
    over a copy, e.g.
    \code
    QValueList<QVariant> list = myVariant.toList();
    QValueList<QVariant>::Iterator it = list.begin();
    while( it != list.end() ) {
	myProcessing( *it );
	++it;
    }
    \endcode

    \sa asList()
*/
const QValueList<QVariant> QVariant::toList() const
{
    if ( d->typ == List )
	return *((QValueList<QVariant>*)d->value.ptr);
#ifndef QT_NO_STRINGLIST
    if ( d->typ == StringList ) {
	QValueList<QVariant> lst;
	QStringList::ConstIterator it = stringListBegin();
	QStringList::ConstIterator end = stringListEnd();
	for( ; it != end; ++it )
	    lst.append( QVariant( *it ) );
	return lst;
    }
#endif //QT_NO_STRINGLIST
    return QValueList<QVariant>();
}
#endif



#define Q_VARIANT_AS( f ) Q##f& QVariant::as##f() { \
   if ( d->typ != f ) *this = QVariant( to##f() ); else detach(); return *((Q##f*)d->value.ptr);}

Q_VARIANT_AS(String)
Q_VARIANT_AS(CString)
#ifndef QT_NO_STRINGLIST
Q_VARIANT_AS(StringList)
#endif
Q_VARIANT_AS(Date)
Q_VARIANT_AS(Time)
Q_VARIANT_AS(DateTime)
Q_VARIANT_AS(ByteArray)
Q_VARIANT_AS(BitArray)

/*!
    \fn QString& QVariant::asString()

    Tries to convert the variant to hold a string value. If that is
    not possible the variant is set to an empty string.

    Returns a reference to the stored string.

    \sa toString()
*/

/*!
    \fn QCString& QVariant::asCString()

    Tries to convert the variant to hold a string value. If that is
    not possible the variant is set to an empty string.

    Returns a reference to the stored string.

    \sa toCString()
*/

/*!
    \fn QStringList& QVariant::asStringList()

    Tries to convert the variant to hold a QStringList value. If that
    is not possible the variant is set to an empty string list.

    Returns a reference to the stored string list.

    Note that if you want to iterate over the list, you should
    iterate over a copy, e.g.
    \code
    QStringList list = myVariant.asStringList();
    QStringList::Iterator it = list.begin();
    while( it != list.end() ) {
	myProcessing( *it );
	++it;
    }
    \endcode

    \sa toStringList()
*/

/*!
    \fn QFont& QVariant::asFont()

    Tries to convert the variant to hold a QFont. If that is not
    possible the variant is set to the application's default font.

    Returns a reference to the stored font.

    \sa toFont()
*/

/*!
    \fn QPixmap& QVariant::asPixmap()

    Tries to convert the variant to hold a pixmap value. If that is
    not possible the variant is set to a null pixmap.

    Returns a reference to the stored pixmap.

    \sa toPixmap()
*/

/*!
    \fn QImage& QVariant::asImage()

    Tries to convert the variant to hold an image value. If that is
    not possible the variant is set to a null image.

    Returns a reference to the stored image.

    \sa toImage()
*/

/*!
    \fn QBrush& QVariant::asBrush()

    Tries to convert the variant to hold a brush value. If that is not
    possible the variant is set to a default black brush.

    Returns a reference to the stored brush.

    \sa toBrush()
*/

/*!
    \fn QPoint& QVariant::asPoint()

    Tries to convert the variant to hold a point value. If that is not
    possible the variant is set to a (0, 0) point.

    Returns a reference to the stored point.

    \sa toPoint()
*/

/*!
    \fn QRect& QVariant::asRect()

    Tries to convert the variant to hold a rectangle value. If that is
    not possible the variant is set to an empty rectangle.

    Returns a reference to the stored rectangle.

    \sa toRect()
*/

/*!
    \fn QSize& QVariant::asSize()

    Tries to convert the variant to hold a QSize value. If that is not
    possible the variant is set to an invalid size.

    Returns a reference to the stored size.

    \sa toSize() QSize::isValid()
*/

/*!
    \fn QSizePolicy& QVariant::asSizePolicy()

    Tries to convert the variant to hold a QSizePolicy value. If that
    fails, the variant is set to an arbitrary (valid) size policy.
*/


/*!
    \fn QColor& QVariant::asColor()

    Tries to convert the variant to hold a QColor value. If that is
    not possible the variant is set to an invalid color.

    Returns a reference to the stored color.

    \sa toColor() QColor::isValid()
*/

/*!
    \fn QPalette& QVariant::asPalette()

    Tries to convert the variant to hold a QPalette value. If that is
    not possible the variant is set to a palette of black colors.

    Returns a reference to the stored palette.

    \sa toString()
*/

/*!
    \fn QColorGroup& QVariant::asColorGroup()

    Tries to convert the variant to hold a QColorGroup value. If that
    is not possible the variant is set to a color group of all black
    colors.

    Returns a reference to the stored color group.

    \sa toColorGroup()
*/

/*!
    \fn QIconSet& QVariant::asIconSet()

    Tries to convert the variant to hold a QIconSet value. If that is
    not possible the variant is set to an empty iconset.

    Returns a reference to the stored iconset.

    \sa toIconSet()
*/

/*!
    \fn QPointArray& QVariant::asPointArray()

    Tries to convert the variant to hold a QPointArray value. If that
    is not possible the variant is set to an empty point array.

    Returns a reference to the stored point array.

    \sa toPointArray()
*/

/*!
    \fn QBitmap& QVariant::asBitmap()

    Tries to convert the variant to hold a bitmap value. If that is
    not possible the variant is set to a null bitmap.

    Returns a reference to the stored bitmap.

    \sa toBitmap()
*/

/*!
    \fn QRegion& QVariant::asRegion()

    Tries to convert the variant to hold a QRegion value. If that is
    not possible the variant is set to a null region.

    Returns a reference to the stored region.

    \sa toRegion()
*/

/*!
    \fn QCursor& QVariant::asCursor()

    Tries to convert the variant to hold a QCursor value. If that is
    not possible the variant is set to a default arrow cursor.

    Returns a reference to the stored cursor.

    \sa toCursor()
*/

/*!
    \fn QDate& QVariant::asDate()

    Tries to convert the variant to hold a QDate value. If that is not
    possible then the variant is set to an invalid date.

    Returns a reference to the stored date.

    \sa toDate()
*/

/*!
    \fn QTime& QVariant::asTime()

    Tries to convert the variant to hold a QTime value. If that is not
    possible then the variant is set to an invalid time.

    Returns a reference to the stored time.

    \sa toTime()
*/

/*!
    \fn QDateTime& QVariant::asDateTime()

    Tries to convert the variant to hold a QDateTime value. If that is
    not possible then the variant is set to an invalid date/time.

    Returns a reference to the stored date/time.

    \sa toDateTime()
*/

/*!
    \fn QByteArray& QVariant::asByteArray()

    Tries to convert the variant to hold a QByteArray value. If that
    is not possible then the variant is set to an empty bytearray.

    Returns a reference to the stored bytearray.

    \sa toByteArray()
*/

/*!
    \fn QBitArray& QVariant::asBitArray()

    Tries to convert the variant to hold a QBitArray value. If that is
    not possible then the variant is set to an empty bitarray.

    Returns a reference to the stored bitarray.

    \sa toBitArray()
*/

/*!
    \fn QKeySequence& QVariant::asKeySequence()

    Tries to convert the variant to hold a QKeySequence value. If that
    is not possible then the variant is set to an empty key sequence.

    Returns a reference to the stored key sequence.

    \sa toKeySequence()
*/

/*! \fn QPen& QVariant::asPen()

  Tries to convert the variant to hold a QPen value. If that
  is not possible then the variant is set to an empty pen.

  Returns a reference to the stored pen.

  \sa toPen()
*/

/*!
    Returns the variant's value as int reference.
*/
int& QVariant::asInt()
{
    if ( d->typ != Int ) {
	detach();
	int i = toInt();
	d->clear();
 	d->value.i = i;
	d->typ = Int;
    }
    return d->value.i;
}

/*!
    Returns the variant's value as unsigned int reference.
*/
uint& QVariant::asUInt()
{
    if ( d->typ != UInt ) {
	detach();
	uint u = toUInt();
	d->clear();
	d->value.u = u;
	d->typ = UInt;
    }
    return d->value.u;
}

/*!
    Returns the variant's value as long long reference.
*/
Q_LLONG& QVariant::asLongLong()
{
    if ( d->typ != LongLong ) {
	detach();
	Q_LLONG ll = toLongLong();
	d->clear();
 	d->value.ll = ll;
	d->typ = LongLong;
    }
    return d->value.ll;
}

/*!
    Returns the variant's value as unsigned long long reference.
*/
Q_ULLONG& QVariant::asULongLong()
{
    if ( d->typ != ULongLong ) {
	detach();
	Q_ULLONG ull = toULongLong();
	d->clear();
 	d->value.ull = ull;
	d->typ = ULongLong;
    }
    return d->value.ull;
}

/*!
    Returns the variant's value as bool reference.
*/
bool& QVariant::asBool()
{
    if ( d->typ != Bool ) {
	detach();
	bool b = toBool();
	d->clear();
	d->value.b = b;
	d->typ = Bool;
    }
    return d->value.b;
}

/*!
    Returns the variant's value as double reference.
*/
double& QVariant::asDouble()
{
    if ( d->typ != Double ) {
	detach();
	double dbl = toDouble();
	d->clear();
	d->value.d = dbl;
	d->typ = Double;
    }
    return d->value.d;
}

#ifndef QT_NO_TEMPLATE_VARIANT
/*!
    Returns the variant's value as variant list reference.

    Note that if you want to iterate over the list, you should iterate
    over a copy, e.g.
    \code
    QValueList<QVariant> list = myVariant.asList();
    QValueList<QVariant>::Iterator it = list.begin();
    while( it != list.end() ) {
	myProcessing( *it );
	++it;
    }
    \endcode
*/
QValueList<QVariant>& QVariant::asList()
{
    if ( d->typ != List )
	*this = QVariant( toList() );
    return *((QValueList<QVariant>*)d->value.ptr);
}

/*!
    Returns the variant's value as variant map reference.

    Note that if you want to iterate over the map, you should iterate
    over a copy, e.g.
    \code
    QMap<QString, QVariant> map = myVariant.asMap();
    QMap<QString, QVariant>::Iterator it = map.begin();
    while( it != map.end() ) {
	myProcessing( *it );
	++it;
    }
    \endcode
*/
QMap<QString, QVariant>& QVariant::asMap()
{
    if ( d->typ != Map )
	*this = QVariant( toMap() );
    return *((QMap<QString,QVariant>*)d->value.ptr);
}
#endif

/*!
    Returns TRUE if the variant's type can be cast to the requested
    type, \a t. Such casting is done automatically when calling the
    toInt(), toBool(), ... or asInt(), asBool(), ... methods.

    The following casts are done automatically:
    \table
    \header \i Type \i Automatically Cast To
    \row \i Bool \i Double, Int, UInt, LongLong, ULongLong
    \row \i Color \i String
    \row \i Date \i String, DateTime
    \row \i DateTime \i String, Date, Time
    \row \i Double \i String, Int, Bool, UInt
    \row \i Font \i String
    \row \i Int \i String, Double, Bool, UInt
    \row \i List \i StringList (if the list contains strings or
    something that can be cast to a string)
    \row \i String \i CString, Int, Uint, Bool, Double, Date,
    Time, DateTime, KeySequence, Font, Color
    \row \i CString \i String
    \row \i StringList \i List
    \row \i Time \i String
    \row \i UInt \i String, Double, Bool, Int
    \row \i KeySequence \i String, Int
    \endtable
*/
bool QVariant::canCast( Type t ) const
{
    if ( Type( d->typ ) == t )
	return TRUE;
    switch ( t ) {
    case Bool:
	return d->typ == Double || d->typ == Int || d->typ == UInt ||
	       d->typ == LongLong || d->typ == ULongLong || d->typ == String;
    case Int:
	return d->typ == String || d->typ == Double || d->typ == Bool ||
        d->typ == UInt || d->typ == LongLong || d->typ == ULongLong ;
    case UInt:
	return d->typ == String || d->typ == Double || d->typ == Bool ||
	       d->typ == Int || d->typ == LongLong || d->typ == ULongLong;
    case LongLong:
	return d->typ == String || d->typ == Double || d->typ == Bool ||
	       d->typ == Int || d->typ == UInt || d->typ == ULongLong;
    case ULongLong:
	return d->typ == String || d->typ == Double || d->typ == Bool ||
	       d->typ == Int || d->typ == UInt || d->typ == LongLong;
    case Double:
	return d->typ == String || d->typ == Int || d->typ == Bool || d->typ == UInt;
    case CString:
	return d->typ == String;
    case String:
	return d->typ == CString || d->typ == ByteArray || d->typ == Int || d->typ == UInt || d->typ == Bool || d->typ == Double || d->typ == Date || d->typ == Time || d->typ == DateTime || d->typ == LongLong || d->typ == ULongLong;
    case ByteArray:
	return d->typ == CString;
    case Date:
	return d->typ == String || d->typ == DateTime;
    case Time:
	return d->typ == String || d->typ == DateTime;
    case DateTime:
	return d->typ == String || d->typ == Date;
#ifndef QT_NO_STRINGLIST
    case List:
	return d->typ == StringList;
#endif
#ifndef QT_NO_TEMPLATE_VARIANT
    case StringList:
	if ( d->typ == List ) {
	    QValueList<QVariant>::ConstIterator it = listBegin();
	    QValueList<QVariant>::ConstIterator end = listEnd();
	    for( ; it != end; ++it ) {
		if ( !(*it).canCast( String ) )
		    return FALSE;
	    }
	    return TRUE;
	}
	return FALSE;
#endif
    default:
	return FALSE;
    }
}

/*!
    Casts the variant to the requested type. If the cast cannot be
    done, the variant is set to the default value of the requested
    type (e.g. an empty string if the requested type \a t is
    QVariant::String, an empty point array if the requested type \a t
    is QVariant::PointArray, etc). Returns TRUE if the current type of
    the variant was successfully cast; otherwise returns FALSE.

    \sa canCast()
*/

bool QVariant::cast( Type t )
{
    switch ( t ) {
#ifndef QT_NO_TEMPLATE_VARIANT
    case QVariant::Map:
	asMap();
	break;
    case QVariant::List:
	asList();
	break;
#endif
    case QVariant::String:
	asString();
	break;
#ifndef QT_NO_STRINGLIST
    case QVariant::StringList:
	asStringList();
	break;
#endif
    case QVariant::Int:
	asInt();
	break;
    case QVariant::UInt:
	asUInt();
	break;
    case QVariant::Bool:
	asBool();
	break;
    case QVariant::Double:
	asDouble();
	break;
    case QVariant::CString:
	asCString();
	break;
    case QVariant::Date:
	asDate();
	break;
    case QVariant::Time:
	asTime();
	break;
    case QVariant::DateTime:
	asDateTime();
	break;
    case QVariant::ByteArray:
	asByteArray();
	break;
    case QVariant::BitArray:
	asBitArray();
	break;
    case QVariant::LongLong:
	asLongLong();
	break;
    case QVariant::ULongLong:
	asULongLong();
	break;
    default:
    case QVariant::Invalid:
	(*this) = QVariant();
    }
    return canCast( t );
}

/*!
    Compares this QVariant with \a v and returns TRUE if they are
    equal; otherwise returns FALSE.
*/

bool QVariant::operator==( const QVariant &v ) const
{
    if ( !v.canCast( type() ) )
	return FALSE;
    switch( d->typ ) {
#ifndef QT_NO_TEMPLATE_VARIANT
    case List:
	return v.toList() == toList();
    case Map: {
	if ( v.toMap().count() != toMap().count() )
	    return FALSE;
	QMap<QString, QVariant>::ConstIterator it = v.toMap().begin();
	QMap<QString, QVariant>::ConstIterator it2 = toMap().begin();
	while ( it != v.toMap().end() ) {
	    if ( *it != *it2 )
		return FALSE;
	    ++it;
	    ++it2;
	}
	return TRUE;
    }
#endif
    case String:
	return v.toString() == toString();
    case CString:
	return v.toCString() == toCString();
#ifndef QT_NO_STRINGLIST
    case StringList:
	return v.toStringList() == toStringList();
#endif
    case Int:
	return v.toInt() == toInt();
    case UInt:
	return v.toUInt() == toUInt();
    case LongLong:
	return v.toLongLong() == toLongLong();
    case ULongLong:
	return v.toULongLong() == toULongLong();
    case Bool:
	return v.toBool() == toBool();
    case Double:
	return v.toDouble() == toDouble();
    case Date:
	return v.toDate() == toDate();
    case Time:
	return v.toTime() == toTime();
    case DateTime:
	return v.toDateTime() == toDateTime();
    case ByteArray:
	return v.toByteArray() == toByteArray();
    case BitArray:
	return v.toBitArray() == toBitArray();
    case Invalid:
	break;
    }
    return FALSE;
}

/*!
    Compares this QVariant with \a v and returns TRUE if they are not
    equal; otherwise returns FALSE.
*/

bool QVariant::operator!=( const QVariant &v ) const
{
    return !( v == *this );
}


/*! \internal

  Reads or sets the variant type and ptr
 */
void* QVariant::rawAccess( void* ptr, Type typ, bool deepCopy )
{
    if ( ptr ) {
	clear();
	d->typ = typ;
	d->value.ptr = ptr;
	d->is_null = FALSE;
	if ( deepCopy ) {
	    QVariant::Private* p = new Private( d );
	    d->typ = Invalid;
	    delete d;
	    d = p;
	}
    }

    if ( !deepCopy )
	return d->value.ptr;
    QVariant::Private* p = new Private( d );
    void *ret = (void*)p->value.ptr;
    p->typ = Invalid;
    delete p;
    return ret;
}

/*!
  Returns TRUE if this is a NULL variant, FALSE otherwise.
*/
bool QVariant::isNull() const
{
    switch( d->typ )
	{
	case QVariant::String:
	    return ((QString*) d->value.ptr)->isNull();
	case QVariant::CString:
	    return ((QCString*) d->value.ptr)->isNull();
	case QVariant::Date:
	    return ((QDate*) d->value.ptr)->isNull();
	case QVariant::Time:
	    return ((QTime*) d->value.ptr)->isNull();
	case QVariant::DateTime:
	    return ((QDateTime*) d->value.ptr)->isNull();
	case QVariant::ByteArray:
	    return ((QByteArray*) d->value.ptr)->isNull();
	case QVariant::BitArray:
	    return ((QBitArray*) d->value.ptr)->isNull();
#ifndef QT_NO_STRINGLIST
	case QVariant::StringList:
#endif //QT_NO_STRINGLIST
#ifndef QT_NO_TEMPLATE_VARIANT
	case QVariant::Map:
	case QVariant::List:
#endif
	case QVariant::Invalid:
	case QVariant::Int:
	case QVariant::UInt:
	case QVariant::LongLong:
	case QVariant::ULongLong:
	case QVariant::Bool:
	case QVariant::Double:
	    break;
	}
    return d->is_null;
}
#endif //QT_NO_VARIANT
