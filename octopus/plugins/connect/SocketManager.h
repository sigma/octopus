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

#ifndef SOCKETMANAGER_H
#define SOCKETMANAGER_H

#ifdef WIN32 
#include <winsock2.h>
#endif

#include <iostream>
#include <list>

#include "Connection.h"
#include "UserManager.h"

using std::list;
using std::cout;
using std::endl;

#ifndef WIN32
//#define SOCKET int
#endif

class SocketManager {

public:
  static SocketManager * Factory(int port, UserManager *userManager);
  SocketManager() {};
  ~SocketManager();
  bool DoIO();

  void* operator new(size_t s) {
    void *address = malloc(s);
    cout << "Socket manager creation" << endl;  
    return address;
  }

private:
  void SetupFDSets();

  SOCKET m_listeningSocket;
  fd_set ReadFDs;
  fd_set WriteFDs;
  fd_set ExceptFDs;

  list<cConnection*> m_connections;
  UserManager *m_userManager;
};

#endif // SOCKETMANAGER_H
