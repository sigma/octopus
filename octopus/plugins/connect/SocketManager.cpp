/*********************************************************************************
 *   This file is part of Octopus.                                               *
 *                                                                               *
 *   Octopus is free software; you can redistribute it and/or modify             *
 *   it under the terms of the GNU General Public License as published by        *
 *   the Free Software Foundation; either version 2 of the License, or           *
 *   (at your option) any later version.                                         *
 *                                                                               *
 *   Octopus is distributed in the hope that it will be useful,                  *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of              *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               *
 *   GNU General Public License for more details.                                *
 *                                                                               *
 *   You should have received a copy of the GNU General Public License           *
 *   along with Octopus; if not, write to the Free Software                      *
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA   *
 *********************************************************************************/

#include "SocketManager.h"
#include "DebugUtils.h"

#include <iostream>

using namespace std;

#define __FILENAME__ "SocketManager.cpp"

SocketManager* SocketManager::Factory(int port, UserManager *userManager) {

  if(userManager == NULL) {
    ERR << "userManager is NULL";
    return NULL;
  }

  SocketManager * newSocketManager = new SocketManager();
  if(newSocketManager == NULL) {
    ERR << "cannot create SocketManager" << endl;
    return NULL;
  } 

  newSocketManager->m_userManager = userManager;

#ifdef WIN32
  WSADATA wsaData;
  if(WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
    ERR << "WSAStartup failed" << endl;
    delete newSocketManager;
    return NULL;
  }
#endif

  newSocketManager->m_listeningSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if(newSocketManager->m_listeningSocket == INVALID_SOCKET) {
    ERR << "socket failed" << endl;
    return NULL;
  }

	SOCKADDR_IN saServer;
	
	saServer.sin_family = AF_INET;
	saServer.sin_addr.s_addr = INADDR_ANY;
	saServer.sin_port = htons(port);

  if(bind(newSocketManager->m_listeningSocket, (LPSOCKADDR)(&saServer), sizeof(saServer)) == SOCKET_ERROR) {
    ERR << "bind failed" << endl;
    delete newSocketManager;
    return NULL;
  }
  
  if(listen(newSocketManager->m_listeningSocket, SOMAXCONN) == SOCKET_ERROR) {
    ERR << "listen failed" << endl;
    delete newSocketManager;
    return NULL;
  }

/*  newSocketManager->m_connections = cChainedList::Factory();
  if(newSocketManager->m_connections == NULL) {
    ERR << "cannot create ChainedList" << endl;
    delete newSocketManager;
    return NULL;
  } */

  return newSocketManager;
}

SocketManager::~SocketManager() {
#ifdef WIN32
  WSACleanup();
#endif
}

bool SocketManager::DoIO() {

  SetupFDSets();

  int ready;
  TIMEVAL timeout = {0, 0};
  ready = select(0, &ReadFDs, &WriteFDs, &ExceptFDs, &timeout);
  if(ready == 0) return true;
  if(ready == SOCKET_ERROR) {
    ERR << "select failed" << endl;
    return false;
  }

  if(FD_ISSET(m_listeningSocket, &ReadFDs)) {
   
    sockaddr_in sinRemote;
    socklen_t addrSize = sizeof(sinRemote);

    SOCKET sock = accept(m_listeningSocket, (sockaddr*)(&sinRemote), &addrSize);
    if(sock == INVALID_SOCKET) {
      ERR << "accept failed" << endl;
      return false;
    }

    cConnection * newConnection = cConnection::Factory(sock);
    if(newConnection == NULL) {
      ERR << "cannot create new connection";
      return false;
    }

    m_connections.push_back(newConnection);  
//    m_connections.Add(newConnection);
  } else {

    if(FD_ISSET(m_listeningSocket, &ExceptFDs)) {
      ERR << "exception occured on listening socket";
      return false;
    }
  }

  list<cConnection*>::iterator i = m_connections.begin();
  while (i != m_connections.end()) {
    if ((*i)->IsSocketInFD(&ReadFDs)) {
      (*i)->Read();
    }
    
    i++;
  }

  return true;
}

void SocketManager::SetupFDSets() {

  FD_ZERO(&ReadFDs);
  FD_ZERO(&WriteFDs);
  FD_ZERO(&ExceptFDs);

  FD_SET(m_listeningSocket, &ReadFDs);
  FD_SET(m_listeningSocket, &ExceptFDs);

  list<cConnection*>::iterator i = m_connections.begin();
  while (i != m_connections.end()) {
    (*i)->PutSocketInFD(&ReadFDs);
    (*i)->PutSocketInFD(&WriteFDs);
    (*i)->PutSocketInFD(&ExceptFDs);
    i++;
  }
}
