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
/**
 * @file   Plugin.h
 * @author Yann Hodique
 * @date   Sat Mar  1 16:05:58 2003
 *
 * @brief  Interface for a plugin.
 *
 *
 */

#ifndef PLUGIN_H
#define PLUGIN_H

#define PLUGIN_FACTORY(a)                                       \
    extern "C" Plugin* create(PluginManagerInterface* parent) { \
        return new a(parent);                                   \
    }                                                           \
                                                                \
    extern "C" void destroy(Plugin* p) {                        \
        delete p;                                               \
    }                                                           \
    extern "C" QString pluginName() {                           \
        return #a;                                              \
    }

#define PLUGIN_OBJECT(cl)                                               \
    private:                                                            \
    typedef void(cl::*cl_com)(const QString&,const QStringList&);       \
    int cl_index;                                                       \
    QMap<int,cl_com> cl_map;                                            \
    void registerCommand(const QString& prefix, cl_com c,               \
                         const QString& pattern,                        \
                         const QString& description = QString::null,    \
                         const QString& group = QString::null) {        \
        if(manager()) {                                                 \
            manager()->registerCommand(this, prefix,                    \
                                       cl_index, pattern,               \
                                       description, group);             \
            desc_map.insert(prefix, description);                       \
            cl_map.insert(cl_index++,c);                                \
        }                                                               \
    }                                                                   \
    public:                                                             \
    void callCommand(int id,const QString& from,                        \
                     const QStringList& args) {                         \
        QMap<int,cl_com>::Iterator it;                                  \
        if((it = cl_map.find(id)) != cl_map.end())                      \
            (this->*(*it))(from,args);                                  \
        else  std::cerr << "Unknown command" << std::endl;              \
    }                                                                   \
    const QString pluginName() const {                                  \
        return #cl;                                                     \
    }                                                                   \
    private:

#include <iostream>
#include <qmap.h>
#include <qstring.h>
#include <qobject.h>
#include <qdir.h>

class PluginManagerInterface;

class Plugin : public QObject {

    Q_OBJECT

public:
    /**
     * Constructor.
     *
     * @param parent corresponding plugin manager.
     *
     */
    Plugin(PluginManagerInterface* parent=0) : QObject((QObject*)parent) {
        mng = parent;
    }

    /**
     * Destructor.
     *
     *
     */
    virtual ~Plugin() {}

    /**
     * Associated plugin manager.
     *
     *
     * @return pointer to the plugin manager.
     */
    PluginManagerInterface* manager() {return mng;}

    /**
     * Virtually calls a method corresponding to a command. Automatically redefined using PLUGIN_OBJECT macro.
     *
     * @param id handler of the method.
     * @param from first argument of the method : the user who emitted the request.
     * @param args second argument of the method : the arguments.
     */
    virtual void callCommand(int id, const QString& from, const QStringList& args) = 0;

    /**
     * Plugin identifier.
     *
     * @return Name of the plugin.
     */
    virtual const QString pluginName() const = 0;

    /**
     * Exports the defined commands, by calls at PluginManagerInterface::registerCommand
     *
     */
    virtual void exportCommands() = 0;

    /**
     * Actions when a user comes in.
     *
     * @param login login of the user.
     */
    virtual void incomingUser(const QString& /*login*/) {}

    /**
     * Actions when a user leaves.
     *
     * @param login login of the user.
     */
    virtual void outgoingUser(const QString& /*login*/) {}

    /**
     * Actions when a user is killed.
     *
     * @param login login of the user.
     */
    virtual void killedUser(const QString& /*login*/) {}

    /**
     * Actions when a user is renamed.
     *
     * @param login login of the user.
     */
    virtual void renamedUser(const QString& /*old_login*/, const QString& /*new_login*/) {}

    /**
     * Actions when server is shut down.
     */
    virtual void shutdown() {}

    virtual void modifiedInternalVariable(const QString& /*login*/, const QString& /*name*/, const QString& /*value*/) {}

    QMap<QString,QString> desc_map;

protected:
    PluginManagerInterface *mng;
};

typedef Plugin* create_t(PluginManagerInterface*);
typedef void destroy_t(Plugin*);
typedef QString string_func_t();


#endif // PLUGIN_H

