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

#ifndef CONNECTION_H
#define CONNECTION_H

#include <string>

#ifdef WIN32
#include <winsock2.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#endif

using std::string;

#ifndef WIN32
#define SOCKET int
#define SOCKET_ERROR (-1)
#define INVALID_SOCKET (SOCKET)(~0)
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr *LPSOCKADDR;
typedef struct timeval TIMEVAL;
#endif

class cConnection {

public:
  static cConnection * Factory(SOCKET sock);
  void PutSocketInFD(fd_set * set);
  bool IsSocketInFD(fd_set * set);
  bool Read();

private:
  cConnection() {};

  SOCKET m_socket;

  string m_inputBuffer;
  string m_outputBuffer;
};

#endif // CONNECTION_H
