/*  Time-stamp: <18/03/2005 21:24:15 Yann Hodique>  */

/**
 *  @file clientsocket.h
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

#ifndef _CLIENTSOCKET_H_
#define _CLIENTSOCKET_H_

#include <QTcpSocket>

class ClientSocket : public QTcpSocket {
    Q_OBJECT

public:
    ClientSocket(QObject *parent = 0);
    ~ClientSocket();

    void setLogin(const QString& l) {login = l;}
    QString getLogin() {return login;}

    QString readLineData();

private:
    QString login;
};	// end of class ClientSocket


#endif /* _CLIENTSOCKET_H_ */
