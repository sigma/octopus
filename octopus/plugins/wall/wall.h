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

#ifndef _WALL_H_
#define _WALL_H_

#include <qstringlist.h>

#include "plugins.h"

#define WALL_LINES 20
#define WALL_MAX_LINES 100

class Wall : public Plugin {
    Q_OBJECT
    PLUGIN_OBJECT(Wall)
public:
    Wall(PluginManagerInterface* parent=0);

    ~Wall();

    bool loadWall();
    void addToWall(const QString&);

    void wallCmd(const QString &from, const QStringList&);
    void modwallCmd(const QString &from, const QStringList&);

    void exportCommands();

private:
    QStringList current_wall;

    QString getWall(const QString& user);
};

#endif
