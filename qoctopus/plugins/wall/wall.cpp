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

#include "wall.h"

PLUGIN_FACTORY(Wall);

Wall::Wall(PluginManagerInterface* parent) : Plugin(parent) {
    loadWall();
    manager()->databasePlugin()->registerInternalVariable(this, "wall_display_count", PRIVATE | INTERNAL | RESIDENT | GLOBAL | UNENCRYPTED, INT, 0);
}

Wall::~Wall() {}

bool Wall::loadWall() {
    QFile fin(manager()->dataDir() + "/wall");
    if (!fin.open(IO_ReadOnly)) {
        std::cerr << "Could not read wall file" << std::endl;
        return false;
    }

    QTextStream stream(&fin);
    QString line;
    QStringList list;
    while((line = stream.readLine()) != QString::null)
        list << line;

    if(list.count() <= WALL_MAX_LINES)
        current_wall = list;
    else {
        QStringList::Iterator it = list.end();
        --it;
        for(int i=0; i < WALL_MAX_LINES; i++,--it)
            current_wall.prepend(*it);
    }
    fin.close();
    return true;
}

void Wall::addToWall(const QString& msg) {
    QFile fout(manager()->dataDir() + "/wall");
    if( !fout.open(IO_WriteOnly | IO_Append)) {
        std::cerr << "Could not write the wall file." << std::endl;
        return;
    }

    QTextStream stream(&fout);
    stream << (msg + "\n");
    fout.close();

    current_wall << msg;
    if(current_wall.count() > WALL_MAX_LINES)
        current_wall.erase(current_wall.begin());
}


void Wall::exportCommands() {
    registerCommand("wall",&Wall::wallCmd,"[{<sentence>}]","Display the wall");
    registerCommand("modwall", &Wall::modwallCmd,"{<int>}[ {<sentence>}]","Modify a writing on the wall","System");
}

void Wall::wallCmd(const QString &from, const QStringList& list) {
    if(list[0] != "") {
        QString message(QDate::currentDate().toString("dd/MM/yyyy ") + QTime::currentTime().toString("hh:mm:ss ") + from);
        for(int i=from.length(); i<=manager()->databasePlugin()->validLoginLength(); i++)
            message += " ";
        message += list[0];
        addToWall(message);
        manager()->logPlugin()->write("wall", message);
        QString txt1(from + " writes to the wall: " + list[0]);
        manager()->connectionPlugin()->serverBroadcastOthers(from, txt1);
        QString txt2("You write to the wall: " + list[0]);
        manager()->connectionPlugin()->serverSend(from, txt2);
    } else {
        QString wall_lines = getWall(from);
        QString txt;
        if (wall_lines == "")
            txt = "Wall is empty";
        else
            txt = "Wall :\n" + wall_lines;
        manager()->connectionPlugin()->serverSend(from, txt);
        if (wall_lines != "")
            manager()->connectionPlugin()->serverSend(from, "End of Wall");
    }
}

void Wall::modwallCmd(const QString &from, const QStringList &list) {
    uint num = list[0].toInt();
    QString replacement = QString::null;
    if (list[1] != "")
        replacement = list[1];

    // Read the current wall and store it in a list
    QFile fwall(manager()->dataDir() + "/wall");
    if (!fwall.open(IO_ReadOnly)) {
        std::cerr << "Could not read wall file" << std::endl;
        return;
    }
    QTextStream stream(&fwall);
    QString line;
    QStringList l;
    while ((line = stream.readLine()) != QString::null)
        l << line;
    fwall.close();

    if (!l.count()) {
        manager()->connectionPlugin()->serverSend(from, "Wall is empty");
        return;
    }

    if (!num || (num > l.count())) {
        manager()->connectionPlugin()->serverSend(from, "Invalid wall line number");
        return;
    }

    // Remove the element and save the file
    QStringList::Iterator it = l.end();
    for (;num > 0; --num)
        it--;

    QString message = *it;
    if(replacement.isNull())
        l.remove(it);
    else {
        QRegExp regex("(([\\d/]+) ([\\d:]+) ([\\w ]{8}) )(.*)"); //@attention Wall format dependant
        if(regex.exactMatch(message)) { // should always be the case, we just want to get the datas
            replacement = regex.cap(1) + replacement;
            *it = replacement;
        }
    }
    QFile fout(manager()->dataDir() + "/wall");
    if (!fout.open(IO_WriteOnly)) {
        std::cerr << "Could not write wall file" << std::endl;
        return;
    }

    QTextStream streamOut(&fout);
    for (it = l.begin(); it != l.end(); it++)
        streamOut << *it + "\n";

    fout.close();
    loadWall();

    // Send messages
    manager()->connectionPlugin()->serverSend(from, "You modify the wall ");
    manager()->connectionPlugin()->serverBroadcastOthers(from, from + " modifies the wall ");
    manager()->logPlugin()->write("modwall", from + " modifies the wall :\n  - " + message + "\n  + " + replacement);
}

QString Wall::getWall(const QString& user) {
    uint count = manager()->databasePlugin()->getValue(user, "wall_display_count").toInt();
    if (count == 0)
        count = WALL_LINES;
    if (current_wall.count() > count)
        count = current_wall.count() - count;
    else
        count = 0;
    QString str;

    for (QStringList::Iterator it = current_wall.begin(); it != current_wall.end(); ++it) {
        if (!count) {
            if (str != "")
                str += "\n";
            str += *it;
        } else
            count--;
    }
    return str;
}
