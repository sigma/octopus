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
 * @file   DatabasePlugin.h
 * @author Yann Hodique
 * @date   Sat Mar  1 15:51:31 2003
 * 
 * @brief  Interface for a database plugin
 * 
 * 
 */

#ifndef _DATABASEPLUGIN_H_
#define _DATABASEPLUGIN_H_

#include <string>

#include "Plugin.h"

using std::string;
    
typedef enum {
    // default = PUBLIC
    PUBLIC = 1,
    PRIVATE = 2,
    INFO = 4,

    // default = EXTERNAL
    EXTERNAL = 8,
    INTERNAL = 16,

    // default = RESIDENT
    RESIDENT = 32,
    TRANSIENT = 64,

    // default = LOCAL
    LOCAL = 128,
    GLOBAL = 256,

    // default = UNENCRYPTED
    UNENCRYPTED = 512,
    ENCRYPTED = 1024
} Access;

#define DEFAULT_MODE (PUBLIC | EXTERNAL | RESIDENT | LOCAL | UNENCRYPTED)

typedef enum {
    STRING,
    INT,
    BOOL
} Type;

class DatabasePlugin : public Plugin {

    Q_OBJECT

public:
    /**
     * Constructor.
     * 
     * @param parent corresponding plugin manager. 
     * 
     */
    DatabasePlugin(PluginManagerInterface* parent=0) : Plugin(parent) {}

    /**
     * Destructor.
     * 
     * 
     */
    virtual ~DatabasePlugin() {}

    /**
     * Determines if a login candidate is correct.
     * 
     * @param login login to test.
     * 
     * @return true in case of correctness.
     */
    virtual bool isValidLogin(const QString& login) const = 0;

    /**
     * Maximal length of a login.
     * 
     * 
     * @return max.
     */
    virtual int validLoginLength() const = 0;

    /**
     * Tests if a user is registered.
     * 
     * @param login user to test.
     * 
     * @return true if the user is registered.
     */
    virtual bool isRegisteredUser(const QString& login) const = 0;

    /**
     * Returns the canonical name of a user.
     * 
     * @param login One of the strings representing the user.
     *
     * @return canonical Id.
     */
    virtual const QString& canonicalName(const QString& login) const {
        return login;
    }

    /**
     * User authentification.
     * 
     * @param login login of the user.
     * @param pass provided password.
     * 
     * @return true if the provided password corresponds to the user.
     */
    virtual bool authUser(const QString& login, const QString& pass) const = 0;

    /**
     * Tests if a user is in a group (and automatically creates it if needed).
     * 
     * @param login login of the user.
     * @param group name of the group.
     * 
     * @return true if the user is in the group.
     */
    virtual bool inGroup(const QString& login, const QString& group) = 0;

    virtual bool isDefinedVariable(const QString& login, const QString& label) const = 0;

    virtual void defineVariable(const QString& login, const QString& label, int mode = DEFAULT_MODE, int type = STRING, const QString& def = QString::null) = 0;

    /**
     * Sets a user variable in a hierarchical tree.
     * 
     * @param login login of the user.
     * @param label name of the variable (e.g. path/to/myvariable)
     * @param value value of the variable.
     * @param notify controls plugin notification for internal variables
     *
     * @return true if affectation was successful.
     */
    virtual bool setValue(const QString& login, const QString& label, const QString& value, bool notify = true) = 0;

    /**
     * Unsets a user variable in a hierarchical tree.
     * 
     * @param login login of the user.
     * @param label name of the variable (e.g. path/to/myvariable)
     */
    virtual void undefineVariable(const QString& login, const QString& label) = 0;

    /**
     * Gets the value of a user variable in a hierarchical tree.
     * 
     * @param login login of the user.
     * @param label name of the variable (e.g. path/to/myvariable)
     */
    virtual QString getValue(const QString& login, const QString& label) const = 0;

    virtual void registerInternalVariable(Plugin*,const QString& name, int mode = DEFAULT_MODE, int type = STRING, const QString& def = QString::null) = 0;

protected:
    /**
     * Creates a new user.
     * 
     * @param login login of the new user.
     */
    virtual void createUser(const QString& login) = 0;

    /**
     * Deletes a user.
     * 
     * @param login login of the user.
     */
    virtual void deleteUser(const QString& login) = 0;

    /**
     * Tests if a group exists.
     * 
     * @param group name of the group.
     * 
     * @return true if group exists.
     */
    virtual bool existGroup(const QString& group) const = 0;

    /**
     * Creates a new group.
     * 
     * @param group name of the new group.
     */
    virtual void createGroup(const QString& group) = 0;

    /**
     * Deletes a group.
     * 
     * @param group name of the group.
     */
    virtual void deleteGroup(const QString& group) = 0;

    /**
     * Adds a user to a group.
     * 
     * @param login login of the user.
     * @param group name of the group.
     */
    virtual void addToGroup(const QString& login, const QString& group) = 0;

    /**
     * Removes a user from a group.
     * 
     * @param login login of the user.
     * @param group name of the group.
     */
    virtual void removeFromGroup(const QString& login, const QString& group) = 0;

};

#endif
