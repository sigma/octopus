/*  Time-stamp: <16/03/2005 22:47:30 Yann Hodique>  */

/**
 *  @file connection.h
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

#ifndef _CONNECTION_H_
#define _CONNECTION_H_

#include "plugins.h"

#include <QDateTime>

class Connection : public ConnectionPlugin {
    Q_OBJECT
    PLUGIN_OBJECT(Connection)
public:

    Connection(PluginManagerInterface* parent=0);
    ~Connection();

    void launch(int port);

    bool send(const QString & to, const QString & msg);
    bool serverSend(const QString & to, const QString & msg);

    void broadcast(const QString & msg);
    void broadcastOthers(const QString & except, const QString & msg);
    void broadcastOthers(const QStringList & except, const QString & msg);
    void serverBroadcast(const QString & msg);
    void serverBroadcastOthers(const QString & except, const QString & msg);
    void serverBroadcastOthers(const QStringList & except, const QString & msg);

    void exportCommands();

    QString host(const QString& login) const;

    QDateTime connectedFor(const QString &login) const;
    QDateTime idleFor(const QString &login) const;
    QDateTime serverStartDateTime() const;

    void incomingUser(const QString& login);
    void outgoingUser(const QString& login);
    void renamedUser(const QString&, const QString&);
    void killedUser(const QString& login);
};

#endif /* _CONNECTION_H_ */
