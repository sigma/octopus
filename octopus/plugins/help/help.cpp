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
#include <qfile.h>
#include <QTextStream>

#include "help.h"

PLUGIN_FACTORY(Help);

Help::Help(PluginManagerInterface* parent) : Plugin(parent) {}

Help::~Help() {}

void Help::helpCmd(const QString& from, const QStringList& list) {
	if (list[0] != "") {
	    QString txt;
	    QStringList l;

	    if (loadHelpFile(list[0], &l)) {
            txt = "Help for \"" + list[0] + "\" :";
            for (QStringList::Iterator it = l.begin(); it != l.end(); ++it)
                txt += "\n" + *it;
            manager()->connectionPlugin()->serverSend(from, txt);
            txt = "End of help";
            manager()->connectionPlugin()->serverSend(from, txt);
	    }
	    else {
            txt = "No help available for topic \"" + list[0] + "\"";
            manager()->connectionPlugin()->serverSend(from, txt);
            return;
	    }
	}
	else {
	    QDir dir(manager()->dataDir() + "/help/");
	    QString txt = "Help for \"help\" :\n" + dir.entryList(QDir::Files).join("\n");
	    manager()->connectionPlugin()->serverSend(from, txt);
	    manager()->connectionPlugin()->serverSend(from, "End of help");
	}
}

void Help::incomingUser(const QString& login) {
    QFile fin(manager()->dataDir() + "/helpmsg");
    if (!fin.open(QIODevice::ReadOnly)) {
        std::cerr << "Could not read helpmsg file" << std::endl;
        return;
    } else {
        QTextStream stream(&fin);
        QString line, txt;
	while ((line = stream.readLine()) != QString::null) {
	    if (txt != "")
		txt += "\n";
	    txt += line;
	}
        manager()->connectionPlugin()->send(login,txt);
    }
    fin.close();
}

void Help::exportCommands() {
    registerCommand("help",&Help::helpCmd,"{<word>}","Display help");
}

bool Help::loadHelpFile(const QString &helpFilename, QStringList *list) {
    QFile fin(manager()->dataDir() + "/help/" + helpFilename);
    if (!fin.open(QIODevice::ReadOnly)) {
	return false;
    } else {
	QRegExp re("^\\\\(.+)$");
	QTextStream stream(&fin);
	QString line;
	while((line = stream.readLine()) != QString::null) {
	    line = line.simplified();
	    // Is a mark line?
	    if (re.exactMatch(line)) {
		(*list).push_back(re.cap(1).toUpper());
	    } else {
		(*list).push_back("       " + line);
	    }
	}
    }
    fin.close();
    return true;
}

