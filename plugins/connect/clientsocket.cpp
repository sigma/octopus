/***************************************************************************
 *  file                 : clientsocket.cpp                                *
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

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include <iostream>
#include <qtextstream.h>

#include "clientsocket.h"
#include "debug.h"

 ClientSocket::ClientSocket( int sock, QObject *parent, const char *name ) :
         Q3Socket( parent, name ) {
   connect( this, SIGNAL(readyRead()),
        SLOT(readClient()) );
   connect( this, SIGNAL(connectionClosed()),
        SLOT(deleteLater()) );
   connect( this, SIGNAL(delayedCloseFinished()),
        SLOT(deleteLater()) );
   setSocket( sock );
 }

 ClientSocket::~ClientSocket() {
     octInfo("~ClientSocket()\n");
 }

 void ClientSocket::readClient() {
   while ( canReadLine() ) {
       QString str = extractCommands(QString(readLine()).remove("\r\n").remove('\n'));
       emit processText(str);
   }
 }

 QString ClientSocket::peerHostName() {
     Q_UINT32 ip(peerAddress().ip4Addr());
     char *p_ip = (char*)&ip;
     char ip2[sizeof(Q_UINT32)];
     for(uint i = 0; i<sizeof(Q_UINT32); i++)
     ip2[i] = p_ip[sizeof(Q_UINT32)-i-1];
     struct hostent *host = gethostbyaddr((const char*)ip2,sizeof(Q_UINT32),AF_INET);
     QString h(host?host->h_name:"");
     if(!h.length())
         h = peerAddress().toString();
     return h;
 }

 QString ClientSocket::extractCommands(const QString & msg) {
     QString str(msg);
 //    return str.replace(tel_re,"");
     for(int i = 0; i != -1;) {
         i = str.find('ÿ',i);
         if(i != -1) {
             if(str[i+1] == 'ÿ') {
                 str.remove(i,1);
                 i++;
             } else {
                 int com = ((str[i+1].latin1()) << 8) + str[i+2].latin1();
                 emit telnetCommand(com);
                 str.remove(i,3);
             }
         }
     }
    return str;
}
