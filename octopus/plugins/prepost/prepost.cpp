//
//
// C++ Implementation: cpp
//
// Description:
//
//
// Author: Yann Hodique <Yann.Hodique@lifl.fr>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include <iostream>

#include <qregexp.h>
#include <qdatetime.h>
#include <QTextStream>

#include "prepost.h"

using namespace std;

PLUGIN_FACTORY(PrePost);

PrePost::PrePost(PluginManagerInterface* parent) : PrePostPlugin(parent) {
    manager()->databasePlugin()->registerInternalVariable(this, "away", PUBLIC | INTERNAL | TRANSIENT | GLOBAL | UNENCRYPTED, BOOL);
}

PrePost::~PrePost() {}

void PrePost::modifiedInternalVariable(const QString& login, const QString& name, const QString& value) {
    if(name == "away") {
        int index = away_cache.indexOf(login);
        QString txt(login);
        if(value == "1") {
            txt += " is away";
	    if(index == -1)
		away_cache << login;
        } else {
            txt += " is back";
	    if(index != -1)
		away_cache.removeAt(index);
        }
        manager()->connectionPlugin()->serverBroadcast(txt);
    }
}

QString PrePost::treatIncomingMessage(const QString& from, const QString& msg) {
#ifdef _REDIRECT_
    QStringList commands = this->splitCommand(msg);
    QString cmd = commands[0];
    commands.pop_front();
    command_queue[from] = commands;
    return this->expandAliases(from,cmd);
#else

    return this->expandAliases(from,msg);
#endif
}

QString PrePost::treatOutgoingMessage(const QString& to, const QString& msg) {
#ifdef _REDIRECT_
    if(!command_queue[to].count())
        return msg;
    else {
        QString cmd = expandAliases(to, command_queue[to][0] + " " + msg);
        command_queue[to].pop_front();
        manager()->dispatch(to,cmd);
        return QString::null;
    }
#else
    if(away_cache.indexOf(to) != -1) {
	QStringList list = msg.split("\n",QString::SkipEmptyParts);
        QString date = QTime::currentTime().toString("hh:mm:ss ");
        return date + list.join("\n" + date);
    } else
        return msg;
#endif
}

void PrePost::loadAlias(const QString& login) {
    QFile fin(manager()->dataDir() + "/alias/" + login);
    if (fin.open(QIODevice::ReadOnly)) {
        QTextStream stream(&fin);
        QString line;
        while((line = stream.readLine()) != QString::null) {
            int pos = line.indexOf(' ');
            alias[login][line.left(pos)] = line.right(line.length() - pos - 1);
        }
    }
    fin.close();
}

void PrePost::storeAlias(const QString& login) {
    QFile fout(manager()->dataDir() + "/alias/" + login);
    if( !fout.open(QIODevice::WriteOnly)) {
        std::cerr << "Could not write alias file." << std::endl;
        return;
    }

    QTextStream stream(&fout);
    for(AliasMap::Iterator it = alias[login].begin(); it != alias[login].end(); ++it) {
        stream << (it.key() + " " + it.value() + "\n");
    }
    fout.close();
}

void PrePost::exportCommands() {
    registerCommand("alias",&PrePost::aliasCmd,"[{<word>} {<sentence>}]","Create an alias");
    registerCommand("unalias",&PrePost::unaliasCmd,"{<word>}","Suppress an alias");
    registerCommand("realias",&PrePost::realiasCmd,"{<word>} {<word>}","Rename an alias");
}

void PrePost::incomingUser(const QString& login) {
    if(manager()->databasePlugin()->isRegisteredUser(login))
        loadAlias(login);
    if(manager()->databasePlugin()->isRegisteredUser(login))
      manager()->databasePlugin()->setValue(login,"away","off",false);
}

void PrePost::outgoingUser(const QString& login) {
    if(manager()->databasePlugin()->isRegisteredUser(login)) {
        storeAlias(login);
    }
    alias.remove(login);
    away_cache.removeAll(login);
}

void PrePost::killedUser(const QString& login) {
    QFile f(manager()->dataDir() + "/alias/" + login);
    f.remove();
    alias.remove(login);
}

void PrePost::renamedUser(const QString& old_login, const QString& new_login) {
    QFile fin(manager()->dataDir() + "/alias/" + old_login);
    QFile fout(manager()->dataDir() + "/alias/" + new_login);
    if(fin.open(QIODevice::ReadOnly)) {
        if(fout.open(QIODevice::WriteOnly)) {
            fout.write(fin.readAll());
            fout.close();
        }
        fin.close();
    }
    fin.remove();
    AliasMap aliases = alias[old_login];
    alias.remove(old_login);
    alias[new_login] = aliases;
}

void PrePost::aliasCmd(const QString& from, const QStringList& list) {
    if(list[0] != "") {
        addAlias(from,list[0],list[1]);
        QString txt("You alias \"" + list[0] + "\" to \"" + list[1] + "\"");
        manager()->connectionPlugin()->serverSend(from,txt);
    } else {
        QString txt;
        if (alias[from].count()) {
            manager()->connectionPlugin()->serverSend(from, "User Aliases :");
            for(AliasMap::Iterator it = alias[from].begin(); it != alias[from].end(); ++it) {
                if (txt != "")
                    txt += "\n";
                txt += it.key() + " -> " + it.value();
            }
            manager()->connectionPlugin()->send(from, txt);
            manager()->connectionPlugin()->serverSend(from, "End of Aliases");
        }
        else {
            manager()->connectionPlugin()->serverSend(from, "There are currently no aliases");
        }
    }
}

void PrePost::unaliasCmd(const QString& from, const QStringList& list) {
    delAlias(from,list[0]);
    QString txt("You unalias \"" + list[0] + "\"");
    manager()->connectionPlugin()->serverSend(from,txt);
}

void PrePost::realiasCmd(const QString& from, const QStringList& list) {
    AliasMap m = alias[from];
    QString text;
    if(m.find(list[0]) == m.end()) {
        QString error("Undefined alias " + list[0]);
        manager()->connectionPlugin()->serverSend(from,error);
        return;
    } else {
        text = m[list[0]];
    }
    delAlias(from,list[0]);
    addAlias(from,list[1],text);
    QString txt("You realias \"" + list[0] + "\" to \"" + list[1] + "\"");
    manager()->connectionPlugin()->serverSend(from,txt);
}

void PrePost::addAlias(const QString& login, const QString& com, const QString& al) {
    alias[login][com] = al;
}

void PrePost::delAlias(const QString& login, const QString& com) {
    alias[login].remove(com);
}

QString PrePost::expandAliases(const QString& from, const QString& msg) {
    int pos = msg.indexOf(' ');
    QString word = (pos != -1)?msg.left(pos):msg;
    QStringList args;
    if(pos != -1)
        args = msg.right(msg.length() - pos - 1).split(' ',QString::SkipEmptyParts);
    QStringList largs;
    for(AliasMap::Iterator it = alias[from].begin(); it != alias[from].end(); ++it) {
        if(it.key() == word) {
            QString tmp = *it;
            QString res;
            int i = 0;
            while((i = tmp.indexOf('$')) != -1) {
                res += tmp.left(i);
                tmp.remove(0,i);
                if(tmp.startsWith("$$")) {
                    res += "$";
                    tmp.remove(0,2);
                } else if (tmp.startsWith("$*")) {
                    res += args.join(" ");
                    tmp.remove(0,2);
                } else if (tmp.startsWith("$n")) {
                    res += "\n";
                    tmp.remove(0,2);
                } else if (tmp.startsWith("$+")) {
                    if(args.begin() != args.end()) {
                        largs << *(args.begin());
                        args.erase(args.begin());
                    }
                    tmp.remove(0,2);
                } else if (tmp.startsWith("$-")) {
                    if(largs.begin() != largs.end()) {
                        QStringList::Iterator p = --largs.end();
                        args.prepend(*p);
                        largs.erase(p);
                    }
                    tmp.remove(0,2);
                } else {
                    QRegExp re("^\\$(\\d).*");
                    if(re.exactMatch(tmp)) {
                        int number = re.cap(1).toInt();
                        res += args[number-1];
                        tmp.remove(0,2);
                    } else {
                        QString error("Wrong argument : \"" + tmp.left(2) + "\"");
                        manager()->connectionPlugin()->serverSend(from,error);
                        return "";
                    }
                }
            }
            res += tmp;
            //	    cerr << "Alias expanded to " << res << endl;
            return res;
        }
    }

    return msg;
}

#ifdef _REDIRECT_
QStringList PrePost::splitCommand(const QString& msg) {
    return QStringList::split("|",msg);
}
#endif
