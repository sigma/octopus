/*  Time-stamp: <16/03/2005 23:57:25 Yann Hodique>  */

/**
 *  @file serversocket.h
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

#ifndef _SERVERSOCKET_H_
#define _SERVERSOCKET_H_

#include <QTcpServer>
#include <QQueue>
#include <QMap>

class ClientSocket;

class ServerSocket : public QTcpServer {
    Q_OBJECT

public:
    ServerSocket(QObject *parent = 0);

    ~ServerSocket();

    virtual bool hasPendingConnections() const;

    virtual QTcpSocket * nextPendingConnection();

    bool send(const QString & to, const QString & msg);
    void broadcast(const QString & msg);
    void broadcastOthers(const QString & except, const QString & msg);
    void broadcastOthers(const QStringList & except, const QString & msg);

protected:
    virtual void incomingConnection (int socketDescriptor);

private:
    QQueue<ClientSocket*> pending;
    QList<ClientSocket*> active;

    QMap<QString, ClientSocket*> users;
};	// end of class ServerSocket


#endif /* _SERVERSOCKET_H_ */
