/***************************************************************************
 *  file                 : clientsocket.h                                  *
 *  begin                : Tuesday, April 22, 2003                         *
 *                           -------------------                           *
 *  copyright            : Yann Hodique                                    *
 *  email                : Yann.Hodique@lifl.fr                            *
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

/****************************************************************************
**
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef _CLIENTSOCKET_H_
#define _CLIENTSOCKET_H_

#include <qsocket.h>
#include <qdatetime.h>

/*
  The ClientSocket class provides a socket that is connected with a client.
  For every client that connects to the server, the server creates a new
  instance of this class.
*/
class ClientSocket : public QSocket {
    Q_OBJECT
public:
    ClientSocket( int sock, QObject *parent=0, const char *name=0 );

    ~ClientSocket();


    void brutallyClose() {
        flush();
        close();
        emit simulateDisconnect();
        deleteLater();
    }

    void forceClose() {
        flush();
        close();
        emit simulateClose();
        deleteLater();
    }

    QString peerHostName();

    QDateTime connectedFor;

    QString extractCommands(const QString&);

signals:
    void logText( const QString& );
    void processText(const QString& );
    void simulateDisconnect();
    void simulateClose();

    void telnetCommand(int);

private slots:
    void readClient();
};

#endif
