/***************************************************************************
 *  file                 : simpleserver.cpp                                *
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

#include "clientsocket.h"
#include "simpleserver.h"
#include "server.h"

SimpleServer::SimpleServer( int port, QObject* parent ) : QServerSocket( port, 1, parent ) {
  if ( !ok() ) {
    qWarning("Failed to bind to port " + QString::number(port));
    //            exit(1);
  }
}

SimpleServer::~SimpleServer() {}

void SimpleServer::killall(Connect* s) {
//    std::cout << " BEGIN killall() " << sockets.count() << std::endl;
    for(QValueList<ClientSocket*>::Iterator it = sockets.begin(); it != sockets.end(); ++it) {
      disconnect(*it, SIGNAL(connectionClosed()),
 		 s, SLOT(connectionClose()));
      disconnect(*it, SIGNAL(connectionClosed()),
		 this, SLOT(closedSocket()));
      disconnect(*it, SIGNAL(simulateDisconnect()),
		 this, SLOT(closedSocket()));
      disconnect(*it, SIGNAL(error(int)),
		 this, SLOT(closedSocket()));
      (*it)->brutallyClose();
    }
    sockets.clear();
//    std::cout << " END killall()" << std::endl;
}

void SimpleServer::newConnection( int socket ) {
  ClientSocket *s = new ClientSocket( socket );
  sockets << s;
  emit newConnect( s );

  connect(s, SIGNAL(connectionClosed()),
	  this, SLOT(closedSocket()));
  connect(s, SIGNAL(delayedCloseFinished()),
	  this, SLOT(closedSocket()));
  connect(s, SIGNAL(simulateDisconnect()),
	  this, SLOT(closedSocket()));
  connect(s, SIGNAL(simulateClose()),
	  this, SLOT(closedSocket()));
  connect(s, SIGNAL(error(int)),
	  this, SLOT(closedSocket()));
}
