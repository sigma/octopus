/***************************************************************************
 *  file                 : simpleserver.h                                  *
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

#ifndef _SIMPLESERVER_H_
#define _SIMPLESERVER_H_

#include <Q3ServerSocket>
#include <iostream>
/*
  The SimpleServer class handles new connections to the server. For every
  client that connects, it creates a new ClientSocket -- that instance is now
  responsible for the communication with that client.
*/
class Connect;
class ClientSocket;

class SimpleServer : public Q3ServerSocket {
    Q_OBJECT
public:
    SimpleServer( int port, QObject* parent=0 );
  ~SimpleServer();

    void killall(Connect* s);

  void newConnection( int socket );

signals:
    void newConnect( ClientSocket* );

private slots:
    void closedSocket() {
        sockets.removeAll((ClientSocket*) sender());
    }

private:
    QList<ClientSocket*> sockets;
};


#endif
