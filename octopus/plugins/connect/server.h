/***************************************************************************
 *  file                 : server.h                                        *
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

#ifndef _SERVER_H_
#define _SERVER_H_

#include <qstringlist.h>
#include <QDateTime>

#include "plugins.h"

/*
  The Connect class creates the
  SimpleServer and as a result the server.
*/

#define HISTORY_LINES 20

class ClientSocket;
class SimpleServer;

class Connect : public ConnectionPlugin {
    Q_OBJECT
    PLUGIN_OBJECT(Connect)
public:

    typedef QList<ClientSocket*> HostList;
    typedef QMap<QString,HostList> NameMap;
    typedef QMap<ClientSocket*,QString> SocketMap;
    typedef QMap<QString,QStringList> HostMap;
    typedef QMap<int,SimpleServer*> PortsMap;
    typedef QMap<QString,QDateTime> DateMap;

    Connect(PluginManagerInterface* parent=0);

    ~Connect();

    void killedUser(const QString& /*login*/);

    void launch(int port);
    bool send(const QString & to, const QString & msg);
    void broadcastOthers(const QString & except, const QString & msg);
    void broadcastOthers(const QStringList & except, const QString & msg);
    void broadcast(const QString & msg);
    bool serverSend(const QString & to, const QString & msg);
    void serverBroadcastOthers(const QString & except, const QString & msg);
    void serverBroadcastOthers(const QStringList & except, const QString & msg);
    void serverBroadcast(const QString & msg);
    void exportCommands();

    QString host(const QString& login) const;
    QDateTime connectedFor(const QString &login) const;
    QDateTime idleFor(const QString &login) const;
    QDateTime serverStartDateTime() const;

    void incomingUser(const QString& login);
    void outgoingUser(const QString& login);
    void renamedUser(const QString&, const QString&);

public slots:
    void quitCmd(const QString&, const QStringList&);
    void kickCmd(const QString&, const QStringList&);
    void forceCmd(const QString&, const QStringList&);
    void historyCmd(const QString&, const QStringList&);
    void openCmd(const QString&, const QStringList&);
    void closeCmd(const QString&, const QStringList&);
    void lsportsCmd(const QString&, const QStringList&);
    void uptimeCmd(const QString&, const QStringList&);
    void lssessionsCmd(const QString&, const QStringList&);
    void closesessionCmd(const QString&, const QStringList&);

private slots:

    void newConnect( ClientSocket *s );
    void connectionClose();
    void connectionClosed();
    void login(const QString& txt);
    void password(const QString & txt);
    void processText(const QString & txt);
    void closeAll();
    void closePort(int);
    void shutdown();

private:
    NameMap map_name;
    SocketMap map_socket;
    SocketMap map_pending;
    HostMap map_host;
    QStringList current_history;
    QDateTime startDateTime;

    PortsMap map_ports;

    DateMap map_idle;

    bool loadHistory();
    void addToHistory(const QString&);
    void accept(const QString&, ClientSocket*);
    void sendLogo(ClientSocket*);
    void updateHosts(const QString&);
    QString serverDisplayPrefix(const QString&);
    QString formatUptime();
};

#endif
