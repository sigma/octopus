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
 * Define a plugin
 */

#ifndef PLUGIN_H
#define PLUGIN_H

#define PLUGIN_FACTORY(a) \
extern "C" Plugin* create() {\
    return new a;\
}\
\
extern "C" void destroy(Plugin* p) {\
    delete p;\
}

#include <string>

#include "PluginManagerInterface.h"

using std::string;

class Plugin {

public:

    Plugin() {}
    virtual ~Plugin() {}
    void setPluginManager(PluginManagerInterface* manager) {
	this->mng = manager;
	this->exportCommands();
    }
    virtual void callCommand(int id, string& msg) = 0;
    
protected:
    void registerCommand(const char* prefix, int id) {
	mng->registerCommand(this, prefix, id);
    }
    virtual void exportCommands() = 0;
    
private:
    PluginManagerInterface *mng;
};

typedef Plugin* create_t();
typedef void destroy_t(Plugin*);


#endif // PLUGIN_H

