//
//
// C++ Interface:
//
// Description:
//
//
// Author: Yann Hodique <Yann.Hodique@lifl.fr>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef PGDB_H
#define PGDB_H

#include "plugins.h"

/**
PostgreSql plugin for QOctopus
 
@author Yann Hodique
*/

class QSqlDatabase;

class PgDb : public DatabasePlugin {

    Q_OBJECT
    PLUGIN_OBJECT(PgDb)

public:
    PgDb(PluginManagerInterface* parent = 0);

    ~PgDb();

    void exportCommands();
    
    void registerInternalVariable(Plugin*,const QString&, int mode = DEFAULT_MODE, int type = STRING, const QString& def = QString::null);
    
    bool authUser(const QString& login, const QString& pass) const;
    bool existGroup(const QString& group) const;
    bool inGroup(const QString& login, const QString& group);
    bool isRegisteredUser(const QString& login) const;
    bool isValidLogin(const QString& login) const;
    int validLoginLength() const;
    QString canonicalName(const QString& login);
    QString getValue(const QString& login, const QString& label) const;
    void addToGroup(const QString& login, const QString& group);
    void createGroup(const QString& group);
    void createUser(const QString& login);
    void deleteGroup(const QString& group);
    void deleteUser(const QString& login);
    void removeFromGroup(const QString& login, const QString& group);
    bool setValue(const QString& login, const QString& label, const QString& value, bool notify = true);
    void defineVariable(const QString& login, const QString& label, int mode = 0, int type = 0, const QString& def = QString::null);
    void undefineVariable(const QString& login, const QString& label);
    bool isDefinedVariable(const QString&, const QString&) const;
    

private:
    QSqlDatabase *defaultDB;

};

#endif
