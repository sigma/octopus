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

#include "plugins.h"

#include <list>
#include <qmap.h>
#include <qstringlist.h>
#include <qregexp.h>

#define DEFAULT_PORT 4000

class DatabasePlugin;
class DefaultPlugin;
class PrePostPlugin;
class ConnectionPlugin;
class LogPlugin;

class PluginManager : public PluginManagerInterface {

    Q_OBJECT
    PLUGIN_OBJECT(PluginManager)

    typedef struct {
        Plugin* plugin;
        void* handler;
    }
    Data;

    typedef struct {
        QString pattern;
        QRegExp regexp;
        QList<int> regexp_positions;
        QString group;
    } ComScheme;

    typedef struct {
        Plugin* plugin;
        int id;
        QString description;
        QList<ComScheme> groups_behavior;
    } ComRef;

    typedef QMap<QString,ComRef> CommandMap;

    typedef void(PluginManager::*Command)(const QString&, const QString&);

public:
    PluginManager(PluginManagerInterface* parent = 0, const QString& d = QString::null);
    ~PluginManager();
    void dispatch(const QString &, const QString &);

    void exportCommands() {}

    void destroyPlugins();

    // access to required plugins
    ConnectionPlugin* connectionPlugin() const;
    DatabasePlugin* databasePlugin() const;
    DefaultPlugin* defaultPlugin() const;
    PrePostPlugin* prepostPlugin() const;
    LogPlugin* logPlugin() const;
    Plugin* getPluginByName(const QString& name) const;

    void registerCommand(Plugin* plugin, const QString & prefix,
                                 int id, const QString& pattern,
                                 const QString & description = QString::null,
                                 const QString& group = QString::null);

    void in(const QString& /*login*/);
    void out(const QString& /*login*/);
    void killed(const QString& /*login*/);
    void renamed(const QString& /*old_login*/, const QString& /*new_login*/);

    void rebootCmd(const QString&, const QStringList&);
    void shutdownCmd(const QString&, const QStringList&);
    void commandsCmd(const QString&, const QStringList&);
    void modprobeCmd(const QString&, const QStringList&);
    void lsmodCmd(const QString&, const QStringList&);
    void rmmodCmd(const QString&, const QStringList&);

public slots:
    void slotReboot();

private:
    std::list<Data> active;
    ConnectionPlugin *connect;
    DefaultPlugin *def;
    DatabasePlugin *data;
    PrePostPlugin *prepost;
    LogPlugin *log;

    QString connection_name, default_name, database_name, prepost_name, log_name;
    QStringList other_plugins;
    QList<int> ports;

    CommandMap available_commands;

    bool isLoadedPlugin(const QString &name) const;

    void loadPluginsDirectory(QString dir_name);
    Plugin* loadPluginFile(QString file_name, QString *error);
    void loadConnectionPluginFile(QString file_name);
    void loadDefaultPluginFile(QString file_name);
    void loadDatabasePluginFile(QString file_name);
    void loadPrePostPluginFile(QString file_name);
    void loadLogPluginFile(QString file_name);

    void buildRegexp(ComScheme*);

public:
    void addConnectionPort(int port) {
        ports << port;
    }

    void addPluginFile(QString file_name) {
        other_plugins << file_name;
    }
    void setConnectionPluginFile(QString file_name) {
        connection_name = file_name;
    }
    void setDefaultPluginFile(QString file_name) {
        default_name = file_name;
    }
    void setDatabasePluginFile(QString file_name) {
        database_name = file_name;
    }
    void setPrePostPluginFile(QString file_name) {
        prepost_name = file_name;
    }
    void setLogPluginFile(QString file_name) {
        log_name = file_name;
    }

    bool boot();
};

#endif // PLUGINMANAGER_H
