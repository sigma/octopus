/*  Time-stamp: <16/03/2005 23:55:38 Yann Hodique>  */

/**
 *  @file connection.cpp
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

#include "connection.h"
#include "serversocket.h"

PLUGIN_FACTORY(Connection);

Connection::Connection(PluginManagerInterface* parent) : ConnectionPlugin(parent) {}

Connection::~Connection() {}

void Connection::killedUser(const QString& // login
                            ) {}

void Connection::incomingUser(const QString& // login
                              ) {}

void Connection::outgoingUser(const QString& // login
                              ) {}

void Connection::renamedUser(const QString& // old_login
                             , const QString& // new_login
                             ) {}

void Connection::exportCommands() {}

QString Connection::host(const QString& // login
                         ) const {
    return QString::null;
}

QDateTime Connection::connectedFor(const QString &// login
                                   ) const {
    return QDateTime::currentDateTime();
}

QDateTime Connection::idleFor(const QString &// login
                              ) const {
    return QDateTime::currentDateTime();
}

QDateTime Connection::serverStartDateTime() const {
    return QDateTime::currentDateTime();
}

bool Connection::send(const QString & to, const QString & msg) {
    ServerSocket* ssock = find(to);
    return ssock->send(to,msg);
}

void Connection::broadcast(const QString & msg) {
    for(ServerList::ConstIterator it = servers.begin(); it != servers.end(); ++it)
        (*it)->broadcast(msg);
}

void Connection::broadcastOthers(const QString & except, const QString & msg) {
    for(ServerList::ConstIterator it = servers.begin(); it != servers.end(); ++it)
        (*it)->broadcastOthers(except, msg);
}

void Connection::broadcastOthers(const QStringList & except, const QString & msg) {
    for(ServerList::ConstIterator it = servers.begin(); it != servers.end(); ++it)
        (*it)->broadcastOthers(except, msg);
}

bool Connection::serverSend(const QString & to, const QString & msg) {
    return send(to, serverSays(msg));
}

void Connection::serverBroadcast(const QString &msg) {
    broadcast(serverSays(msg));
}

void Connection::serverBroadcastOthers(const QString & except, const QString & msg) {
    broadcastOthers(except, serverSays(msg));
}

void Connection::serverBroadcastOthers(const QStringList &except, const QString &msg) {
    broadcastOthers(except, serverSays(msg));
}

void Connection::launch(int port) {
    ServerSocket *ssock = new ServerSocket(this);
    ssock->listen(port);
    servers << ssock;
}

QString Connection::serverSays(const QString& msg) const {
    return "<Mtp> " + msg;
}

ServerSocket * Connection::find(const QString& user) const {
    return users[user];
}
