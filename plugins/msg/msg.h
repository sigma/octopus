//
//
// C++ Interface: h
//
// Description:
//
//
// Author: Yann Hodique <Yann.Hodique@lifl.fr>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef _MSG_H_
#define _MSG_H_

#include <qstringlist.h>

#include "plugins.h"

class Msg : public Plugin {
    Q_OBJECT
    PLUGIN_OBJECT(Msg)
public:
    Msg(PluginManagerInterface* parent=0);

    ~Msg();

    void showmsgCmd(const QString&, const QStringList&);
    void sendmsgCmd(const QString&, const QStringList&);
    void clearmsgCmd(const QString&, const QStringList&);
    void subscribeCmd(const QString&, const QStringList&);
    void unsubscribeCmd(const QString&, const QStringList&);

    void exportCommands();
    void incomingUser(const QString&);
    void outgoingUser(const QString&);
    void killedUser(const QString&);
    void renamedUser(const QString&, const QString&);

private:
    bool writeMsg(const QString&, const QString&);
    void sendmsgUser(const QString& from, const QStringList& list, bool quiet = false);
    QStringList getUsers(const QString& listname);
};

#endif
