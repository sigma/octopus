/*  Time-stamp: <16/03/2005 22:47:35 Yann Hodique>  */

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

bool Connection::send(const QString & // to
                      , const QString & // msg
                      ) {
    return true;
}

void Connection::broadcastOthers(const QString & // except
                                 , const QString & // msg
                                 ) {}

void Connection::broadcastOthers(const QStringList & // except
                                 , const QString & // msg
                                 ) {}

void Connection::broadcast(const QString & // msg
                           ) {}

bool Connection::serverSend(const QString & // to
                            , const QString & // msg
                            ) {
    return true;
}

void Connection::serverBroadcastOthers(const QString & // except
                                       , const QString & // msg
                                       ) {}

void Connection::serverBroadcastOthers(const QStringList & // except
                                       , const QString & // msg
                                       ) {}

void Connection::serverBroadcast(const QString & // msg
                                 ) {}

void Connection::launch(int // port
                        ) {}
