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
#include <dlfcn.h>

#include "PluginManager.h"
#include "DebugUtils.h"

#define __FILENAME__ "PluginManager.cpp"

using std::cout;
using std::cerr;
using std::endl;

PluginManager::PluginManager() {
    connect = 0;
}

PluginManager::~PluginManager() {
  for(list<Data>::iterator it = active.begin(); it != active.end(); ++it) {
    destroy_t* destroy_plugin = (destroy_t*) dlsym((*it).handler, "destroy");
    destroy_plugin((*it).plugin);
    dlclose((*it).handler);
  }
}

void PluginManager::loadPluginsDirectory(const char* dir_name) {}

Plugin* PluginManager::loadPluginFile(const char* file_name) {

    Data d;
    
    void* plug = dlopen(file_name, RTLD_LAZY);
    if (!plug) {
        cerr << "Cannot load library: " << dlerror() << '\n';
        return 0;
    }
    d.handler = plug;

    create_t* create_plugin = (create_t*) dlsym(plug, "create");
    destroy_t* destroy_plugin = (destroy_t*) dlsym(plug, "destroy");
    if (!create_plugin || !destroy_plugin) {
        cerr << "Cannot load symbols: " << dlerror() << '\n';
        return 0;
    }

    Plugin* p = create_plugin();
    d.plugin = p;
    
    active.push_back(d);
    
    p->setPluginManager(this);
    
    return p;
}

void PluginManager::loadConnectionPluginFile(const char* file_name) {
    connect = (ConnectionPlugin*)loadPluginFile(file_name);
    if(!connect) exit(1);
}

ConnectionPlugin* PluginManager::connectionPlugin() {
    return connect;
}

Plugin* PluginManager::databasePlugin() {
    return 0;
}

Plugin* PluginManager::defaultPlugin() {
    return 0;
}

void PluginManager::registerCommand(Plugin* plugin, const char* prefix, int id){
    ComRef cr;
    cr.plugin = plugin;
    cr.id = id;
    available_commands.insert(CommandMap::value_type(prefix,cr));
    
    cerr << "Registered command \"" << prefix << "\"" << endl;
}

void PluginManager::receiveMsg(const string&) {}

void PluginManager::emitMsg(const string&) {}
