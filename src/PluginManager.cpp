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
#include <typeinfo>

#include <QCoreApplication>
#include <QTimer>

#include "PluginManager.h"

using namespace std;

PLUGIN_FACTORY(PluginManager)

PluginManager::PluginManager(PluginManagerInterface*, const QString& d) : PluginManagerInterface(this,d) {
    connect = 0;
    def = 0;
    data = 0;
    prepost = 0;
    log = 0;

    registerCommand("reboot",&PluginManager::rebootCmd,QString::null,"Reboot the server","System");
    registerCommand("shutdown",&PluginManager::shutdownCmd,QString::null,"Halt the server","System");
    registerCommand("commands",&PluginManager::commandsCmd,QString::null,"Display the list of available commands");
    registerCommand("modprobe",&PluginManager::modprobeCmd,"{<value>}","Load a new extension","System");
    registerCommand("rmmod",&PluginManager::rmmodCmd,"{<word>}","Unload an extension","System");
    registerCommand("lsmod",&PluginManager::lsmodCmd,QString::null,"List loaded extensions","System");
}

PluginManager::~PluginManager() {
    destroyPlugins();
    exit(0);
}

void PluginManager::destroyPlugins() {
    for(std::list<Data>::iterator it = active.begin(); it != active.end(); ++it) {
        destroy_t* destroy_plugin = (destroy_t*) dlsym((*it).handler, "destroy");
        destroy_plugin((*it).plugin);
        dlclose((*it).handler);
    }
    active.clear();
    available_commands.clear();
    connect = 0;
    def = 0;
    data = 0;
    prepost = 0;
    log = 0;
}

void PluginManager::loadPluginsDirectory(QString /*dir_name*/) {}

Plugin* PluginManager::loadPluginFile(QString file_name, QString *error) {

    Data d;

    void* plug = dlopen(file_name, RTLD_LAZY);
    if (!plug) {
        *error = QString("Cannot load library %1 : %2").arg(file_name).arg(dlerror());
        return 0;
    }
    d.handler = plug;

    create_t* create_plugin = (create_t*) dlsym(plug, "create");
    destroy_t* destroy_plugin = (destroy_t*) dlsym(plug, "destroy");
    string_func_t* pluginName = (string_func_t*) dlsym(plug, "pluginName");
    if (!create_plugin || !destroy_plugin || !pluginName) {
        *error = QString("cannot load %1 symbols : %2").arg(file_name).arg(dlerror());
        return 0;
    }

    // Unicity of plugin name
    if (isLoadedPlugin(pluginName())) {
        *error = pluginName() + " is already loaded.";
        return 0;
    }

    Plugin* p = create_plugin(this);
    p->exportCommands();
    d.plugin = p;

    active.push_front(d);

    return p;
}

void PluginManager::loadConnectionPluginFile(QString file_name) {
    QString error;
    connect = static_cast<ConnectionPlugin*>(loadPluginFile(file_name,&error));
    if(!connect) {
        octInfo(QString("Incorrect Connection Plugin : %1\n").arg(error));
        exit(1);
    }
}

void PluginManager::loadDefaultPluginFile(QString file_name) {
    QString error;
    def = static_cast<DefaultPlugin*>(loadPluginFile(file_name, &error));
    if(!def) {
        octInfo(QString("Incorrect Default Plugin : %1\n").arg(error));
        exit(1);
    }
}

void PluginManager::loadDatabasePluginFile(QString file_name) {
    QString error;
    data = static_cast<DatabasePlugin*>(loadPluginFile(file_name, &error));
    if(!data) {
        octInfo(QString("Incorrect Database Plugin : %1\n").arg(error));
        exit(1);
    }
}

void PluginManager::loadPrePostPluginFile(QString file_name) {
    QString error;
    prepost = static_cast<PrePostPlugin*>(loadPluginFile(file_name, &error));
    if(!prepost) {
        octInfo(QString("Incorrect PrePost Plugin : %1\n").arg(error));
        exit(1);
    }
}

void PluginManager::loadLogPluginFile(QString file_name) {
    QString error;
    log = static_cast<LogPlugin*>(loadPluginFile(file_name, &error));
    if (!log) {
        octInfo(QString("Incorrect Log Plugin : %1\n").arg(error));
        exit(1);
    }
}

ConnectionPlugin* PluginManager::connectionPlugin() const {
    return connect;
}

DatabasePlugin* PluginManager::databasePlugin() const {
    return data;
}

DefaultPlugin* PluginManager::defaultPlugin() const {
    return def;
}

PrePostPlugin* PluginManager::prepostPlugin() const {
    return prepost;
}

LogPlugin* PluginManager::logPlugin() const {
    return log;
}

Plugin* PluginManager::getPluginByName(const QString& name) const {
    for(std::list<Data>::const_iterator it = active.begin(); it != active.end(); ++it) {
        if((*it).plugin->pluginName() == name)
            return (*it).plugin;
    }
    return NULL;
}

void PluginManager::registerCommand(Plugin* plugin, const QString & prefix,
                             int id, const QString& pattern,
                             const QString & description,
                             const QString& group) {
    ComRef cr;
    bool already_present = false;

    if(available_commands.contains(prefix)) {
        cr = available_commands.value(prefix);
        already_present = true;
    }
    else {
        cr.plugin = plugin;
        cr.id = id;
        cr.description = description;
    }

    ComScheme sch;
    sch.pattern = pattern;
    buildRegexp(&sch);
    sch.pattern.replace(QChar('{'),QString::null).replace(QChar('}'),QString::null);
    sch.group = group;

    cr.groups_behavior << sch;

    available_commands.insert(prefix,cr);

    if(!already_present) {
        octInfo(QString("Registered command \"%1\"\n").arg(prefix));
    }
}

bool PluginManager::isLoadedPlugin(const QString &name) const {
    for(std::list<Data>::const_iterator it = active.begin(); it != active.end(); ++it)
        if (it->plugin->pluginName() == name)
            return true;
    return false;
}

void PluginManager::dispatch(const QString & from, const QString & msg) {
    CommandMap::Iterator it;
    uint length = 0;
    for(it = available_commands.begin(); it != available_commands.end(); ++it) {
        if(msg.startsWith(it.key()) && ((msg[it.key().length()] == ' ' || msg.length() == it.key().length()))) {
            length = it.key().length();
            break;
        }
    }
    ComRef &cr = it.value();
    if (length) {
        bool ok = false;
        for(QList<ComScheme>::ConstIterator iter = cr.groups_behavior.begin(); iter !=  cr.groups_behavior.end(); ++iter) {
            const ComScheme &sch = *iter;
            if(sch.group.isNull() || databasePlugin()->inGroup(from,sch.group)) {
                if(sch.regexp.exactMatch(msg.right(msg.length() - length))) {
                    QStringList args;
                    QList<int>::ConstIterator iterator = sch.regexp_positions.begin();
                    for(;iterator != sch.regexp_positions.end(); ++iterator) {
                        QRegExp re = sch.regexp;
                        args << re.cap(*iterator);
                    }
                    (cr.plugin)->callCommand(cr.id,from,args);
                }
                else {
                    connectionPlugin()->serverSend(from, "Usage: " + it.key() + " " + sch.pattern);
                }
                ok = true;
                break;
            }
        }
        if(!ok) {
            connectionPlugin()->serverSend(from, it.key() + ": permission denied.");
        }
    }
    else {
        defaultPlugin()->manageMessage(from, msg);
    }
}

void PluginManager::in(const QString& login) {
    for(std::list<Data>::iterator it = active.begin(); it != active.end(); ++it) {
        (*it).plugin->incomingUser(login);
    }
}

void PluginManager::out(const QString& login) {
    for(std::list<Data>::iterator it = active.begin(); it != active.end(); ++it) {
        (*it).plugin->outgoingUser(login);
    }
}

void PluginManager::killed(const QString& login) {
    for(std::list<Data>::iterator it = active.begin(); it != active.end(); ++it) {
        (*it).plugin->killedUser(login);
    }
}

void PluginManager::renamed(const QString& old_login, const QString& new_login) {
    for(std::list<Data>::iterator it = active.begin(); it != active.end(); ++it) {
        (*it).plugin->renamedUser(old_login, new_login);
    }
}

void PluginManager::rebootCmd(const QString&, const QStringList&) {
    QString txt("System rebooting now");
    connectionPlugin()->serverBroadcast(txt);

    for(std::list<Data>::iterator it = active.begin(); it != active.end(); ++it) {
        (*it).plugin->shutdown();
    }
//            QTimer::singleShot(1000,this,SLOT(slotReboot()));
    slotReboot();
}

void PluginManager::shutdownCmd(const QString&, const QStringList&) {
    std::cout << "System halting now" << std::endl;
    QString txt("System halting now");
    connectionPlugin()->serverBroadcast(txt);

    for(std::list<Data>::iterator it = active.begin(); it != active.end(); ++it) {
        std::cout << QString("Shutdown plugin %1").arg((*it).plugin->pluginName()).ascii() << std::endl;
        (*it).plugin->shutdown();
        std::cout << QString("Plugin %1 unloaded").arg((*it).plugin->pluginName()).ascii() << std::endl;
    }
    delete this;
//            QTimer::singleShot(1000,this,SLOT(deleteLater()));
}

void PluginManager::commandsCmd(const QString& from, const QStringList&) {
    QString message;
    connectionPlugin()->serverSend(from, "Commands:");
    if (this->desc_map.count()) {
        if (message != "")
            message += "\n";
        message += QString("%1 commands:").arg(this->pluginName());
        for (QMap<QString,QString>::Iterator commandIt = this->desc_map.begin(); commandIt != this->desc_map.end(); ++commandIt) {
            message += "\n  " + QString("%1 %2").arg(commandIt.key(), -12).arg(commandIt.value(), -60);
        }
    }
    for(std::list<Data>::iterator it = active.begin(); it != active.end(); ++it) {
        if ((*it).plugin->desc_map.count()) {
            if (message != "")
                message += "\n";
            message += QString("%1 commands:").arg((*it).plugin->pluginName());
            for (QMap<QString,QString>::Iterator commandIt = (*it).plugin->desc_map.begin(); commandIt != (*it).plugin->desc_map.end(); ++commandIt) {
                message += "\n  " + QString("%1 %2").arg(commandIt.key(), -12).arg(commandIt.value(), -60);
            }
        }
    }
    connectionPlugin()->send(from, message);
    message = "End of Commands";
    connectionPlugin()->serverSend(from,message);
}

/*!
    \fn PluginManager::modprobeCmd(const QString& from, const QString& msg)
 */
void PluginManager::modprobeCmd(const QString& from, const QStringList& list) {
    QString txt;
    QString error;
    if(loadPluginFile(dataDir() + "/plugins/" + list[0], &error)) {
        txt = "Plugin loaded";
    } else {
        txt = "Error during plugin loading : " + error;
    }
    connectionPlugin()->serverSend(from,txt);
}

/*!
    \fn PluginManager::lsmodCmd(const QString& from, const QString& msg)
 */
void PluginManager::lsmodCmd(const QString& from, const QStringList&) {
    QString txt;
    connectionPlugin()->serverSend(from, "Active plugins :");
    for(std::list<Data>::iterator it = active.begin(); it != active.end(); ++it) {
        if (txt != "")
            txt += "\n";
        txt += (*it).plugin->pluginName();
    }
    connectionPlugin()->send(from, txt);
    txt = "End of plugins";
    connectionPlugin()->serverSend(from, txt);
}

/*!
    \fn PluginManager::rmmodCmd(const QString& from, const QString& msg)
 */
void PluginManager::rmmodCmd(const QString& from, const QStringList& list) {
    for(std::list<Data>::iterator it = active.begin(); it != active.end(); ++it) {
        if(list[0] == (*it).plugin->pluginName()) {
            Plugin* plug = (*it).plugin;
            if(plug == defaultPlugin() || plug == databasePlugin() || plug == connectionPlugin() || plug == prepostPlugin() || plug == logPlugin()) {
                QString txt("Plugin cannot be removed");
                connectionPlugin()->serverSend(from,txt);
                return;
            }
            active.erase(it);
            for(CommandMap::Iterator iter = available_commands.begin(); iter != available_commands.end(); ++iter)
                if(iter.value().plugin == plug)
                    available_commands.erase(iter);
            destroy_t* destroy_plugin = (destroy_t*) dlsym((*it).handler, "destroy");
            destroy_plugin(plug);
            dlclose((*it).handler);
            QString txt("Plugin unloaded");
            connectionPlugin()->serverSend(from,txt);
            return;
        }
    }
    QString txt("No such plugin \"" + list[0] + "\"");
    connectionPlugin()->serverSend(from,txt);
}

void PluginManager::slotReboot() {
    destroyPlugins();
    boot();
}

bool PluginManager::boot() {
    QString error;

    loadConnectionPluginFile(connection_name);
    loadDatabasePluginFile(database_name);
    loadDefaultPluginFile(default_name);
    loadPrePostPluginFile(prepost_name);
    loadLogPluginFile(log_name);
    for(QStringList::Iterator it = other_plugins.begin(); it != other_plugins.end(); ++it)
        loadPluginFile(*it, &error);
    if(connectionPlugin() && defaultPlugin() && databasePlugin() && prepostPlugin() && logPlugin()) {
        if(ports.count())
            for(QList<int>::Iterator it = ports.begin(); it != ports.end(); ++it)
                connectionPlugin()->launch(*it);
        else
            connectionPlugin()->launch(DEFAULT_PORT);
//        std::cout << "boot() ok" << std::endl;
        return true;
    } else {
//        std::cout << "boot() ko" << std::endl;
        return false;
    }
}

void PluginManager::buildRegexp(ComScheme* sch) {
    QString re(sch->pattern);

    re.replace(QChar('['),"(")
        .replace(QChar(']'),")?")
        .replace(QChar(' ')," +")
        .replace(QChar('.'),"\\.")
        .replace("<int>","\\d+")
        .replace("<word>","[^ ,]+")
        .replace("<wordlist>","[^ ,]+(,[^ ,]+)*")
        .replace("<value>","[^ ]+")
        .replace("<sentence>","[^ ].*")
        .replace("<access>","(public|private)")
        .replace("<mode>","(resident|transient)")
        .replace("<type>","(string|int|bool)")
        .replace("<login>","\\w+")
        .replace("<loginlist>","\\w+(,\\w+)*")
        .replace(QChar('}'),")");

    int index=0;
    int pos = 0;
    QRegExp rx("[\\(\\{]");
    while(index != -1) {
        index = re.indexOf(rx,index);
        if(index != -1) {
            pos++;
            if(re[index] == '{') {
                re[index] = '(';
                sch->regexp_positions << pos;
            }
            index++;
        }
    }
    sch->regexp = QRegExp(" *" + re + " *");
}
