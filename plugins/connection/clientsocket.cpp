/*  Time-stamp: <18/03/2005 21:26:21 Yann Hodique>  */

/**
 *  @file clientsocket.cpp
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

#include "clientsocket.h"

ClientSocket::ClientSocket(QObject *parent) : QTcpSocket(parent) {}

ClientSocket::~ClientSocket() {}

QString ClientSocket::readLineData() {
    QString res(readLine());
    if(res.endsWith("\r\n"))
        return res.left(res.size() - 2);
    else if(res.endsWith("\n"))
        return res.left(res.size() - 1);
    else
        return res;
}
