//
//
// C++ Implementation: vote
//
// Description:
//
//
// Author: Guillaume Denry <denryg@wanadoo.fr>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include <iostream>
#include <qregexp.h>
#include <qstring.h>
#include <QTextStream>
#include <QDateTime>

#include "vote.h"

PLUGIN_FACTORY(Vote);

Vote::Vote(PluginManagerInterface* parent) : Plugin(parent) {
    loadPolls();
    loadPollWall();
    manager()->databasePlugin()->registerInternalVariable(this, "pollwall_display_count", PRIVATE | INTERNAL | RESIDENT | GLOBAL | UNENCRYPTED, INT, 0);
}

Vote::~Vote() {}

bool Vote::loadPolls() {
    QFile fin(manager()->dataDir() + "/polls");
    if (!fin.open(QIODevice::ReadOnly)) {
        octInfo("Could not read polls file\n");
        return false;
    }

    polls.clear();
    QTextStream stream(&fin);
    QString line;
    QRegExp re("\\\\id\\:([0-9]+)\\\\message\\:([^\\\\]+)\\\\author\\:([^\\\\]+)((\\\\choice\\:[^\\\\]+\\:[0-9]+)+)((\\\\user\\:.+)*)");
    while ((line = stream.readLine()) != QString::null) {
	if (re.exactMatch(line)) {
	    Poll poll;
	    // Make the poll
	    poll.id = re.cap(1).toUInt();
	    poll.question = re.cap(2);
	    poll.author = re.cap(3);

	    // Choices
	    QStringList choices = re.cap(4).split('\\',QString::SkipEmptyParts);
	    QRegExp choiceRe("choice\\:(.+)\\:([0-9]+)");
	    for (int i = 0; i < choices.count(); ++i) {
		choiceRe.exactMatch(choices[i]);
		poll.choices << choiceRe.cap(1).simplified();
		poll.results.push_back(choiceRe.cap(2).toUInt());
	    }
	    // Already polled users
	    QStringList users = re.cap(6).split('\\',QString::SkipEmptyParts);
	    QRegExp userRe("user\\:(.+)");
	    for (int i = 0; i < users.count(); ++i) {
		userRe.exactMatch(users[i]);
		poll.polledUsers << userRe.cap(1).simplified();
	    }
	    // Store the poll
	    polls.push_back(poll);
	}
    }

    fin.close();
    return true;
}

bool Vote::savePolls() {
    QFile fout(manager()->dataDir() + "/polls");
    if (!fout.open(QIODevice::WriteOnly)) {
        octInfo("Could not write polls file\n");
        return false;
    }

    QTextStream stream(&fout);
    QString line;
    for (PollList::Iterator it = polls.begin(); it != polls.end(); ++it) {
	// make the line
	line = "\\id:" + QString::number((*it).id);
	line += "\\message:" + (*it).question;
	line += "\\author:" + (*it).author;

	// choices
	for (int i = 0; i != (*it).choices.count(); ++i)
	    line += "\\choice:" + (*it).choices[i] + ":" + QString::number((*it).results[i]);

	// polled users
	for (QStringList::Iterator userIt = (*it).polledUsers.begin(); userIt != (*it).polledUsers.end(); ++userIt)
	    line += "\\user:" + *userIt;
	stream << line + "\n";
    }

    fout.close();

    return true;
}

void Vote::exportCommands() {
    registerCommand("addpoll", &Vote::addpollCmd,"{<wordlist>} {<sentence>}", "Add a poll");
    registerCommand("closepoll", &Vote::closepollCmd,"{<int>}","Close a poll");
    registerCommand("lspoll", &Vote::lspollCmd,QString::null,"List all polls");
    registerCommand("vote", &Vote::voteCmd,"{<int>} {<word>}","Vote");
    registerCommand("pollwall", &Vote::pollwallCmd,QString::null,"display the closed polls");
}

void Vote::addpollCmd(const QString &from, const QStringList& list) {
    octAssert(list.size() == 2);

	QStringList choiceList = list[0].split(",",QString::SkipEmptyParts);

	Poll poll;
	uint i = 0;
	poll.author = from;
	poll.question = list[1].simplified();
	for (QStringList::Iterator it = choiceList.begin(); it != choiceList.end(); ++it, ++i) {
	    poll.choices << *it;
	    poll.results.push_back(0);
	}
	poll.id = getFreePollId();
	polls.push_back(poll);
	savePolls();
	manager()->connectionPlugin()->serverSend(from, "You propose a poll (" + QString::number(poll.id) + ") : " + poll.question + " [" + poll.choices.join("/") + "]");
	manager()->connectionPlugin()->serverBroadcastOthers(from, from + " proposes a poll (" + QString::number(poll.id) + ") : " + poll.question + " [" + poll.choices.join("/") + "]");
}

void Vote::closepollCmd(const QString &from, const QStringList &list) {
    octAssert(list.size() == 1);

    uint id = list[0].toUInt();

    PollList::Iterator p;
    bool found(false);
    for(PollList::Iterator it = polls.begin(); it != polls.end(); ++it)
        if((*it).id == id) {
            p = it;
            found = true;
            break;
        }

    if(!found) {
        manager()->connectionPlugin()->serverSend(from, "No such id : " + QString::number(id));
        return;
    } else {
        if((from == (*p).author) || from == "Root") {
            QString results("Poll " + QString::number(id) + " (" + (*p).question + ")" + " is now closed!\nResults :");
            int i = 0;
            for (QList<uint>::Iterator it = (*p).results.begin(); it != (*p).results.end(); ++it, ++i)
                results += "\n" + (*p).choices[i] + " : " + QString::number(*it);

            manager()->connectionPlugin()->serverBroadcast(results);

            // Add the poll in the pollwall
            addToPollWall(*p);
            // Remove the poll and save all
            polls.erase(p);
            savePolls();
        } else {
            QString error("closepoll: permission denied");
            manager()->connectionPlugin()->serverSend(from, error);
        }
    }
}

void Vote::lspollCmd(const QString &from, const QStringList& list) {
    octAssert(list.size() == 0);

	QString txt;
	QMap<int,QString> poll_map;

	for (PollList::Iterator it = polls.begin(); it != polls.end(); ++it)
	    poll_map.insert((*it).id, getPollDisplay(*it));

	if (poll_map.count()) {
	    txt = "Polls :";
	    for (QMap<int,QString>::Iterator it = poll_map.begin(); it != poll_map.end(); ++it)
            txt += "\n" + it.value();
	    manager()->connectionPlugin()->serverSend(from, txt);
	    manager()->connectionPlugin()->serverSend(from, "End of polls");
	}
	else {
	    txt = "There are currently no polls";
	    manager()->connectionPlugin()->serverSend(from, txt);
	}
}

void Vote::voteCmd(const QString &from, const QStringList& list) {
    octAssert(list.size() == 2);

	uint id = list[0].toUInt();
	QString choice = list[1].simplified();

	PollList::Iterator p;
	// Search for the poll ID
	bool found(false);
	for (PollList::Iterator it = polls.begin(); it != polls.end(); ++it)
	  if((*it).id == id) {
	    p = it;
	    found = true;
	    break;
	  }

	if (!found) {
	  manager()->connectionPlugin()->serverSend(from, "No such poll id : " + QString::number(id));
	  return;
	}

	// Has user already voted?
	if ((*p).polledUsers.indexOf(from) == -1) {
	    // Search for the vote choice
	    int choice_number;
	    if ((*p).choices.indexOf(choice) == -1) {
		manager()->connectionPlugin()->serverSend(from, "No such choice : " + choice);
		return;
	    } else
		choice_number = (*p).choices.indexOf(choice);

	    // Add the user
	    (*p).polledUsers.push_back(from);

	    // Increasing the choice count
	    (*p).results[choice_number]++;

	    savePolls();

	    // And send the message
	    manager()->connectionPlugin()->serverSend(from, "For the poll " + QString::number(id) + ", your choice is " + choice);
	} else
	    manager()->connectionPlugin()->serverSend(from, "You cannot vote twice!");
}

void Vote::pollwallCmd(const QString &from, const QStringList& list) {
    octAssert(list.size() == 0);

	int count = manager()->databasePlugin()->getValue(from, "pollwall_display_count").toInt();
	if (!count)
	    count = POLL_WALL_LINES;
	if (current_poll_wall.count() > count)
	    count = current_poll_wall.count() - count;
	else
	    count = 0;
	QString str;
	if (current_poll_wall.count()) {
	    str = "Poll wall :";
	    for (QStringList::Iterator it = current_poll_wall.begin(); it != current_poll_wall.end(); ++it) {
            if (!count) {
                str += "\n" + *it;
            } else
                count--;
	    }
	} else
	    str = "Poll wall is empty";
	manager()->connectionPlugin()->serverSend(from, str);
	if (current_poll_wall.count())
	    manager()->connectionPlugin()->serverSend(from, "End of poll wall");
}

uint Vote::getMaxPollId() {
    uint max = 0;
    for (PollList::Iterator it = polls.begin(); it != polls.end(); ++it)
	if ((*it).id > max)
	    max = (*it).id;
    return max;
}

bool Vote::isUsedId(uint id) {
    for (PollList::Iterator it = polls.begin(); it != polls.end(); ++it)
	if ((*it).id == id)
	    return true;
    return false;
}

uint Vote::getFreePollId() {
    uint max = getMaxPollId();
    for (uint i = 1; i <= max + 1; ++i)
	if (!isUsedId(i))
	    return i;
    return max;
}

void Vote::incomingUser(const QString& login) {
  QStringList missing_votes;
  for (PollList::Iterator it = polls.begin(); it != polls.end(); ++it) {
      if ((*it).polledUsers.indexOf(login) == -1)
	  missing_votes << getPollDisplay(*it);
  }
  if (missing_votes.count())
    manager()->connectionPlugin()->serverSend(login,"You did not participate to the following polls :\n" + missing_votes.join("\n"));
}

QString Vote::getPollDisplay(Poll &poll) {
    QString vote = QString::number(poll.id) + " : " + poll.question;
    if (vote[vote.length()-1] != '?')
	vote += " ?";
    vote += " [" + poll.choices.join("/") + "] (";
    vote += QString::number(poll.voteCount()) + " vote";
    if (poll.voteCount() != 1)
	vote += "s";
    vote += ")";
    return vote;
}

QString Vote::getPollWinners(const Poll &poll) {
    uint max = 0;
    QString maxChoice;

    // Get the max result
    for (int i = 0; i < poll.choices.count(); ++i) {
	if (poll.results[i] > max)
	    max = poll.results[i];
    }

    // Concat winners
    if (max) {
	for (int i = 0; i < poll.choices.count(); ++i) {
	    if (poll.results[i] == max) {
		if (maxChoice != "")
		    maxChoice += " | ";
		maxChoice += poll.choices[i] + " (" + QString::number(max) + "/" + QString::number(poll.voteCount()) + ")";
	    }
	}
    } else
	maxChoice = "<no winner>";

    return maxChoice;
}

void Vote::addToPollWall(Poll &poll) {
    QFile fout(manager()->dataDir() + "/pollwall");
    QString msg;
    if( !fout.open(QIODevice::WriteOnly | QIODevice::Append)) {
        octInfo("Could not write the pollwall file.\n");
        return;
    }

    QTextStream stream(&fout);
    msg = QDate::currentDate().toString("dd/MM/yyyy ") + QTime::currentTime().toString("hh:mm:ss ");
    msg += poll.author + ": " + poll.question + " [";
    QString choices;
    for (QStringList::Iterator itChoice = poll.choices.begin(); itChoice != poll.choices.end(); ++itChoice) {
	if (choices != "")
	    choices += "/";
	choices += *itChoice;
    }
    msg += choices + "] : " + getPollWinners(poll);

    stream << msg + "\n";
    fout.close();

    current_poll_wall << msg;
    if (current_poll_wall.count() > POLL_WALL_MAX_LINES)
        current_poll_wall.erase(current_poll_wall.begin());
}

bool Vote::loadPollWall() {
    QFile fin(manager()->dataDir() + "/pollwall");
    if (!fin.open(QIODevice::ReadOnly)) {
        octInfo("Could not read pollwall file\n");
        return false;
    }

    QTextStream stream(&fin);
    QString line;
    QStringList list;
    while((line = stream.readLine()) != QString::null)
        list << line;

    if(list.count() <= POLL_WALL_MAX_LINES)
        current_poll_wall = list;
    else {
        QStringList::Iterator it = list.end();
        --it;
        for(int i=0; i < POLL_WALL_MAX_LINES; i++,--it)
            current_poll_wall.prepend(*it);
    }
    fin.close();
    return true;
}

uint Poll::voteCount() const {
    uint result = 0;

    for (QList<uint>::ConstIterator it = results.begin(); it != results.end(); ++it)
	result += (*it);

    return result;
}

