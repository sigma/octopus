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

#include <iostream>

#include "Connection.h"
#include "DebugUtils.h"

using namespace std;

#define __FILENAME__ "Connection.cpp"

cConnection * cConnection::Factory(SOCKET socket) {

  cConnection * newConnection = new cConnection();
  if(newConnection == NULL) return NULL;

  newConnection->m_socket = socket;

  return newConnection;
}

void cConnection::PutSocketInFD(fd_set * set) {

  FD_SET(m_socket, set);
}

bool cConnection::IsSocketInFD(fd_set * set) {

  return FD_ISSET(m_socket, set) ? true : false;
}

bool cConnection::Read() {

  const static int len = 1024;
  char buffer[len];

  int length = recv(m_socket, buffer, len - 1, 0);
  if(length == 0) {
    // socket has been closed
    return false;
  }
  if(length == SOCKET_ERROR) {
    ERR << "socket error" << endl;
    return false;
  }

  buffer[length] = '\0';
  m_inputBuffer += buffer;

  cout << m_inputBuffer << endl;

  return true;
}
