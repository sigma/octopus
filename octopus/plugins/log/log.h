//
//
// C++ Interface: h
//
// Description: 
//
//
// Author: Guillaume Denry <denryg@wanadoo.fr>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef _LOG_H_
#define _LOG_H_

#include <qstringlist.h>

#include "plugins.h"

#define WALL_LINES 20
#define WALL_MAX_LINES 100

class Log : public LogPlugin {
    Q_OBJECT
    PLUGIN_OBJECT(Log)
public:
    Log(PluginManagerInterface* parent=0);
    
    ~Log();
    
    void exportCommands();
    
    bool write(const QString & section, const QString & msg);    
private:
    QString logDir();
};

#endif
