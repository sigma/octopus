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

#ifndef _HELP_H_
#define _HELP_H_

#include "plugins.h"

class Help : public Plugin {
    Q_OBJECT
    PLUGIN_OBJECT(Help)
public:
    Help(PluginManagerInterface* parent=0);

    ~Help();

    void helpCmd(const QString&, const QStringList&);
    void incomingUser(const QString&);
    void exportCommands();
private:
    bool loadHelpFile(const QString &filename, QStringList *list);
};

#endif
