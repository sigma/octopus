/****************************************************************************
** $Id: qevent.cpp,v 1.1 2003/11/04 10:14:21 Sigma Exp $
**
** Implementation of event classes
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

#include "qevent.h"
#include "qapplication.h"


/*!
    \class QEvent qevent.h
    \brief The QEvent class is the base class of all
    event classes. Event objects contain event parameters.

    \ingroup events
    \ingroup environment

    Qt's main event loop (QApplication::exec()) fetches native window
    system events from the event queue, translates them into QEvents
    and sends the translated events to QObjects.

    In general, events come from the underlying window system
    (spontaneous() returns TRUE) but it is also possible to manually
    send events using QApplication::sendEvent() and
    QApplication::postEvent() (spontaneous() returns FALSE).

    QObjects receive events by having their QObject::event() function
    called. The function can be reimplemented in subclasses to
    customize event handling and add additional event types;
    QWidget::event() is a notable example. By default, events are
    dispatched to event handlers like QObject::timerEvent() and
    QWidget::mouseMoveEvent(). QObject::installEventFilter() allows an
    object to intercept events destined for another object.

    The basic QEvent contains only an event type parameter.
    Subclasses of QEvent contain additional parameters that describe
    the particular event.

    \sa QObject::event() QObject::installEventFilter()
    QWidget::event() QApplication::sendEvent()
    QApplication::postEvent() QApplication::processEvents()
*/


/*!
    \enum Qt::ButtonState

    This enum type describes the state of the mouse and the modifier
    buttons.

    \value NoButton  used when the button state does not refer to any
    button (see QMouseEvent::button()).
    \value LeftButton  set if the left button is pressed, or if this
    event refers to the left button. (The left button may be
    the right button on left-handed mice.)
    \value RightButton  the right button.
    \value MidButton  the middle button.
    \value ShiftButton  a Shift key on the keyboard is also pressed.
    \value ControlButton  a Ctrl key on the keyboard is also pressed.
    \value AltButton  an Alt key on the keyboard is also pressed.
    \value MetaButton a Meta key on the keyboard is also pressed.
    \value Keypad  a keypad button is pressed.
    \value KeyButtonMask a mask for ShiftButton, ControlButton,
    AltButton and MetaButton.
    \value MouseButtonMask a mask for LeftButton, RightButton and MidButton.
*/

/*!
    \enum QEvent::Type

    This enum type defines the valid event types in Qt. The event
    types and the specialized classes for each type are these:

    \value None  Not an event.
    \value Accessibility Accessibility information is requested
    \value Timer  Regular timer events, \l{QTimerEvent}.
    \value MouseButtonPress  Mouse press, \l{QMouseEvent}.
    \value MouseButtonRelease  Mouse release, \l{QMouseEvent}.
    \value MouseButtonDblClick  Mouse press again, \l{QMouseEvent}.
    \value MouseMove  Mouse move, \l{QMouseEvent}.
    \value KeyPress  Key press (including Shift, for example), \l{QKeyEvent}.
    \value KeyRelease  Key release, \l{QKeyEvent}.
    \value IMStart The start of input method composition.
    \value IMCompose Input method composition is taking place.
    \value IMEnd The end of input method composition.
    \value FocusIn  Widget gains keyboard focus, \l{QFocusEvent}.
    \value FocusOut  Widget loses keyboard focus, \l{QFocusEvent}.
    \value Enter  Mouse enters widget's boundaries.
    \value Leave  Mouse leaves widget's boundaries.
    \value Paint  Screen update necessary, \l{QPaintEvent}.
    \value Move  Widget's position changed, \l{QMoveEvent}.
    \value Resize  Widget's size changed, \l{QResizeEvent}.
    \value Show  Widget was shown on screen, \l{QShowEvent}.
    \value Hide  Widget was hidden, \l{QHideEvent}.
    \value ShowToParent A child widget has been shown.
    \value HideToParent A child widget has been hidden.
    \value Close  Widget was closed (permanently), \l{QCloseEvent}.
    \value ShowNormal Widget should be shown normally.
    \value ShowMaximized Widget should be shown maximized.
    \value ShowMinimized Widget should be shown minimized.
    \value ShowFullScreen Widget should be shown full-screen.
    \value ShowWindowRequest Widget's window should be shown (obsolete).
    \value DeferredDelete The object will be deleted after it has
    cleaned up.
    \value Accel  Key press in child for shortcut key handling, \l{QKeyEvent}.
    \value Wheel  Mouse wheel rolled, \l{QWheelEvent}.
    \value ContextMenu context popup menu, \l{QContextMenuEvent}
    \value AccelOverride  Key press in child, for overriding shortcut key handling, \l{QKeyEvent}.
    \value AccelAvailable internal.
    \value WindowActivate  Window was activated.
    \value WindowDeactivate  Window was deactivated.
    \value CaptionChange  Widget's caption changed.
    \value IconChange  Widget's icon changed.
    \value ParentFontChange  Font of the parent widget changed.
    \value ApplicationFontChange  Default application font changed.
    \value PaletteChange  Palette of the widget changed.
    \value ParentPaletteChange  Palette of the parent widget changed.
    \value ApplicationPaletteChange  Default application palette changed.
    \value Clipboard  Clipboard contents have changed.
    \value SockAct  Socket activated, used to implement \l{QSocketNotifier}.
    \value DragEnter  A drag-and-drop enters widget, \l{QDragEnterEvent}.
    \value DragMove  A drag-and-drop is in progress, \l{QDragMoveEvent}.
    \value DragLeave  A drag-and-drop leaves widget, \l{QDragLeaveEvent}.
    \value Drop  A drag-and-drop is completed, \l{QDropEvent}.
    \value DragResponse  Internal event used by Qt on some platforms.
    \value ChildInserted  Object gets a child, \l{QChildEvent}.
    \value ChildRemoved  Object loses a child, \l{QChildEvent}.
    \value LayoutHint  Widget child has changed layout properties.
    \value ActivateControl  Internal event used by Qt on some platforms.
    \value DeactivateControl  Internal event used by Qt on some platforms.
    \value LanguageChange The application translation changed, \l{QTranslator}
    \value LayoutDirectionChange The direction of layouts changed
    \value LocaleChange The system locale changed
    \value Quit  Reserved.
    \value Create  Reserved.
    \value Destroy  Reserved.
    \value Reparent  Reserved.
    \value Speech Reserved for speech input.
    \value TabletMove  A Wacom Tablet Move Event.
    \value User  User defined event.
    \value MaxUser  Last user event id.
    \value TabletPress A Wacom Tablet Press Event
    \value TabletRelease A Wacom Tablet Release Event

    \value Style Internal use only

    User events should have values between User and MaxUser inclusive.
*/
/*!
    \fn QEvent::QEvent( Type type )

    Contructs an event object of type \a type.
*/

/*!
    \fn QEvent::Type QEvent::type() const

    Returns the event type.
*/

/*!
    \fn bool QEvent::spontaneous() const

    Returns TRUE if the event originated outside the application, i.e.
    it is a system event; otherwise returns FALSE.
*/


/*!
    \class QTimerEvent qevent.h
    \brief The QTimerEvent class contains parameters that describe a
    timer event.

    \ingroup events

    Timer events are sent at regular intervals to objects that have
    started one or more timers. Each timer has a unique identifier. A
    timer is started with QObject::startTimer().

    The QTimer class provides a high-level programming interface that
    uses signals instead of events. It also provides one-shot timers.

    The event handler QObject::timerEvent() receives timer events.

    \sa QTimer, QObject::timerEvent(), QObject::startTimer(),
    QObject::killTimer(), QObject::killTimers()
*/

/*!
    \fn QTimerEvent::QTimerEvent( int timerId )

    Constructs a timer event object with the timer identifier set to
    \a timerId.
*/

/*!
    \fn int QTimerEvent::timerId() const

    Returns the unique timer identifier, which is the same identifier
    as returned from QObject::startTimer().
*/


/*!
    \class QMouseEvent qevent.h
    \ingroup events

    \brief The QMouseEvent class contains parameters that describe a mouse event.

    Mouse events occur when a mouse button is pressed or released
    inside a widget or when the mouse cursor is moved.

    Mouse move events will occur only when a mouse button is pressed
    down, unless mouse tracking has been enabled with
    QWidget::setMouseTracking().

    Qt automatically grabs the mouse when a mouse button is pressed
    inside a widget; the widget will continue to receive mouse events
    until the last mouse button is released.

    A mouse event contains a special accept flag that indicates
    whether the receiver wants the event. You should call
    QMouseEvent::ignore() if the mouse event is not handled by your
    widget. A mouse event is propagated up the parent widget chain
    until a widget accepts it with QMouseEvent::accept() or an event
    filter consumes it.

    The functions pos(), x() and y() give the cursor position relative
    to the widget that receives the mouse event. If you move the
    widget as a result of the mouse event, use the global position
    returned by globalPos() to avoid a shaking motion.

    The QWidget::setEnabled() function can be used to enable or
    disable mouse and keyboard events for a widget.

    The event handlers QWidget::mousePressEvent(),
    QWidget::mouseReleaseEvent(), QWidget::mouseDoubleClickEvent() and
    QWidget::mouseMoveEvent() receive mouse events.

    \sa QWidget::setMouseTracking(), QWidget::grabMouse(),
    QCursor::pos()
*/




/*!
    \enum Qt::Modifier

    This enum type describes the keyboard modifier keys supported by
    Qt.

    \value SHIFT the Shift keys provided on all standard keyboards.
    \value META the Meta keys.
    \value CTRL the Ctrl keys.
    \value ALT the normal Alt keys, but not e.g. AltGr.
    \value MODIFIER_MASK is a mask of Shift, Ctrl, Alt and Meta.
    \value UNICODE_ACCEL the accelerator is specified as a Unicode code
    point, not as a Qt Key.
*/



/*!
    \class QCloseEvent qevent.h
    \brief The QCloseEvent class contains parameters that describe a close event.

    \ingroup events

    Close events are sent to widgets that the user wants to close,
    usually by choosing "Close" from the window menu, or by clicking
    the `X' titlebar button. They are also sent when you call
    QWidget::close() to close a widget programmatically.

    Close events contain a flag that indicates whether the receiver
    wants the widget to be closed or not. When a widget accepts the
    close event, it is hidden (and destroyed if it was created with
    the \c WDestructiveClose flag). If it refuses to accept the close
    event nothing happens. (Under X11 it is possible that the window
    manager will forcibly close the window; but at the time of writing
    we are not aware of any window manager that does this.)

    The application's main widget -- QApplication::mainWidget() --
    is a special case. When it accepts the close event, Qt leaves the
    main event loop and the application is immediately terminated
    (i.e. it returns from the call to QApplication::exec() in the
    main() function).

    The event handler QWidget::closeEvent() receives close events. The
    default implementation of this event handler accepts the close
    event. If you do not want your widget to be hidden, or want some
    special handing, you should reimplement the event handler.

    The \link simple-application.html#closeEvent closeEvent() in the
    Application Walkthrough\endlink shows a close event handler that
    asks whether to save a document before closing.

    If you want the widget to be deleted when it is closed, create it
    with the \c WDestructiveClose widget flag. This is very useful for
    independent top-level windows in a multi-window application.

    \l{QObject}s emits the \link QObject::destroyed()
    destroyed()\endlink signal when they are deleted.

    If the last top-level window is closed, the
    QApplication::lastWindowClosed() signal is emitted.

    The isAccepted() function returns TRUE if the event's receiver has
    agreed to close the widget; call accept() to agree to close the
    widget and call ignore() if the receiver of this event does not
    want the widget to be closed.

    \sa QWidget::close(), QWidget::hide(), QObject::destroyed(),
    QApplication::setMainWidget(), QApplication::lastWindowClosed(),
    QApplication::exec(), QApplication::quit()
*/

/*!
    \fn QCloseEvent::QCloseEvent()

    Constructs a close event object with the accept parameter flag set
    to FALSE.

    \sa accept()
*/

/*!
    \fn bool QCloseEvent::isAccepted() const

    Returns TRUE if the receiver of the event has agreed to close the
    widget; otherwise returns FALSE.

    \sa accept(), ignore()
*/

/*!
    \fn void QCloseEvent::accept()

    Sets the accept flag of the close event object.

    Setting the accept flag indicates that the receiver of this event
    agrees to close the widget.

    The accept flag is \e not set by default.

    If you choose to accept in QWidget::closeEvent(), the widget will
    be hidden. If the widget's \c WDestructiveClose flag is set, it
    will also be destroyed.

    \sa ignore(), QWidget::hide()
*/

/*!
    \fn void QCloseEvent::ignore()

    Clears the accept flag of the close event object.

    Clearing the accept flag indicates that the receiver of this event
    does not want the widget to be closed.

    The close event is constructed with the accept flag cleared.

    \sa accept()
*/

/*!
    \class QContextMenuEvent qevent.h
    \brief The QContextMenuEvent class contains parameters that describe a context menu event.

    \ingroup events

    Context menu events are sent to widgets when a user triggers a
    context menu. What triggers this is platform dependent. For
    example, on Windows, pressing the menu button or releasing the
    right mouse button will cause this event to be sent.

    When this event occurs it is customary to show a QPopupMenu with a
    context menu, if this is relevant to the context.

    Context menu events contain a special accept flag that indicates
    whether the receiver accepted the event. If the event handler does
    not accept the event, then whatever triggered the event will be
    handled as a regular input event if possible.

    \sa QPopupMenu
*/

/*!
    \fn QContextMenuEvent::QContextMenuEvent( Reason reason, const QPoint &pos, const QPoint &globalPos, int state )

    Constructs a context menu event object with the accept parameter
    flag set to FALSE.

    The \a reason parameter must be \c QContextMenuEvent::Mouse or \c
    QContextMenuEvent::Keyboard.

    The \a pos parameter specifies the mouse position relative to the
    receiving widget. \a globalPos is the mouse position in absolute
    coordinates. \a state is the ButtonState at the time of the event.
*/



/*!
    \class QIMEvent qevent.h
    \brief The QIMEvent class provides parameters for input method events.

    \ingroup events

    Input method events are sent to widgets when an input method is
    used to enter text into a widget. Input methods are widely used to
    enter text in Asian and other complex languages.

    The events are of interest to widgets that accept keyboard input
    and want to be able to correctly handle complex languages. Text
    input in such languages is usually a three step process.

    \list 1
    \i <b>Starting to Compose</b><br>
    When the user presses the first key on a keyboard an input context
    is created. This input context will contain a string with the
    typed characters.

    \i <b>Composing</b><br>
    With every new key pressed, the input method will try to create a
    matching string for the text typed so far. While the input context
    is active, the user can only move the cursor inside the string
    belonging to this input context.

    \i <b>Completing</b><br>
    At some point, e.g. when the user presses the Spacebar, they get
    to this stage, where they can choose from a number of strings that
    match the text they have typed so far. The user can press Enter to
    confirm their choice or Escape to cancel the input; in either case
    the input context will be closed.
    \endlist

    Note that the particular key presses used for a given input
    context may differ from those we've mentioned here, i.e. they may
    not be Spacebar, Enter and Escape.

    These three stages are represented by three different types of
    events. The IMStartEvent, IMComposeEvent and IMEndEvent. When a
    new input context is created, an IMStartEvent will be sent to the
    widget and delivered to the \l QWidget::imStartEvent() function.
    The widget can then update internal data structures to reflect
    this.

    After this, an IMComposeEvent will be sent to the widget for
    every key the user presses. It will contain the current
    composition string the widget has to show and the current cursor
    position within the composition string. This string is temporary
    and can change with every key the user types, so the widget will
    need to store the state before the composition started (the state
    it had when it received the IMStartEvent). IMComposeEvents will be
    delivered to the \l QWidget::imComposeEvent() function.

    Usually, widgets try to mark the part of the text that is part of
    the current composition in a way that is visible to the user. A
    commonly used visual cue is to use a dotted underline.

    After the user has selected the final string, an IMEndEvent will
    be sent to the widget. The event contains the final string the
    user selected, and could be empty if they canceled the
    composition. This string should be accepted as the final text the
    user entered, and the intermediate composition string should be
    cleared. These events are delivered to \l QWidget::imEndEvent().

    If the user clicks another widget, taking the focus out of the
    widget where the composition is taking place the IMEndEvent will
    be sent and the string it holds will be the result of the
    composition up to that point (which may be an empty string).
*/

/*!
    \fn  QIMEvent::QIMEvent( Type type, const QString &text, int cursorPosition )

    Constructs a new QIMEvent with the accept flag set to FALSE. \a
    type can be one of QEvent::IMStartEvent, QEvent::IMComposeEvent
    or QEvent::IMEndEvent. \a text contains the current compostion
    string and \a cursorPosition the current position of the cursor
    inside \a text.
*/

/*!
    \fn const QString &QIMEvent::text() const

    Returns the composition text. This is a null string for an
    IMStartEvent, and contains the final accepted string (which may be
    empty) in the IMEndEvent.
*/

/*!
    \fn int QIMEvent::cursorPos() const

    Returns the current cursor position inside the composition string.
    Will return 0 for IMStartEvent and IMEndEvent.
*/

/*!
    \fn int QIMEvent::selectionLength() const

    Returns the number of characters in the composition string (
    starting at cursorPos() ) that should be marked as selected by the
    input widget receiving the event.
    Will return 0 for IMStartEvent and IMEndEvent.
*/

/*!
    \fn bool QIMEvent::isAccepted() const

    Returns TRUE if the receiver of the event processed the event;
    otherwise returns FALSE.
*/

/*!
    \fn void QIMEvent::accept()

    Sets the accept flag of the input method event object.

    Setting the accept parameter indicates that the receiver of the
    event processed the input method event.

    The accept flag is not set by default.

    \sa ignore()
*/


/*!
    \fn void QIMEvent::ignore()

    Clears the accept flag parameter of the input method event object.

    Clearing the accept parameter indicates that the event receiver
    does not want the input method event.

    The accept flag is cleared by default.

    \sa accept()
*/

/*!
    \class QTabletEvent qevent.h
    \brief The QTabletEvent class contains parameters that describe a Tablet
    event.

    \ingroup events

    Tablet Events are generated from a Wacom&copy; tablet. Most of
    the time you will want to deal with events from the tablet as if
    they were events from a mouse, for example retrieving the position
    with x(), y(), pos(), globalX(), globalY() and globalPos(). In
    some situations you may wish to retrieve the extra information
    provided by the tablet device driver, for example, you might want
    to adjust color brightness based on pressure. QTabletEvent allows
    you to get the pressure(), the xTilt() and yTilt(), as well as the
    type of device being used with device() (see \l{TabletDevice}).

    A tablet event contains a special accept flag that indicates
    whether the receiver wants the event. You should call
    QTabletEvent::accept() if you handle the tablet event; otherwise
    it will be sent to the parent widget.

    The QWidget::setEnabled() function can be used to enable or
    disable mouse and keyboard events for a widget.

  The event handler QWidget::tabletEvent() receives all three types of tablet
  events.  Qt will first send a tabletEvent and then, if it is not accepted,
  it will send a mouse event.  This allows applications that don't utilize
  tablets to use a tablet like a mouse while also enabling those who want to
  use both tablets and mouses differently.

*/

/*!
    \enum QTabletEvent::TabletDevice

    This enum defines what type of device is generating the event.

    \value NoDevice    No device, or an unknown device.
    \value Puck    A Puck (a device that is similar to a flat mouse with
    a transparent circle with cross-hairs).
    \value Stylus  A Stylus (the narrow end of the pen).
    \value Eraser  An Eraser (the broad end of the pen).
    \omit
    \value Menu  A menu button was pressed (currently unimplemented).
*/

/*!
    \fn QPair<int, int> QTabletEvent::uniqueId()

    Returns a unique ID for the current device. It is possible to
    generate a unique ID for any Wacom&copy; device. This makes it
    possible to differentiate between multiple devices being used at
    the same time on the tablet. The \c first member contains a value
    for the type, the \c second member contains a physical ID obtained
    from the device. Each combination of these values is unique. Note:
    for different platforms, the \c first value is different due to
    different driver implementations.
*/

/*!
    \class QChildEvent qevent.h
    \brief The QChildEvent class contains event parameters for child object
    events.

    \ingroup events

    Child events are sent to objects when children are inserted or
    removed.

    A \c ChildRemoved event is sent immediately, but a \c
    ChildInserted event is \e posted (with QApplication::postEvent()).

    Note that if a child is removed immediately after it is inserted,
    the \c ChildInserted event may be suppressed, but the \c
    ChildRemoved event will always be sent. In this case there will be
    a \c ChildRemoved event without a corresponding \c ChildInserted
    event.

    The handler for these events is QObject::childEvent().
*/

/*!
    \fn QChildEvent::QChildEvent( Type type, QObject *child )

    Constructs a child event object. The \a child is the object that
    is to be removed or inserted.

    The \a type parameter must be either \c QEvent::ChildInserted or
    \c QEvent::ChildRemoved.
*/

/*!
    \fn QObject *QChildEvent::child() const

    Returns the child widget that was inserted or removed.
*/

/*!
    \fn bool QChildEvent::inserted() const

    Returns TRUE if the widget received a new child; otherwise returns
    FALSE.
*/

/*!
    \fn bool QChildEvent::removed() const

    Returns TRUE if the object lost a child; otherwise returns FALSE.
*/




/*!
    \class QCustomEvent qevent.h
    \brief The QCustomEvent class provides support for custom events.

    \ingroup events

    QCustomEvent is a generic event class for user-defined events.
    User defined events can be sent to widgets or other QObject
    instances using QApplication::postEvent() or
    QApplication::sendEvent(). Subclasses of QWidget can easily
    receive custom events by implementing the QWidget::customEvent()
    event handler function.

    QCustomEvent objects should be created with a type ID that
    uniquely identifies the event type. To avoid clashes with the
    Qt-defined events types, the value should be at least as large as
    the value of the "User" entry in the QEvent::Type enum.

    QCustomEvent contains a generic void* data member that may be used
    for transferring event-specific data to the receiver. Note that
    since events are normally delivered asynchronously, the data
    pointer, if used, must remain valid until the event has been
    received and processed.

    QCustomEvent can be used as-is for simple user-defined event
    types, but normally you will want to make a subclass of it for
    your event types. In a subclass, you can add data members that are
    suitable for your event type.

    Example:
    \code
    class ColorChangeEvent : public QCustomEvent
    {
    public:
	ColorChangeEvent( QColor color )
	    : QCustomEvent( 65432 ), c( color ) {}
	QColor color() const { return c; }
    private:
	QColor c;
    };

    // To send an event of this custom event type:

    ColorChangeEvent* ce = new ColorChangeEvent( blue );
    QApplication::postEvent( receiver, ce );  // Qt will delete it when done

    // To receive an event of this custom event type:

    void MyWidget::customEvent( QCustomEvent * e )
    {
	if ( e->type() == 65432 ) {  // It must be a ColorChangeEvent
	    ColorChangeEvent* ce = (ColorChangeEvent*)e;
	    newColor = ce->color();
	}
    }
    \endcode

    \sa QWidget::customEvent(), QApplication::notify()
*/


/*!
    Constructs a custom event object with event type \a type. The
    value of \a type must be at least as large as QEvent::User. The
    data pointer is set to 0.
*/

QCustomEvent::QCustomEvent( int type )
    : QEvent( (QEvent::Type)type ), d( 0 )
{
}


/*!
    \fn QCustomEvent::QCustomEvent( Type type, void *data )

    Constructs a custom event object with the event type \a type and a
    pointer to \a data. (Note that any int value may safely be cast to
    QEvent::Type).
*/


/*!
    \fn void QCustomEvent::setData( void* data )

    Sets the generic data pointer to \a data.

    \sa data()
*/

/*!
    \fn void *QCustomEvent::data() const

    Returns a pointer to the generic event data.

    \sa setData()
*/



/*!
    \fn QDragMoveEvent::QDragMoveEvent( const QPoint& pos, Type type )

    Creates a QDragMoveEvent for which the mouse is at point \a pos,
    and the event is of type \a type.

    \warning Do not create a QDragMoveEvent yourself since these
    objects rely on Qt's internal state.
*/

/*!
    \fn void QDragMoveEvent::accept( const QRect & r )

    The same as accept(), but also notifies that future moves will
    also be acceptable if they remain within the rectangle \a r on the
    widget: this can improve performance, but may also be ignored by
    the underlying system.

    If the rectangle is \link QRect::isEmpty() empty\endlink, then
    drag move events will be sent continuously. This is useful if the
    source is scrolling in a timer event.
*/

/*!
    \fn void QDragMoveEvent::ignore( const QRect & r)

    The opposite of accept(const QRect&), i.e. says that moves within
    rectangle \a r are not acceptable (will be ignored).
*/

/*!
    \fn QRect QDragMoveEvent::answerRect() const

    Returns the rectangle for which the acceptance of the move event
    applies.
*/



/*!
    \fn const QPoint& QDropEvent::pos() const

    Returns the position where the drop was made.
*/

/*!
    \fn bool QDropEvent::isAccepted () const

    Returns TRUE if the drop target accepts the event; otherwise
    returns FALSE.
*/

/*!
    \fn void QDropEvent::accept(bool y=TRUE)

    Call this function to indicate whether the event provided data
    which your widget processed. Set \a y to TRUE (the default) if
    your widget could process the data, otherwise set \a y to FALSE.
    To get the data, use encodedData(), or preferably, the decode()
    methods of existing QDragObject subclasses, such as
    QTextDrag::decode(), or your own subclasses.

    \sa acceptAction()
*/

/*!
    \fn void QDropEvent::acceptAction(bool y=TRUE)

    Call this to indicate that the action described by action() is
    accepted (i.e. if \a y is TRUE, which is the default), not merely
    the default copy action. If you call acceptAction(TRUE), there is
    no need to also call accept(TRUE).
*/

/*!
  \fn void QDragMoveEvent::accept( bool y )
  \reimp
  \internal
  Remove in 3.0
*/

/*!
  \fn void QDragMoveEvent::ignore()
  \reimp
  \internal
  Remove in 3.0
*/


/*!
    \enum QDropEvent::Action

    This enum describes the action which a source requests that a
    target perform with dropped data.

    \value Copy The default action. The source simply uses the data
		provided in the operation.
    \value Link The source should somehow create a link to the
		location specified by the data.
    \value Move The source should somehow move the object from the
		location specified by the data to a new location.
    \value Private  The target has special knowledge of the MIME type,
		which the source should respond to in a similar way to
		a Copy.
    \value UserAction  The source and target can co-operate using
		special actions. This feature is not currently
		supported.

    The Link and Move actions only makes sense if the data is a
    reference, for example, text/uri-list file lists (see QUriDrag).
*/

/*!
    \fn void QDropEvent::setAction( Action a )

    Sets the action to \a a. This is used internally, you should not
    need to call this in your code: the \e source decides the action,
    not the target.
*/

/*!
    \fn Action QDropEvent::action() const

    Returns the Action which the target is requesting to be performed
    with the data. If your application understands the action and can
    process the supplied data, call acceptAction(); if your
    application can process the supplied data but can only perform the
    Copy action, call accept().
*/

/*!
    \fn void QDropEvent::ignore()

    The opposite of accept(), i.e. you have ignored the drop event.
*/

/*!
    \fn bool QDropEvent::isActionAccepted () const

    Returns TRUE if the drop action was accepted by the drop site;
    otherwise returns FALSE.
*/


/*!
    \fn void QDropEvent::setPoint (const QPoint & np)

    Sets the drop to happen at point \a np. You do not normally need
    to use this as it will be set internally before your widget
    receives the drop event.
*/ // ### here too - what coordinate system?


/*!
    \class QDragEnterEvent qevent.h
    \brief The QDragEnterEvent class provides an event which is sent to the widget when a drag and drop first drags onto the widget.

    \ingroup events
    \ingroup draganddrop

    This event is always immediately followed by a QDragMoveEvent, so
    you only need to respond to one or the other event. This class
    inherits most of its functionality from QDragMoveEvent, which in
    turn inherits most of its functionality from QDropEvent.

    \sa QDragLeaveEvent, QDragMoveEvent, QDropEvent
*/

/*!
    \fn QDragEnterEvent::QDragEnterEvent (const QPoint & pos)

    Constructs a QDragEnterEvent entering at the given point, \a pos.

    \warning Do not create a QDragEnterEvent yourself since these
    objects rely on Qt's internal state.
*/

/*!
    \class QDragLeaveEvent qevent.h
    \brief The QDragLeaveEvent class provides an event which is sent to the widget when a drag and drop leaves the widget.

    \ingroup events
    \ingroup draganddrop

    This event is always preceded by a QDragEnterEvent and a series of
    \l{QDragMoveEvent}s. It is not sent if a QDropEvent is sent
    instead.

    \sa QDragEnterEvent, QDragMoveEvent, QDropEvent
*/

/*!
    \fn QDragLeaveEvent::QDragLeaveEvent()

    Constructs a QDragLeaveEvent.

    \warning Do not create a QDragLeaveEvent yourself since these
    objects rely on Qt's internal state.
*/

/*!
    \class QHideEvent qevent.h
    \brief The QHideEvent class provides an event which is sent after a widget is hidden.

    \ingroup events

    This event is sent just before QWidget::hide() returns, and also
    when a top-level window has been hidden (iconified) by the user.

    If spontaneous() is TRUE the event originated outside the
    application, i.e. the user hid the window using the window manager
    controls, either by iconifying the window or by switching to
    another virtual desktop where the window isn't visible. The window
    will become hidden but not withdrawn. If the window was iconified,
    QWidget::isMinimized() returns TRUE.

    \sa QShowEvent
*/

/*!
    \fn QHideEvent::QHideEvent()

    Constructs a QHideEvent.
*/

/*!
    \class QShowEvent qevent.h
    \brief The QShowEvent class provides an event which is sent when a widget is shown.

    \ingroup events

    There are two kinds of show events: show events caused by the
    window system (spontaneous) and internal show events. Spontaneous
    show events are sent just after the window system shows the
    window, including after a top-level window has been shown
    (un-iconified) by the user. Internal show events are delivered
    just before the widget becomes visible.

    \sa QHideEvent
*/

/*!
    \fn QShowEvent::QShowEvent()

    Constructs a QShowEvent.
*/


/*!
  \fn QByteArray QDropEvent::data(const char* f) const

  \obsolete

  Use QDropEvent::encodedData().
*/


/*!
  Destroys the event. If it was \link
  QApplication::postEvent() posted \endlink,
  it will be removed from the list of events to be posted.
*/

QEvent::~QEvent()
{
    if ( posted && qApp )
	QApplication::removePostedEvent( this );
}
