/*********************************************************************************
 *   Copyright (C) 2003 by Octopus Authors                                       *
 *   octopus-devel@nongnu.org                                                    *
 *                                                                               *
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

/**
 * @file   ConnectionPlugin.h
 * @author Yann Hodique
 * @date   Sat Mar  1 15:44:56 2003
 * 
 * @brief  Interface for a connection plugin
 * 
 * 
 */

#ifndef _CONNECTIONPLUGIN_H_
#define _CONNECTIONPLUGIN_H_

#include "Plugin.h"

class ConnectionPlugin : public Plugin {

    Q_OBJECT

public:
    /**
     * Constructor.
     * 
     * @param parent Associated plugin manager 
     * 
     */
    ConnectionPlugin(PluginManagerInterface* parent=0) : Plugin(parent) {}

    /**
     * Destructor
     * 
     * 
     */
    virtual ~ConnectionPlugin() {}

    /**
     * Launches server socket.
     * 
     * @param port 
     */
    virtual void launch(int port) = 0;

    /**
     * Sends a message to somebody.
     * 
     * @param to name of the destination.
     * @param msg message to send.
     * 
     * @return true if the message was successfully sent.
     */
    virtual bool send(const QString & to, const QString & msg) = 0;

    /**
     * Broadcasts a message to everybody.
     * 
     * @param msg message to broadcast.
     */
    virtual void broadcast(const QString & msg) = 0;

    /**
     * Broadcasts a message to nearly everybody.
     * 
     * @param except exception to the rule.
     * @param msg message to broadcast.
     */
    virtual void broadcastOthers(const QString & except, const QString & msg) = 0;


    /**
     * Broadcasts a message to nearly everybody.
     * 
     * @param except exceptions to the rule.
     * @param msg message to broadcast.
     */
    virtual void broadcastOthers(const QStringList & except, const QString & msg) = 0;

    /**
     * Sends a message to somebody from the server.
     * 
     * @param to name of the destination.
     * @param msg message to send.
     * 
     * @return true if the message was successfully sent.
     */
    virtual bool serverSend(const QString & to, const QString & msg) = 0;

    /**
     * Broadcasts a message to everybody from the server.
     * 
     * @param msg message to broadcast.
     */
    virtual void serverBroadcast(const QString & msg) = 0;

    /**
     * Broadcasts a message to nearly everybody from the server.
     * 
     * @param except exception to the rule.
     * @param msg message to broadcast.
     */
    virtual void serverBroadcastOthers(const QString & except, const QString & msg) = 0;


    /**
     * Broadcasts a message to nearly everybody from the server.
     * 
     * @param except exceptions to the rule.
     * @param msg message to broadcast.
     */
    virtual void serverBroadcastOthers(const QStringList & except, const QString & msg) = 0;
    
    /**
     * Determines the host of a correspondant.
     * 
     * @param login identity of the correspondant.
     * 
     * @return host name.
     */
    virtual QString host(const QString& login) const = 0;
    
    /**
     * Return the user connection date and time.
     *
     * @param login identity of the correspondant.
     *
     * @return the user connection date and time.
     */
    virtual QDateTime connectedFor(const QString &login) const = 0;

    /**
     * Return the user's last sentence date and time.
     *
     * @param login identity of the correspondant.
     *
     * @return the user's last sentence date and time.
     */
    virtual QDateTime idleFor(const QString &login) const = 0;
    
    /**
     * Return the server start date and time.
     *
     * @return the server start date and time.
     */
    virtual QDateTime serverStartDateTime() const = 0;
};

#endif
