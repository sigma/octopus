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

#ifndef _DEFAULT_H_
#define _DEFAULT_H_

#include "plugins.h"
#include "channel.h"

class Default : public DefaultPlugin {
    Q_OBJECT
    PLUGIN_OBJECT(Default)
public:
    Default(PluginManagerInterface* parent=0);

    ~Default();

  void modifiedInternalVariable(const QString&, const QString&, const QString&);

    void manageMessage(const QString&, const QString&);
    void broadcastChannel(const QString& channel, const QString& msg);
    void serverBroadcastChannel(const QString& channel, const QString& msg);
    void broadcastChannelOthers(const QString& except, const QString& channel, const QString& msg);
    void serverBroadcastChannelOthers(const QString& except, const QString& channel, const QString& msg);
    void incomingUser(const QString&);
    void outgoingUser(const QString&);
    void renamedUser(const QString&, const QString&);

    void exportCommands();

    void joinCmd(const QString&, const QStringList&);
    void leaveCmd(const QString&, const QStringList&);
    void channelsCmd(const QString&, const QStringList&);
    void shoutCmd(const QString&, const QStringList&);
    void whoCmd(const QString&, const QStringList&);
    void emoteCmd(const QString&, const QStringList&);
    void topicCmd(const QString&, const QStringList&);

private:
    QMap<QString,Channel> channels;

    QString findChannel(const QString & user) const;
    QString formatIdle(const QDateTime &connectedFor);
};

#endif
