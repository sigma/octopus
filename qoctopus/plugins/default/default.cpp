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

#include "default.h"

#define DEFAULT_CHANNEL "Hall"

PLUGIN_FACTORY(Default);

Default::Default(PluginManagerInterface* parent) : DefaultPlugin(parent) {
    // Default channel
    Channel channel(DEFAULT_CHANNEL);

    // Load the default topic
    QFile file(manager()->dataDir() + "/topic/" + DEFAULT_CHANNEL);
    if (!file.open(IO_ReadOnly)) {
        std::cerr << "Could not read topic file" << std::endl;
    } else {
	QTextStream stream(&file);
	QString line;
	if ((line = stream.readLine()) != QString::null)
	    channel.topic = line;
	file.close();
    }

    channels.insert(DEFAULT_CHANNEL, channel);

}

Default::~Default() {}

void Default::modifiedInternalVariable(const QString& /*login*/, const QString& /*name*/, const QString& /*value*/) {
}

void Default::manageMessage(const QString& author, const QString& msg) {
    QString s("<" + author + "> " + msg);
    //    std::cout << "[Default] : <" << author << "> " << msg << std::endl;
    broadcastChannel(findChannel(author),s);
    //    manager()->connectionPlugin()->broadcast(s);
}

void Default::broadcastChannel(const QString& channel, const QString& msg) {
    for(QStringList::Iterator it = channels[channel].users.begin(); it != channels[channel].users.end(); ++it)
        manager()->connectionPlugin()->send(*it,msg);
}

void Default::serverBroadcastChannel(const QString& channel, const QString& msg) {
    for(QStringList::Iterator it = channels[channel].users.begin(); it != channels[channel].users.end(); ++it)
        manager()->connectionPlugin()->serverSend(*it,msg);
}

void Default::broadcastChannelOthers(const QString& except, const QString& channel, const QString& msg) {
    for (QStringList::Iterator it = channels[channel].users.begin(); it != channels[channel].users.end(); ++it)
	if (*it != except)
	    manager()->connectionPlugin()->send(*it, msg);
}

void Default::serverBroadcastChannelOthers(const QString& except, const QString& channel, const QString& msg) {
    for (QStringList::Iterator it = channels[channel].users.begin(); it != channels[channel].users.end(); ++it)
	if (*it != except)
	    manager()->connectionPlugin()->serverSend(*it, msg);
}

void Default::incomingUser(const QString& login) {
    channels[DEFAULT_CHANNEL].users << login;

    // Send it the topic
    if (channels[DEFAULT_CHANNEL].topic != "")
        topicCmd(login,"");
}

void Default::outgoingUser(const QString& login) {
  QString channel = findChannel(login);
  channels[channel].users.remove(login);
  if(!channels[channel].users.count())
    channels.remove(channel);
}

void Default::renamedUser(const QString& old_login, const QString& new_login) {
    for(QMap<QString,Channel>::Iterator it = channels.begin(); it != channels.end(); ++it) {
	if((*it).users.find(old_login) != (*it).users.end()) {
	    (*it).users.remove(old_login);
	    (*it).users << new_login;
	}
    }
}

void Default::exportCommands() {
    registerCommand("join",&Default::joinCmd,"{<word>}","Join a channel");
    registerCommand("leave",&Default::leaveCmd,QString::null,"Return to the default channel");
    registerCommand("channels",&Default::channelsCmd,QString::null,"Display the available channels");
    registerCommand("shout",&Default::shoutCmd,"{<sentence>}","Shout through the channels");
    registerCommand("who",&Default::whoCmd,"[{all|<word>}]","Display online users");
    registerCommand("emote",&Default::emoteCmd,"{<sentence>}","Display \"emotions\"","Emote");
    registerCommand("topic",&Default::topicCmd,"[{<sentence>}]","Change or display the topic","Topic");
}

void Default::joinCmd(const QString& from, const QStringList& list) {
    QString channel = findChannel(from);
    if(channel != list[0]) {
        channels[channel].users.remove(from);
        if(!channels[channel].users.count())
            channels.remove(channel);
        else {
            QString txt1(from + " joins channel " + list[0]);
            serverBroadcastChannel(channel,txt1);
        }

        QString txt2(from + " comes in from channel " + channel);
        QString txt3("You join channel " + list[0]);

        serverBroadcastChannel(list[0],txt2);
        manager()->connectionPlugin()->serverSend(from,txt3);
        channels[list[0]].users << from;
        // display the channel topic
        if (channels[list[0]].topic != "")
            topicCmd(from, "");
    } else {
        QString txt("You are already in channel " +  channel);
        manager()->connectionPlugin()->serverSend(from,txt);
    }
}

void Default::leaveCmd(const QString& from, const QStringList&) {
	QString cmd(DEFAULT_CHANNEL);
	joinCmd(from, cmd);
}

void Default::channelsCmd(const QString& from, const QStringList&) {
	QString txt("Channel  Users                     Topic\n"
                "-------- ----- ------------------------------------------------");
	int count = 0;
    for(QMap<QString,Channel>::Iterator it = channels.begin(); it != channels.end(); ++it,++count) {
        //txt += "\n" + it.key() + "(" + QString::number(it.data().users.count()) + ")";
	    txt += QString("\n%1 %2 %3").arg(it.key().left(8),-8).arg(QString::number(it.data().users.count()),5).arg(it.data().topic.left(48),-48);
    }
    manager()->connectionPlugin()->send(from,txt);
	txt = QString("There ") + (count-1?"are":"is") + " currently " + QString::number(count) + " channel" + (count-1?"s":"");
    manager()->connectionPlugin()->serverSend(from, txt);
}

void Default::shoutCmd(const QString& from, const QStringList& list) {
    QString txt1(from + " shouts: " + list[0]);
    manager()->connectionPlugin()->serverBroadcastOthers(from,txt1);
	manager()->logPlugin()->write("shout", txt1);

    QString txt2("You shout: " + list[0]);
    manager()->connectionPlugin()->serverSend(from,txt2);
}

void Default::whoCmd(const QString& from, const QStringList& list) {
    int count = 0;
    bool unique = true;
    QString channel;
    QString txt(" Login   Channel   Idle  On For             From host               \n"
                "-------- -------- ------ ------ ------------------------------------");

    QStringList l;
    if(list[0] == "")
        l << findChannel(from);
    else if(list[0] == "all") {
        for(QMap<QString,Channel>::Iterator iter = channels.begin(); iter != channels.end(); ++iter)
            l << iter.key();
        unique = false;
    } else {
        if(channels.find(list[0]) != channels.end())
            l << list[0];
        else {
            QString txt("Channel " + list[0] + " does not exist!");
            manager()->connectionPlugin()->serverSend(from,txt);
            return;
        }
    }

    for(QStringList::Iterator iter = l.begin(); iter != l.end(); ++iter) {
        channel = *iter;
	    QStringList users = channels[channel].users;
        for(QStringList::Iterator it = users.begin(); it != users.end(); ++it, count++) {
            if (txt != "")
                txt += "\n";
            QDateTime dateTime = manager()->connectionPlugin()->idleFor(*it);
            QString idle = "<no date>";
            if (dateTime.isValid()) {
                idle = formatIdle(dateTime);
//			dateTime.toString("hh:mm");
            }

            dateTime = manager()->connectionPlugin()->connectedFor(*it);
            QString onfor = "<no date>";
            if (dateTime.isValid()) {
                onfor = formatIdle(dateTime);
//			dateTime.toString("hh:mm");
            }

            txt += QString("%1 %2 %3 %4 %5")
                .arg(*it,-8)
                .arg(channel.left(8),-8)
                .arg(idle, 6)
                .arg(onfor, 6)
                .arg(manager()->connectionPlugin()->host(*it).left(36),-36);
            /*		txt += *it;
              for(int i=(*it).length(); i<manager()->databasePlugin()->validLoginLength() + 1; i++) txt += " ";
              txt += channel;
              for(int i=channel.length(); i<manager()->databasePlugin()->validLoginLength() + 1; i++) txt += " ";
              txt += manager()->connectionPlugin()->host(*it) + "\n";*/
        }
    }
	manager()->connectionPlugin()->send(from, txt);
	txt = QString("There ") + (count>1?"are":"is") + " currently " + QString::number(count)
        + " user" + (count>1?"s":"") + (unique?(" in channel " + channel):"");
    manager()->connectionPlugin()->serverSend(from, txt);
}

void Default::emoteCmd(const QString& from, const QStringList& list) {
    QString txt1("*" + from + " " + list[0] + "*");
    serverBroadcastChannel(findChannel(from),txt1);
}

void Default::topicCmd(const QString& from, const QStringList& list) {
    QString arg = list[0];
    QString theChannel = findChannel(from);
    arg = arg.stripWhiteSpace();
    if (arg == "") {
        // Show the current channel topic
        if (channels[theChannel].topic != "")
            manager()->connectionPlugin()->serverSend(from, theChannel + " topic : " + channels[theChannel].topic);
        else
            manager()->connectionPlugin()->serverSend(from, "there is no topic for " + theChannel);
    } else {
        // Set the current channel topic
        channels[findChannel(from)].topic = arg;
        QString txt = from + " sets the current topic to " + arg;
        serverBroadcastChannelOthers(from, findChannel(from), txt);
        manager()->logPlugin()->write("topic", "[" + findChannel(from) + "] " + txt);
        manager()->connectionPlugin()->serverSend(from, "You set the current topic to " + arg);
    }
}

const QString& Default::findChannel(const QString & user) {
    for(QMap<QString,Channel>::Iterator it = channels.begin(); it != channels.end(); ++it) {
        if((*it).users.find(user) != (*it).users.end()) {
            return it.key();
        }
    }
    return QString::null;
}

QString Default::formatIdle(const QDateTime &connectedFor) {
    QString string;
    QDateTime currentDateTime = QDateTime::currentDateTime();
    QTime time;
    int days, seconds, hours, minutes;

    seconds = connectedFor.secsTo(currentDateTime);
    days = seconds / (3600 * 24);
    hours = seconds / 3600;
    minutes = seconds / 60;
    if (days) {
	time.setHMS((seconds - days * 24 * 3600) / 3600, 0, 0);
	string = QString::number(days) + "d" + time.toString("hh") + "h";
    } else {
	if (seconds >= 3600) {
	    time.setHMS(0, (seconds - hours * 3600) / 60, 0);
	    string = QString::number(hours) + "h" + time.toString("mm") + "m";
	} else if (seconds >= 60) {
	    time.setHMS(0, 0, seconds - minutes * 60);
	    string = QString::number(minutes) + "m" + time.toString("ss") + "s";
	} else {
	    time.setHMS(0, 0, seconds);
	    string = time.toString("s") + "s";
	}
    }

    return string;
}

