/****************************************************************************
** $Id: qevent.h,v 1.1 2003/11/04 10:14:21 Sigma Exp $
**
** Definition of event classes
**
** Created : 931029
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

#ifndef QEVENT_H
#define QEVENT_H

#ifndef QT_H
#include "qwindowdefs.h"
#include "qnamespace.h"
#include "qpair.h"
#endif // QT_H

class Q_EXPORT QEvent: public Qt		// event base class
{
public:
    enum Type {

	/*
	  If you get a strange compiler error on the line with None,
	  it's probably because you're also including X11 headers,
	  which #define the symbol None. Put the X11 includes after
	  the Qt includes to solve this problem.
	*/

	None = 0,				// invalid event


	Timer = 1,				// timer event
	MouseButtonPress = 2,			// mouse button pressed
	MouseButtonRelease = 3,			// mouse button released
	MouseButtonDblClick = 4,		// mouse button double click
	MouseMove = 5,				// mouse move
	KeyPress = 6,				// key pressed
	KeyRelease = 7,				// key released
	FocusIn = 8,				// keyboard focus received
	FocusOut = 9,				// keyboard focus lost
	Enter = 10,				// mouse enters widget
	Leave = 11,				// mouse leaves widget
	Paint = 12,				// paint widget
	Move = 13,				// move widget
	Resize = 14,				// resize widget
	Create = 15,				// after object creation
	Destroy = 16,				// during object destruction
	Show = 17,				// widget is shown
	Hide = 18,				// widget is hidden
	Close = 19,				// request to close widget
	Quit = 20,				// request to quit application
	Reparent = 21,				// widget has been reparented
	ShowMinimized = 22,		       	// widget is shown minimized
	ShowNormal = 23,	       		// widget is shown normal
	WindowActivate = 24,	       		// window was activated
	WindowDeactivate = 25,	       		// window was deactivated
	ShowToParent = 26,	       		// widget is shown to parent
	HideToParent = 27,	       		// widget is hidden to parent
	ShowMaximized = 28,		       	// widget is shown maximized
	ShowFullScreen = 29,			// widget is shown full-screen
	Accel = 30,				// accelerator event
	Wheel = 31,				// wheel event
	AccelAvailable = 32,			// accelerator available event
	CaptionChange = 33,			// caption changed
	IconChange = 34,			// icon changed
	ParentFontChange = 35,			// parent font changed
	ApplicationFontChange = 36,		// application font changed
	ParentPaletteChange = 37,		// parent palette changed
	ApplicationPaletteChange = 38,		// application palette changed
	PaletteChange = 39,			// widget palette changed
	Clipboard = 40,				// internal clipboard event
	Speech = 42,				// reserved for speech input
	SockAct = 50,				// socket activation
	AccelOverride = 51,			// accelerator override event
	DeferredDelete = 52,			// deferred delete event
	DragEnter = 60,				// drag moves into widget
	DragMove = 61,				// drag moves in widget
	DragLeave = 62,				// drag leaves or is cancelled
	Drop = 63,				// actual drop
	DragResponse = 64,			// drag accepted/rejected
	ChildInserted = 70,			// new child widget
	ChildRemoved = 71,			// deleted child widget
	LayoutHint = 72,			// child min/max size changed
	ShowWindowRequest = 73,			// widget's window should be mapped
	ActivateControl = 80,			// ActiveX activation
	DeactivateControl = 81,			// ActiveX deactivation
	ContextMenu = 82,			// context popup menu
	IMStart = 83,				// input method composition start
	IMCompose = 84,				// input method composition
	IMEnd = 85,				// input method composition end
	Accessibility = 86,			// accessibility information is requested
	TabletMove = 87,			// Wacom tablet event
	LocaleChange = 88,			// the system locale changed
	LanguageChange = 89,			// the application language changed
	LayoutDirectionChange = 90,		// the layout direction changed
	Style = 91,				// internal style event
	TabletPress = 92,			// tablet press
	TabletRelease = 93,			// tablet release
	User = 1000,				// first user event id
	MaxUser = 65535				// last user event id
    };


    QEvent( Type type ) : t(type), posted(FALSE), spont(FALSE) {}
    virtual ~QEvent();
    Type  type() const	{ return t; }
    bool spontaneous() const 	{ return spont; }
protected:
    Type  t;
private:
    uint posted : 1;
    uint spont : 1;


    friend class QApplication;
    friend class QAccelManager;
    friend class QBaseApplication;
    friend class QETWidget;
};


class Q_EXPORT QTimerEvent : public QEvent
{
public:
    QTimerEvent( int timerId )
	: QEvent(Timer), id(timerId) {}
    int	  timerId()	const	{ return id; }
protected:
    int	  id;
};

class Q_EXPORT QCloseEvent : public QEvent
{
public:
    QCloseEvent()
	: QEvent(Close), accpt(FALSE) {}
    bool   isAccepted() const	{ return accpt; }
    void   accept()		{ accpt = TRUE; }
    void   ignore()		{ accpt = FALSE; }
protected:
    bool   accpt;
};

class Q_EXPORT QIMEvent : public QEvent
{
public:
    QIMEvent( Type type, const QString &text, int cursorPosition )
	: QEvent(type), txt(text), cpos(cursorPosition), a(FALSE) {}
    const QString &text() const { return txt; }
    int cursorPos() const { return cpos; }
    bool isAccepted() const { return a; }
    void accept() { a = TRUE; }
    void ignore() { a = FALSE; }
    int selectionLength() const;

private:
    QString txt;
    int cpos;
    bool a;
};

class Q_EXPORT QIMComposeEvent : public QIMEvent
{
public:
    QIMComposeEvent( Type type, const QString &text, int cursorPosition,
		     int selLength )
	: QIMEvent( type, text, cursorPosition ), selLen( selLength ) { }

private:
    int selLen;

    friend class QIMEvent;
};

inline int QIMEvent::selectionLength() const
{
    if ( type() != IMCompose ) return 0;
    QIMComposeEvent *that = (QIMComposeEvent *) this;
    return that->selLen;
}



class Q_EXPORT QChildEvent : public QEvent
{
public:
    QChildEvent( Type type, QObject *child )
	: QEvent(type), c(child) {}
    QObject *child() const	{ return c; }
    bool inserted() const { return t == ChildInserted; }
    bool removed() const { return t == ChildRemoved; }
protected:
    QObject *c;
};


class Q_EXPORT QCustomEvent : public QEvent
{
public:
    QCustomEvent( int type );
    QCustomEvent( Type type, void *data )
	: QEvent(type), d(data) {};
    void       *data()	const	{ return d; }
    void	setData( void* data )	{ d = data; }
private:
    void       *d;
};

#endif // QEVENT_H
