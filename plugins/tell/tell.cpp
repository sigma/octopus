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

#include <qstringlist.h>

#include "tell.h"
#include "debug.h"

PLUGIN_FACTORY(Tell);

void Tell::exportCommands() {
    registerCommand("tell",&Tell::tellCmd,"{<loginlist>} {<sentence>}","Privately talk with some users","Tell");
    registerCommand("senddata",&Tell::senddataCmd,"{<login>} {<sentence>}","Send a user some datas","Tell");
}

void Tell::tellCmd(const QString& from, const QStringList& list) {
    octAssert(list.count() == 2);

    QStringList tmp = list[0].split(",",QString::SkipEmptyParts);
    QStringList dest, ok, ko;
    for(QStringList::Iterator it = tmp.begin(); it != tmp.end(); ++it)
        dest << manager()->databasePlugin()->canonicalName(*it);

    QString cc = dest.join(",") + ",";

    for(QStringList::Iterator it = dest.begin(); it != dest.end(); ++it) {
        QString cc_tmp(cc);
        cc_tmp.replace(*it + ",","");
        cc_tmp = cc_tmp.left(cc_tmp.length() - 1);
        QString txt1(from + " tells you" + (dest.count() > 1 ? ("(CC: " + cc_tmp + ")") : "") + ": " + list[1]);

        if(manager()->connectionPlugin()->serverSend(*it,txt1))
            ok << *it;
        else
            ko << *it;
    }

    if(ok.count()){
        QString txt2("You tell " + (ok.join(",")) + ": " + list[1]);
        manager()->connectionPlugin()->serverSend(from,txt2);
    }
    if(ko.count()) {
        QString txt2((ko.join(",")) + (ko.count() > 1 ? " are" : " is") + " not connected!");
        manager()->connectionPlugin()->serverSend(from,txt2);
    }
}

void Tell::senddataCmd(const QString& from, const QStringList& list) {
    octAssert(list.count() == 2);

    QString to = manager()->databasePlugin()->canonicalName(list[0]);
    QString txt1("|" + from + "| " + list[1]);
    if(!manager()->connectionPlugin()->send(to,txt1)) {
	    QString txt2(to + " is not connected!");
	    manager()->connectionPlugin()->serverSend(from,txt2);
    }
}
