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

/*
 * Manage all plugins
 */

#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H

#include <list>
#include <string>
#include <map>
#include "Plugin.h"
#include "ConnectionPlugin.h"
#include "PluginManagerInterface.h"

using std::list;
using std::string;

class PluginManager : public PluginManagerInterface {

    typedef struct {
        Plugin* plugin;
        void* handler;
    } Data;
    
    typedef struct {
	Plugin* plugin;
	int id;
    } ComRef;
    
    typedef std::map<const char*,ComRef> CommandMap;
    
public:
    PluginManager();
    ~PluginManager();
    
    void loadPluginsDirectory(const char* dir_name);
    Plugin* loadPluginFile(const char* file_name);
    void loadConnectionPluginFile(const char* file_name);

    void receiveMsg(const string&);
    void emitMsg(const string&);
    
    // access to required plugins
    ConnectionPlugin* connectionPlugin();
    Plugin* databasePlugin();
    Plugin* defaultPlugin();
    void registerCommand(Plugin* plugin, const char* prefix, int id);

private:
    list<Data> active;
    ConnectionPlugin *connect;
    CommandMap available_commands;
};

#endif // PLUGINMANAGER_H
