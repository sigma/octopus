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

#include "DebugUtils.h"
#include "Server.h"
#include "Utils.h"

using std::cout;
using std::endl;

#define __FILENAME__ "Server.cpp"

PLUGIN_FACTORY(cServer)

/*cServer *cServer::Factory(int port) 
{
  cServer *newServer = new cServer();
  if (newServer == NULL) {
    ERR << "Cannot create object" << endl;
    return NULL;
  }

  // User manager 
  newServer->userManager = new UserManager;
  if (newServer->userManager == NULL) {
    ERR << "User manager creation failed" << endl;
    delete newServer;
    return NULL;
  } 

  // Socket manager
  newServer->socketManager = SocketManager::Factory(port, newServer->userManager);
  if (newServer->socketManager == NULL) {
    ERR << "Socket manager creation failed" << endl;
    delete newServer;
    return NULL;
  }

  newServer->displayStartInfos();

  return newServer;
}*/

cServer::cServer() 
{
  userManager = NULL;
  socketManager = NULL;  
}

cServer::~cServer() 
{
  if (socketManager != NULL) 
    delete socketManager;
  if (userManager != NULL)
    delete userManager;

  displayEndInfos();
}

void cServer::displayStartInfos() 
{
  char *more = charToString('-', strlen(VERSION));
 
  cout << "--------------------" << more << endl;
  cout << "Octopus Chat System " << VERSION << endl;
  cout << "--------------------" << more << endl << endl;

  delete[] more;
}

void cServer::displayEndInfos() 
{
  cout << "closed\n";
}

void cServer::launch(int port)
{
    this->userManager = new UserManager;
    this->socketManager = SocketManager::Factory(port, this->userManager);
    this->displayStartInfos();
}

void cServer::callCommand(int, string&) {}

void cServer::exportCommands() {}
