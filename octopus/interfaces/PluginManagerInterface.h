/*********************************************************************************
 *   Copyright (C) 2003 by Octopus Authors                                       *
 *   octopus-devel@nongnu.org                                                    *
 *                                                                               *
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
 * @file   PluginManagerInterface.h
 * @author Yann Hodique
 * @date   Sat Mar  1 15:26:01 2003
 *
 * @brief  Interface for Plugin Manager
 *
 *
 */

#ifndef _PLUGINMANAGERINTERFACE_H_
#define _PLUGINMANAGERINTERFACE_H_


/*
  class ConnectionPlugin;
  class DefaultPlugin;
  class DatabasePlugin;
  class PrePostPlugin;
  class LogPlugin;
  class Plugin;
*/

#include "ConnectionPlugin.h"
#include "DefaultPlugin.h"
#include "DatabasePlugin.h"
#include "PrePostPlugin.h"
#include "LogPlugin.h"

class PluginManagerInterface : public Plugin {

    Q_OBJECT

        public:

    /**
     * Constructor.
     *
     */
    PluginManagerInterface(PluginManagerInterface* parent = 0, const QString& d = QString::null)
        : Plugin(parent), dir(d.isNull()?QDir::currentPath():d) {}

    /**
     * Destructor.
     *
     */
    virtual ~PluginManagerInterface() {}

    /**
     * Associated connection plugin.
     *
     *
     * @return pointer to the plugin
     */
    virtual ConnectionPlugin* connectionPlugin() const = 0;

    /**
     * Associated database plugin.
     *
     *
     * @return pointer to the plugin.
     */
    virtual DatabasePlugin* databasePlugin() const = 0;

    /**
     * Associated default plugin.
     *
     *
     * @return pointer to the plugin.
     */
    virtual DefaultPlugin* defaultPlugin() const = 0;

    /**
     * Associated prepost plugin.
     *
     *
     * @return pointer to the plugin.
     */
    virtual PrePostPlugin* prepostPlugin() const = 0;

    /**
     * Associated log plugin.
     *
     *
     * @return pointer to the log plugin.
     */
    virtual LogPlugin* logPlugin() const = 0;

    /**
     * Find a plugin by its name
     * @param name Name of the plugin
     *
     * @return pointer to the plugin, or NULL if it does not exist.
     */
    virtual Plugin* getPluginByName(const QString& name) const = 0;

    /**
     * Registers a command from a plugin.
     *
     * @param plugin handler to the plugin.
     * @param prefix prefix that will call the command.
     * @param id handler to a method in the plugin.
     * @param description description of the command
     */
    virtual void registerCommand(Plugin* plugin, const QString & prefix,
                                 int id, const QString& pattern,
                                 const QString & description = QString::null,
                                 const QString& group = QString::null) = 0;

    /**
     * Dispatches a line to the right plugin.
     *
     * @param from user who emitted the message.
     * @param txt the message.
     */
    virtual void dispatch(const QString& from, const QString & txt) = 0;

    /**
     * A user comes in (make all plugins aware of it).
     *
     * @param login login of the user.
     */
    virtual void in(const QString& login) = 0;

    /**
     * A user leaves (make all plugins aware of it).
     *
     * @param login login of the user.
     */
    virtual void out(const QString& login) = 0;

    /**
     * A user is killed (make all plugins aware of it).
     *
     * @param login login of the user.
     */
    virtual void killed(const QString& login) = 0;

    /**
     * A user is renamed (make all plugins aware of it).
     *
     * @param login login of the user.
     */
    virtual void renamed(const QString& old_login, const QString& new_login) = 0;

    /**
     * Have a specific directory for data storage.
     *
     * @param d name of the directory.
     */
    void setDataDir(const QString& d) {dir = d;}

    /**
     * Returns the name of the data directory.
     *
     *
     * @return name.
     */
    const QString& dataDir() const {return dir;}

private:
    QString dir;
};

#endif
