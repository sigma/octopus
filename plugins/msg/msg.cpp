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

#include "msg.h"
#include <QTextStream>
#include <QDateTime>

PLUGIN_FACTORY(Msg);

Msg::Msg(PluginManagerInterface* parent) : Plugin(parent) {
    manager()->databasePlugin()->registerInternalVariable(this, "msg_display_count", PRIVATE | INTERNAL | RESIDENT | GLOBAL | UNENCRYPTED, INT, "8");
}

Msg::~Msg() {}

void Msg::showmsgCmd(const QString& from, const QStringList& list) {
    QFile fin(manager()->dataDir() + "/msg/" + from);
    if (!fin.open(QIODevice::ReadOnly)) {
        manager()->connectionPlugin()->serverSend(from, "You have no message");
        return;
    } else {
        int count = 0;
	    int msg_display_count = manager()->databasePlugin()->getValue(from, "msg_display_count").toInt();
	    manager()->connectionPlugin()->serverSend(from,"Your message(s) :");

	    if (list[0] == " all")
            msg_display_count = 0;
        QTextStream stream(&fin);
	    QStringList list;
        QString line;
	    while ((line = stream.readLine()) != QString::null)
            list << QString::number(++count) + " " + line;

	    if (!msg_display_count || (list.count() < msg_display_count))
            count = 0;
	    else
	        count = list.count() - msg_display_count;

	    QString txt("");
	    for (;count < list.count();count++) {
            txt += list[count];
            if (count < list.count() - 1)
                txt += "\n";
	    }

	    manager()->connectionPlugin()->send(from, txt);

        txt = ("You have " + QString::number(count) + " message" + ((count>1)?"s":""));
        manager()->connectionPlugin()->serverSend(from, txt);
    }
    fin.close();
}

void Msg::sendmsgUser(const QString& from, const QStringList& list, bool quiet) {
    QString txt1(from + " sends a message to you: " + list[1]);
    QString message(QDate::currentDate().toString("dd/MM/yyyy ") + QTime::currentTime().toString("hh:mm:ss ") + from + " : " + list[1]);
    QString to(manager()->databasePlugin()->canonicalName(list[0]));
    if((manager()->connectionPlugin()->serverSend(to,txt1) || manager()->databasePlugin()->isRegisteredUser(to))
       && writeMsg(to,message) && !quiet) {
        QString txt2("You send a message to " + to + ": " + list[1]);
        manager()->connectionPlugin()->serverSend(from,txt2);
    } else if(!quiet) {
		QString error("Invalid user");
		manager()->connectionPlugin()->serverSend(from,error);
    }
}

void Msg::sendmsgCmd(const QString& from, const QStringList& list) {
    if(list[0].startsWith("@")) {
        QStringList users = getUsers(list[0]);
        QStringList newlist(list);
        newlist[1] = "[" + list[0] + "] " + list[1];
        for(QStringList::ConstIterator it = users.begin(); it != users.end(); ++it) {
            newlist[0] = *it;
            sendmsgUser(from,newlist,true);
        }
        QString txt("You send a message to " + list[0] + ": " + list[1]);
        manager()->connectionPlugin()->serverSend(from,txt);
    } else
        sendmsgUser(from,list);
}

QStringList Msg::getUsers(const QString& listname) {
    QStringList l;

    QFile f(manager()->dataDir() + "/msg/" + listname);
    if (!f.open(QIODevice::ReadOnly)) {
        octInfo("Could not read message file\n");
        return l;
    }

    QTextStream stream(&f);
    QString line;
    while((line = stream.readLine()) != QString::null) {
        l << line;
    }
    f.close();

    return l;
}

void Msg::subscribeCmd(const QString& from, const QStringList& list) {
    writeMsg(list[0],from);
    QString txt("You subscribed to list " + list[0]);
    manager()->connectionPlugin()->serverSend(from,txt);
}

void Msg::unsubscribeCmd(const QString& from, const QStringList& list) {
    QString listname = list[0];

    QStringList l = getUsers(listname);
    l.removeAll(from);

    QFile f(manager()->dataDir() + "/msg/" + listname);
    if (l.count()) {
        if (!f.open(QIODevice::WriteOnly)) {
            octInfo("Could not write message file.\n");
            return;
        }

        QTextStream stream2(&f);
        for(QStringList::Iterator it = l.begin(); it != l.end(); ++it)
            stream2 << (*it + "\n");
        f.close();
    } else
        f.remove();

    QString txt("You unsubscribed from list " + listname);
    manager()->connectionPlugin()->serverSend(from,txt);
}

void Msg::clearmsgCmd(const QString& from, const QStringList& list) {
	if(list[0] == "") { // no arg
	    QFile f(manager()->dataDir() + "/msg/" + from);
	    f.remove();
	    QString txt("You clear your messages");
        manager()->connectionPlugin()->serverSend(from,txt);
	}
	else {
	    if (list[0].toInt() < 1) {
            manager()->connectionPlugin()->serverSend(from, "Parameter(s) must be greater than 0!");
            return;
	    }

	    int start = list[0].toInt() - 1;
	    int stop = (list[1] == "")?(start+1):(list[1].toInt());

	    QFile f(manager()->dataDir() + "/msg/" + from);
	    if (!f.open(QIODevice::ReadOnly)) {
            octInfo("Could not read message file\n");
            return;
	    }

	    QTextStream stream(&f);
	    QString line;
	    QStringList l;
	    while((line = stream.readLine()) != QString::null) l << line;
	    f.close();

	    if (start >= l.count()) {
            manager()->connectionPlugin()->serverSend(from, "Too large(s) parameter(s)!");
            return;
	    }
	    if (stop > l.count())
            stop = l.count();

	    l.erase(l.begin() + start, l.begin() + stop);

	    if (l.count()) {
            if (!f.open(QIODevice::WriteOnly)) {
                octInfo("Could not write message file.\n");
                return;
            }

            QTextStream stream2(&f);
            for(QStringList::Iterator it = l.begin(); it != l.end(); ++it)
                stream2 << (*it + "\n");
            f.close();
	    } else
            f.remove();

	    QString txt("You clear message");

	    if (start + 1  == stop)
            txt += " " + QString::number(stop);
	    else
            txt += "s " + QString::number(start+1) + " to " + QString::number(stop);

        manager()->connectionPlugin()->serverSend(from,txt);
	}
}

void Msg::exportCommands() {
    registerCommand("showmsg",&Msg::showmsgCmd,"[{all}]","Show current messages");
    registerCommand("sendmsg",&Msg::sendmsgCmd,"{@?<login>} {<sentence>}","Send a message to a user or list","Msg");
    registerCommand("clearmsg",&Msg::clearmsgCmd,"[{<int>}[ {<int>}]]","Clear some messages");
    registerCommand("subscribe",&Msg::subscribeCmd,"{@<login>}","Subscribe to a list");
    registerCommand("unsubscribe",&Msg::unsubscribeCmd,"{@<login>}","Unsubscribe to a list");
}

void Msg::incomingUser(const QString& login) {
    if(manager()->databasePlugin()->isRegisteredUser(login))
        showmsgCmd(login,QStringList("showmsg"));
}

void Msg::outgoingUser(const QString& login) {
    if(!manager()->databasePlugin()->isRegisteredUser(login)) {
        QFile f(manager()->dataDir() + "/msg/" + login);
        f.remove();
    }
}

void Msg::killedUser(const QString& login) {
    QFile f(manager()->dataDir() + "/msg/" + login);
    f.remove();
}

void Msg::renamedUser(const QString& old_login, const QString& new_login) {
    QFile fin(manager()->dataDir() + "/msg/" + old_login);
    QFile fout(manager()->dataDir() + "/msg/" + new_login);
    if(fin.open(QIODevice::ReadOnly)) {
        if(fout.open(QIODevice::WriteOnly)) {
            fout.write(fin.readAll());
            fout.close();
        }
        fin.close();
    }
    fin.remove();
}

bool Msg::writeMsg(const QString& login, const QString& msg) {
    QFile fout(manager()->dataDir() + "/msg/" + login);
    if( !fout.open(QIODevice::WriteOnly | QIODevice::Append)) {
        octInfo("Could not write message file.\n");
        return false;
    }

    QTextStream stream(&fout);
    stream << (msg + "\n");
    fout.close();
    return true;
}
