//
//
// C++ Interface: vote
//
// Description:
//
//
// Author: Guillaume Denry <denryg@wanadoo.fr>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef _VOTE_H_
#define _VOTE_H_

#include <qstringlist.h>

#include "plugins.h"

#define POLL_WALL_LINES 20
#define POLL_WALL_MAX_LINES 100

class Poll {
public:
    Poll() {};

    ~Poll() {};

    uint voteCount() const;

    uint id;
    QString author;
    QString question;
    QStringList choices;
    QStringList polledUsers;
    QList<uint> results;
};


class Vote : public Plugin {
    Q_OBJECT
    PLUGIN_OBJECT(Vote)
public:
     Vote(PluginManagerInterface* parent=0);

    ~Vote();

    void addpollCmd(const QString&, const QStringList&);
    void closepollCmd(const QString&, const QStringList&);
    void lspollCmd(const QString&, const QStringList&);
    void voteCmd(const QString&, const QStringList&);
    void pollwallCmd(const QString&, const QStringList&);

    void exportCommands();

    void incomingUser(const QString& login);

    typedef QList<Poll> PollList;

private:
    PollList polls;
    QStringList current_poll_wall;

    uint getMaxPollId();
    bool isUsedId(uint id);
    uint getFreePollId();
    QString getPollDisplay(Poll &poll);
    QString getPollWinners(const Poll &poll);
    void addToPollWall(Poll &poll);
    bool loadPollWall();

    bool loadPolls();
    bool savePolls();
};

#endif
