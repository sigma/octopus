/*  Time-stamp: <16/03/2005 23:21:59 Yann Hodique>  */

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
