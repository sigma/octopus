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

////////////////////////////////////////////////////////////////////////////////
//
// Server is the main chat server
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _SERVER_H_
#define _SERVER_H_

#include "Version.h"
#include "SocketManager.h"
#include "UserManager.h"
#include "ConnectionPlugin.h"

class cServer : public ConnectionPlugin {
private:
    SocketManager *socketManager;
    UserManager *userManager;

    //  cServer();

    // Display server start infos
    void displayStartInfos();
    void displayEndInfos();
public:
    cServer();
    //  static cServer *Factory(int port);

    ~cServer();

    // Launch the server
    void launch(int port);
    void callCommand(int, string&);

protected:
    void exportCommands();
};

#endif  // _SERVER_H_
