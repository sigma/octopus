/*  Time-stamp: <18/03/2005 21:43:40 Yann Hodique>  */

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
#include "clientsocket.h"

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
    return find(to).send(manager()->prepostPlugin()->treatOutgoingMessage(to,msg).replace(QChar('\n'),"\r\n") + "\r\n");
}

void Connection::broadcast(const QString & msg) {
    QList<Path> l = users.values();
    for(QList<Path>::Iterator it = l.begin(); it != l.end(); it++)
        (*it).send(msg);
}

void Connection::broadcastOthers(const QString & except, const QString & msg) {
    QList<Path> l = users.values();
    l.removeAll(find(except));
    for(QList<Path>::Iterator it = l.begin(); it != l.end(); it++)
        (*it).send(msg);
}

void Connection::broadcastOthers(const QStringList & except, const QString & msg) {
    QList<Path> l = users.values();
    for(QStringList::ConstIterator it = except.begin(); it != except.end(); ++it)
        l.removeAll(find(*it));
    for(QList<Path>::Iterator it = l.begin(); it != l.end(); it++)
        (*it).send(msg);
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
    connect(ssock, SIGNAL(newConnection()), SLOT(treatIncoming()));
    ssock->listen(port);
    servers << ssock;
}

QString Connection::serverSays(const QString& msg) const {
    return "<Mtp> " + msg;
}

const Connection::Path Connection::find(const QString& user) const {
    return users[user];
}

bool Connection::Path::send(const QString& msg) const {
    bool res = false;
    for(ConstIterator it = begin(); it != end(); ++it) {
        res = (*it)->write(msg.ascii()) || res;
    }
    return res;
}

void Connection::treatIncoming() {
    ServerSocket* ssock = static_cast<ServerSocket*>(sender());
    if(ssock) {
        while(ssock->hasPendingConnections())
            welcomeIncoming((ClientSocket *)ssock->nextPendingConnection());
    }
}

void Connection::welcomeIncoming(ClientSocket* sock) {
    sock->write(serverSays("Login: ").ascii());
    connect(sock, SIGNAL(readyRead()), SLOT(authIncoming()));
}

void Connection::authIncoming() {
    ClientSocket* sock = static_cast<ClientSocket*>(sender());

    disconnect(sock, SIGNAL(readyRead()), this, SLOT(authIncoming()));
    QString login = sock->readLineData();
    octInfo(QString("Welcoming %1\n").arg(login));

    sock->setLogin(login);

    if(manager()->databasePlugin()->isRegisteredUser(login)) {
        sock->write(serverSays("Password: ").ascii());
        connect(sock, SIGNAL(readyRead()), SLOT(checkIncoming()));
    } else
        accept(sock);
}

void Connection::checkIncoming() {
    ClientSocket* sock = static_cast<ClientSocket*>(sender());

    disconnect(sock, SIGNAL(readyRead()), this, SLOT(checkIncoming()));
    if(manager()->databasePlugin()->authUser(sock->getLogin(),sock->readLineData()))
        accept(sock);
    else
        sock->close();
}

void Connection::accept(ClientSocket* sock) {
    Path& p = users[sock->getLogin()];
    p << sock;
    connect(sock, SIGNAL(readyRead()), SLOT(processText()));

    manager()->in(sock->getLogin());
}

void Connection::processText() {
    ClientSocket* sock = static_cast<ClientSocket*>(sender());
    while(sock->canReadLine())
        processText(sock->getLogin(), sock->readLineData());
}

void Connection::processText(const QString& author, const QString& txt) {
    octInfo(QString("Processing text from %1 : \"%2\"\n").arg(author).arg(txt));
    if(!txt.isEmpty()) {
        QString msg = txt;
        msg = manager()->prepostPlugin()->treatIncomingMessage(author,msg);
        if(!msg.isEmpty()) {
            QStringList l = msg.split('\n',QString::SkipEmptyParts);
            for(QStringList::ConstIterator it = l.begin(); it != l.end(); it++) {
                octInfo(QString("Dispatching message \"%1\"\n").arg(txt));
                manager()->dispatch(author, *it);
            }
        }
    }
}
