/*  Time-stamp: <20/03/2005 14:33:59 Yann Hodique>  */

/**
 *  @file serversocket.cpp
 *  @date Wednesday, March 16, 2005
 *  @author Yann Hodique <Yann.Hodique@lifl.fr>
 */

/************************************************************************
 *                                                                      *
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 *                                                                      *
 ************************************************************************/

#include "serversocket.h"
#include "clientsocket.h"

#include "debug.h"

ServerSocket::ServerSocket(QObject *parent) : QTcpServer(parent) {

}

ServerSocket::~ServerSocket() {
    octInfo("~ServerSocket()\n");
    close();
    for(QQueue<ClientSocket*>::Iterator it = pending.begin(); it != pending.end(); ++it) {
        (*it)->deleteLater();
    }
    for(QList<ClientSocket*>::Iterator it = active.begin(); it != active.end(); ++it) {
        (*it)->deleteLater();
    }

}

bool ServerSocket::hasPendingConnections() const {
    return !pending.isEmpty();
}

QTcpSocket* ServerSocket::nextPendingConnection() {
    ClientSocket *sock = pending.dequeue();
    active << sock;
    return sock;
}

void ServerSocket::incomingConnection (int socketDescriptor) {
    octInfo("incomingConnection()\n");
    ClientSocket * sock = new ClientSocket(this);
    connect(sock, SIGNAL(closed()), SLOT(cleanSocket()));
    sock->setSocketDescriptor(socketDescriptor);

    pending.enqueue(sock);
}

void ServerSocket::cleanSocket() {
    ClientSocket * sock = static_cast<ClientSocket*>(sender());

    pending.removeAll(sock);
    active.removeAll(sock);
    sock->deleteLater();
}
