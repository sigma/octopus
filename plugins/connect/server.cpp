/***************************************************************************
 *  file                 : server.cpp                                      *
 *  begin                : Tuesday, April 22, 2003                         *
 *                           -------------------                           *
 *  copyright            : Yann Hodique                                    *
 *  email                : Yann.Hodique@lifl.fr                            *
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

/****************************************************************************
**
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include <qtextstream.h>

#include "server.h"
#include "simpleserver.h"
#include "clientsocket.h"

#define ECHO_ON "ÿü"
#define ECHO_OFF "ÿû"

PLUGIN_FACTORY(Connect);

Connect::Connect(PluginManagerInterface* parent) : ConnectionPlugin(parent) {
    loadHistory();
    // Set the start time
    startDateTime = QDateTime::currentDateTime();
}

Connect::~Connect() {
    for(PortsMap::Iterator it = map_ports.begin(); it != map_ports.end(); ++it)
        delete it.value();
}

void Connect::incomingUser(const QString& login) {
    ClientSocket* socket(map_name[login][map_name[login].count() - 1]);
    QString h = socket->peerHostName();
    map_host[login] << h;

    QString message(QDate::currentDate().toString("dd/MM/yyyy ") + QTime::currentTime().toString("hh:mm:ss IN  "));

    message += QString("%1 %2").arg(login,-(manager()->databasePlugin()->validLoginLength())).arg(h,-30);
    addToHistory(message);

    map_idle[login] = QDateTime::currentDateTime();
}

void Connect::outgoingUser(const QString& login) {
    QString message(QDate::currentDate().toString("dd/MM/yyyy ") + QTime::currentTime().toString("hh:mm:ss OUT "));
    message += QString("%1 %2").arg(login,-(manager()->databasePlugin()->validLoginLength())).arg(host(login),-30);
    addToHistory(message);

    map_host.remove(login);
}

void Connect::renamedUser(const QString& old_login, const QString& new_login) {
    NameMap::Iterator it1 = map_name.find(old_login);
    if(it1 != map_name.end()) {
        HostList l = *it1;
        map_name.erase(it1);
        map_name.insert(new_login,l);
    }

    QList<SocketMap::Iterator> iterators;
    for(SocketMap::Iterator it2 = map_socket.begin(); it2 != map_socket.end(); ++it2) {
        if(*it2 == old_login) {
            iterators << it2;
        }
    }
    for(QList<SocketMap::Iterator>::Iterator iter = iterators.begin(); iter != iterators.end(); ++iter) {
        ClientSocket* socket = (*iter).key();
        map_socket.erase(*iter);
        map_socket.insert(socket,new_login);
    }

    HostMap::Iterator it3 = map_host.find(old_login);
    if(it3 != map_host.end()) {
        QStringList l = *it3;
        map_host.erase(it3);
        map_host.insert(new_login,l);
    }

    map_idle[new_login] = map_idle[old_login];
    map_idle.remove(old_login);
}

bool Connect::loadHistory() {
    QFile fin(manager()->dataDir() + "/history");
    if (!fin.open(QIODevice::ReadOnly)) {
        std::cerr << "Could not read history file" << std::endl;
        return false;
    }

    QTextStream stream(&fin);
    QString line;
    QStringList list;
    while((line = stream.readLine()) != QString::null)
        list << line;

    if(list.count() <= HISTORY_LINES)
        current_history = list;
    else {
        QStringList::Iterator it = list.end();
        --it;
        for(int i=0; i<HISTORY_LINES; i++,--it)
            current_history.prepend(*it);
    }
    fin.close();
    return true;
}

void Connect::addToHistory(const QString& msg) {
    QFile fout(manager()->dataDir() + "/history");
    if( !fout.open(QIODevice::WriteOnly | QIODevice::Append)) {
        std::cerr << "Could not write the history file." << std::endl;
        return;
    }

    QTextStream stream(&fout);
    stream << (msg + "\n");
    fout.close();

    current_history << msg;
    if(current_history.count() > HISTORY_LINES)
        current_history.erase(current_history.begin());
}

void Connect::quitCmd(const QString& from, const QStringList& list) {
    QString txt1(from + " leaves " + ((list[0] == "") ? "" : ("(" + list[0] + ")")) + "!");
    QString txt2("You leave!");
    serverBroadcastOthers(from,txt1);
    serverSend(from,txt2);

    HostList l = (*(map_name.find(from)));
    for(HostList::Iterator it = l.begin(); it != l.end(); ++it) {
        (*it)->close();
    }
}

void Connect::kickCmd(const QString& from, const QStringList& list) {
    QString to(manager()->databasePlugin()->canonicalName(list[0]));
    QString txt1("%1 kicks you! %2");
    if(serverSend(to,txt1.arg(from).arg((list[1] == "")?"":("(" + list[1] + ")")))) {
        HostList l = (*(map_name.find(to)));
        for(HostList::Iterator it = l.begin(); it != l.end(); ++it) {
            disconnect(*it, SIGNAL(delayedCloseFinished()), this, SLOT(connectionClose()));
            (*it)->forceClose();
        }
        QString txt2 = "You kick " + to + " out !";
        if (list[1] != "")
            txt2 += " (" + list[1] + ")";
        serverSend(from, txt2);
        txt2 = "%1 is kicked out by %2";
        if (list[1] != "")
            txt2 += " (" + list[1] + ")";
        serverBroadcastOthers(from, txt2.arg(to).arg(from));
    } else {
        QString txt2(to + " is not connected!");
        serverSend(from,txt2);
    }
}

void Connect::forceCmd(const QString& from, const QStringList& list) {
    QString to(manager()->databasePlugin()->canonicalName(list[0]));
    if(map_name.find(to) != map_name.end()) {
        QString txt2("You force %1 to: %2");
        serverSend(from, txt2.arg(to).arg(list[1]));
        manager()->dispatch(to, list[1]);
    } else {
        QString txt2(to + " is not connected!");
        serverSend(from,txt2);
    }
}

void Connect::historyCmd(const QString& from, const QStringList&) {
    QString txt;
    if (current_history.count())
        txt = "History :\n" + current_history.join("\n") + "\n" + serverDisplayPrefix("End of History");
    else
        txt = "The history is empty";
    serverSend(from,txt);
}

void Connect::openCmd(const QString& from, const QStringList& list) {
    launch(list[0].toInt());
    if(map_ports.find(list[0].toInt()) != map_ports.end()) {
        QString txt("Port " + list[0] + " is now opened");
        serverSend(from,txt);
    } else {
        QString txt("Failed to open port " + list[0]);
        serverSend(from,txt);
    }
}

void Connect::closeCmd(const QString& from, const QStringList& list) {
    QString txt("Port " + list[0] + " is now closed");
    serverSend(from,txt);
    closePort(list[0].toInt());
}

void Connect::lsportsCmd(const QString& from, const QStringList&) {
    QString txt;
    serverSend(from, "Opened ports :");
    for(PortsMap::Iterator it = map_ports.begin(); it != map_ports.end(); ++it) {
        if (txt != "")
            txt += "\n";
        txt += QString::number(it.key());
    }
    send(from, txt);
    serverSend(from, "End of opened ports");
}

void Connect::uptimeCmd(const QString& from, const QStringList&) {
    QString txt("Uptime:\n");
    txt += "Start time   : " + startDateTime.toString("dddd MMMM d yyyy hh:mm:ss") + "\n";
    txt += "Current time : " + QDateTime::currentDateTime().toString("dddd MMMM d yyyy hh:mm:ss") + "\n";
    txt += "Up for       : " + formatUptime();
    serverSend(from, txt);
    serverSend(from, "End of uptime");
}

void Connect::lssessionsCmd(const QString& from, const QStringList&) {
    QString txt;
    serverSend(from, "Opened sessions :");
    int i = 0;
    for(QStringList::Iterator it = map_host[from].begin(); it != map_host[from].end(); ++it,++i) {
        if (txt != "")
            txt += "\n";
        txt += QString::number(i) + " : " + *it + "(" + map_name[from][i]->connectedFor.toString() + ")";
    }
    send(from, txt);
    serverSend(from, "End of opened sessions");
}

void Connect::closesessionCmd(const QString& from, const QStringList& list) {
    ClientSocket* socket = map_name[from][list[0].toInt()];
    disconnect(socket, SIGNAL(delayedCloseFinished()), this, SLOT(connectionClose()));
    socket->forceClose();
}

void Connect::exportCommands() {
    registerCommand("quit",&Connect::quitCmd,"[{<sentence>}]","Leave the server");
    registerCommand("kick",&Connect::kickCmd,"{<login>}[ {<sentence>}]","Kick a user","Kick");
    registerCommand("force",&Connect::forceCmd,"{<login>} {<sentence>}","Force a user to say something","Force");
    registerCommand("history",&Connect::historyCmd,QString::null,"Show in/out history");
    registerCommand("open",&Connect::openCmd,"{<int>}","Open a port","System");
    registerCommand("close",&Connect::closeCmd,"{<int>}","Close a port","System");
    registerCommand("lsports",&Connect::lsportsCmd,QString::null,"List open ports","System");
    registerCommand("uptime",&Connect::uptimeCmd,QString::null,"Display the server uptime");
    registerCommand("lssessions",&Connect::lssessionsCmd,QString::null,"List open sessions");
    registerCommand("closesession",&Connect::closesessionCmd,"{<int>}","Close a session");
}

QString Connect::host(const QString& login) const {
    return map_host[login].join(" ");
}

QDateTime Connect::connectedFor(const QString &login) const {
    QDateTime minDateTime;
    HostList hostList = map_name[login];
    for (HostList::Iterator it = hostList.begin(); it != hostList.end(); ++it) {
        if (!minDateTime.isValid())
            minDateTime = (*it)->connectedFor;
        else if (minDateTime > (*it)->connectedFor)
            minDateTime = (*it)->connectedFor;
    }
    return minDateTime;
}

QDateTime Connect::idleFor(const QString &login) const {
    return map_idle[login];
}

QDateTime Connect::serverStartDateTime() const {
    return startDateTime;
}

bool Connect::send(const QString & to, const QString & msg) {
    HostList l = map_name.value(to);
    if(!l.isEmpty()) {
        for(HostList::Iterator it = l.begin(); it != l.end(); ++it) {
            QTextStream(*it) << manager()->prepostPlugin()->treatOutgoingMessage(to,msg).replace(QChar('\n'),"\r\n") + "\r\n";
        }
        return true;
    } else {
        return false;
    }
}

void Connect::broadcastOthers(const QString & except, const QString & msg) {
    for (NameMap::Iterator it = map_name.begin(); it != map_name.end(); ++it)
        if(it.key() != except)
            for(HostList::Iterator iter = (*it).begin(); iter != (*it).end(); ++iter) {
                QTextStream(*iter) << manager()->prepostPlugin()->treatOutgoingMessage(it.key(),msg).replace(QChar('\n'),"\r\n") + "\r\n";
            }
}

void Connect::broadcastOthers(const QStringList & except, const QString & msg) {
    for (NameMap::Iterator it = map_name.begin(); it != map_name.end(); ++it)
        if(except.indexOf(it.key()) == -1)
            for(HostList::Iterator iter = (*it).begin(); iter != (*it).end(); ++iter) {
                QTextStream(*iter) << manager()->prepostPlugin()->treatOutgoingMessage(it.key(),msg).replace(QChar('\n'),"\r\n") + "\r\n";
            }
}

void Connect::broadcast(const QString & msg) {
    for (SocketMap::Iterator it = map_socket.begin(); it != map_socket.end(); ++it)
        QTextStream(it.key()) << manager()->prepostPlugin()->treatOutgoingMessage(it.value(),msg).replace(QChar('\n'),"\r\n") + "\r\n";
}

bool Connect::serverSend(const QString & to, const QString & msg) {
    return send(to, serverDisplayPrefix(msg));
}

void Connect::serverBroadcastOthers(const QString & except, const QString & msg) {
    broadcastOthers(except, serverDisplayPrefix(msg));
}

void Connect::serverBroadcastOthers(const QStringList & except, const QString & msg) {
    broadcastOthers(except, serverDisplayPrefix(msg));
}

void Connect::serverBroadcast(const QString & msg) {
    broadcast(serverDisplayPrefix(msg));
}

void Connect::killedUser(const QString& login) {
  NameMap::Iterator it = map_name.find(login);
  if(it != map_name.end()) {
    HostList l = *it;
    for(HostList::Iterator iter = l.begin(); iter != l.end(); ++iter) {
      disconnect(*iter, SIGNAL(delayedCloseFinished()), this, SLOT(connectionClose()));
      (*iter)->forceClose();
    }
  }
}

void Connect::launch(int port) {
    if(map_ports.find(port) == map_ports.end()) {
        SimpleServer *server = new SimpleServer(port);

        if(server->ok()) {
            connect( server, SIGNAL(newConnect(ClientSocket*)),
                     SLOT(newConnect(ClientSocket*)) );

            map_ports.insert(port,server);
        } else
            delete server;
    }
    if (!map_ports.count())
        exit(1);
}

void Connect::newConnect( ClientSocket *s ) {
    s->connectedFor = QDateTime::currentDateTime();
    std::cout << "New connection\n";
    connect( s, SIGNAL(connectionClosed()),
             SLOT(connectionClosed()) );
    connect( s, SIGNAL(delayedCloseFinished()),
             SLOT(connectionClose()) );
    connect( s, SIGNAL(simulateDisconnect()),
             SLOT(connectionClosed()));
    connect( s, SIGNAL(simulateClose()),
             SLOT(connectionClose()) );
    connect( s, SIGNAL(processText(const QString&)),
             SLOT(login(const QString&)));
    sendLogo(s);
    QTextStream(s) << serverDisplayPrefix("Login: ");
}

void Connect::connectionClose() {
    std::cout << "Server closed connection\n";
    QMap<ClientSocket*,QString>::Iterator it = map_socket.find((ClientSocket*)sender());
    if(it != map_socket.end()) {
        if(map_name[it.value()].count() == 1) {
            manager()->out(it.value());
            map_name.remove(it.value());
        } else {
            QString msg("%1 closes a session!");
            serverBroadcast(msg.arg(it.value()));
            map_name[it.value()].removeAll((ClientSocket*)sender());
            updateHosts(it.value());
        }
        map_socket.erase(it);
    } else
        map_pending.remove((ClientSocket*)sender());
}

void Connect::connectionClosed() {
    std::cout << "Client closed connection\n";

    QMap<ClientSocket*,QString>::Iterator it = map_socket.find((ClientSocket*)sender());
    if(it != map_socket.end()) {
        if(map_name[it.value()].count() == 1) {
            QString msg("%1 disconnects!");
            serverBroadcast(msg.arg(it.value()));
            manager()->out(it.value());
            map_name.remove(it.value());
        } else {
            QString msg("%1 closes a session!");
            serverBroadcast(msg.arg(it.value()));
            map_name[it.value()].removeAll((ClientSocket*)sender());
            updateHosts(it.value());
        }
        map_socket.erase(it);
    } else
        map_pending.remove((ClientSocket*)sender());
}

void Connect::login(const QString & txt) {
    QString name(manager()->databasePlugin()->canonicalName(txt));

    if(map_name.find(name) == map_name.end() || manager()->databasePlugin()->isRegisteredUser(name)) {
        if(!manager()->databasePlugin()->isValidLogin(name)) {
            QTextStream((ClientSocket*)sender()) << serverDisplayPrefix("Invalid Login!\r\n") << serverDisplayPrefix("Login: ");
            return;
        }
        disconnect(sender(), SIGNAL(processText(const QString&)),
                   this, SLOT(login(const QString&)));

        if(!manager()->databasePlugin()->isRegisteredUser(name)) {
            accept(name,(ClientSocket*)sender());
        } else {
            map_pending.insert((ClientSocket*)sender(),name);
            connect(sender(), SIGNAL(processText(const QString &)),
                    SLOT(password(const QString&)));
            QTextStream((ClientSocket*)sender()) << ECHO_OFF << serverDisplayPrefix("Password: ");
        }
    } else {
        QTextStream((ClientSocket*)sender()) << serverDisplayPrefix("Sorry, this login is already in use\r\n") << ECHO_ON << serverDisplayPrefix("Login: ");
    }
}

void Connect::password(const QString & txt) {
    ClientSocket* socket = (ClientSocket*)sender();
    QString login(*(map_pending.find(socket)));

    disconnect(socket, SIGNAL(processText(const QString&)),
               this, SLOT(password(const QString&)));

    if(manager()->databasePlugin()->authUser(login,txt)) {
        map_pending.remove(socket);

        accept(login,socket);
    } else {
        QTextStream(socket) << serverDisplayPrefix("Bad Password\r\n");
        socket->close();
    }
}

void Connect::processText(const QString & txt) {
    if(txt != "") {
        QString author = *(map_socket.find((ClientSocket*)sender()));

        map_idle[author] = QDateTime::currentDateTime();
        QString msg = txt;
        msg = manager()->prepostPlugin()->treatIncomingMessage(author,msg);
        if(msg != "") {
            QStringList l = msg.split('\n',QString::SkipEmptyParts);
            for(QStringList::Iterator it = l.begin(); it != l.end(); it++)
                manager()->dispatch(author, *it);
        }
    }
}

void Connect::closeAll() {
    for(PortsMap::Iterator it = map_ports.begin(); it != map_ports.end(); ++it) {
        it.value()->killall(this);
        delete it.value();
        map_ports.erase(it);
    }
    //    std::cout << "end of closeAll()" << std::endl;
}

void Connect::closePort(int port) {
    PortsMap::Iterator it = map_ports.find(port);
    if (it != map_ports.end()) {
        it.value()->killall(this);
        delete it.value();
        map_ports.erase(it);
    }
}

void Connect::shutdown() {
    closeAll();
    //    std::cout << "shutdown()" << std::endl;
}

void Connect::accept(const QString& login, ClientSocket* socket) {
    map_name[login] << socket;
    map_socket.insert(socket,login);
    map_pending.remove(socket);

    if(map_name[login].count() == 1) {
        QString welcome("Welcome, " + login + ".");
        QString in(login + " comes in!");
        send(login, ECHO_ON + serverDisplayPrefix(welcome));
        manager()->in(login);
        serverBroadcastOthers(login,in);
    } else {
        send(login, ECHO_ON + serverDisplayPrefix("Welcome, " + login + "."));
        serverSend(login, "You are already logged in " + QString::number(map_name[login].count()) + " time(s)");
        QString in(login + " opens a new session!");
        serverBroadcastOthers(login,in);
        incomingUser(login);
    }

    connect(socket, SIGNAL(processText(const QString &)),
            SLOT(processText(const QString&)));
}

void Connect::sendLogo(ClientSocket* s) {
    QFile fin(manager()->dataDir() + "/logo");
    if (!fin.open(QIODevice::ReadOnly)) {
        std::cerr << "Could not read logo file" << std::endl;
        return;
    }

    QTextStream stream(&fin);
    QString line, logo;
    while((line = stream.readLine()) != QString::null)
        logo += (line + "\n");
    fin.close();
    QTextStream(s) << logo.replace(QChar('\n'),"\r\n");
}

void Connect::updateHosts(const QString& login) {
    HostList l = map_name[login];
    QStringList h;
    for(HostList::Iterator it = l.begin(); it != l.end(); ++it)
        h << (*it)->peerHostName();
    map_host[login] = h;
}

QString Connect::formatUptime() {
    QString string;
    QDateTime currentDateTime = QDateTime::currentDateTime();
    int days, seconds, hours, minutes;

    seconds = startDateTime.secsTo(currentDateTime);
    days = seconds / (24 * 60 * 60);
    if (days) {
        string = QString::number(days) + " days, ";
        seconds -= days * 24 * 60 * 60;
    }
    hours = seconds / (60 * 60);
    minutes = (seconds - (hours * 60 * 60)) / 60;
    seconds = seconds - (hours * 60 * 60) - (minutes * 60);
    QTime time;
    time.setHMS(hours, minutes, seconds);
    string += time.toString("hh:mm:ss");

    return string;
}

QString Connect::serverDisplayPrefix(const QString &str) {
    return "<Mtp> " + str;
}

