/*  Time-stamp: <18/03/2005 19:53:04 Yann Hodique>  */

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

ServerSocket::ServerSocket(QObject *parent) : QTcpServer(parent) {}

ServerSocket::~ServerSocket() {}

bool ServerSocket::hasPendingConnections() const {
    return !pending.isEmpty();
}

QTcpSocket* ServerSocket::nextPendingConnection() {
    ClientSocket *sock = pending.dequeue();
    active << sock;
    return sock;
}

void ServerSocket::incomingConnection (int socketDescriptor) {
    ClientSocket * sock = new ClientSocket(this);
    sock->setSocketDescriptor(socketDescriptor);

    pending.enqueue(sock);
    emit newConnection();
}
