//
//
// C++ Implementation:
//
// Description:
//
//
// Author: Yann Hodique <Yann.Hodique@lifl.fr>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include <unistd.h>

#include <qsqldatabase.h>

#include "pgdb.h"

#define DRIVER "QPSQL7"
#define DBNAME "mtp"
#define USER "mtp"
#define PASSWD "plokiploki"
#define HOST "localhost"

#define MAX_LENGTH 8
#define SEED "fr"

PLUGIN_FACTORY(PgDb);

PgDb::PgDb(PluginManagerInterface* parent)
        : DatabasePlugin(parent) {
    
    defaultDB = QSqlDatabase::addDatabase( DRIVER );
    defaultDB->setDatabaseName( DBNAME );
    defaultDB->setUserName( USER );
    defaultDB->setPassword( PASSWD );
    defaultDB->setHostName( HOST );
    
    if ( defaultDB->open() ) {
    }
    
}


PgDb::~PgDb() {}

void PgDb::exportCommands() {}

void PgDb::registerInternalVariable(Plugin*,const QString&,int,int,const QString&) {}

bool PgDb::authUser(const QString& login, const QString& pass) const {
    QSqlQuery query("SELECT password FROM users WHERE user_name='" + login + "';",defaultDB);
    if (query.isActive() && query.next()) {
        return (query.value(0).toString() == QString(crypt(pass.ascii(),SEED)));
    }
    else return false;
}

bool PgDb::existGroup(const QString& group) const {
    QSqlQuery query("SELECT group_id FROM groups WHERE group_name='" + group + "';",defaultDB);
    return (query.isActive() && query.next());
}

bool PgDb::inGroup(const QString&, const QString&) {
    return false;
}

bool PgDb::isRegisteredUser(const QString& login) const {
    QSqlQuery query("SELECT user_registered FROM users WHERE user_name='" + login + "';",defaultDB);
    if (query.isActive() && query.next()) {
	return (query.value(0).toBool());
    }
    else return false;    
}

bool PgDb::isValidLogin(const QString&) const {
    return true;
}

int PgDb::validLoginLength() const {
    return MAX_LENGTH;
}

QString PgDb::canonicalName(const QString& login) {
    return login;
}

QString PgDb::getValue(const QString&, const QString&) const {
    return QString::null;
}

void PgDb::addToGroup(const QString&, const QString&) {}

void PgDb::createGroup(const QString&) {}

void PgDb::createUser(const QString&) {}

void PgDb::deleteGroup(const QString&) {}

void PgDb::deleteUser(const QString&) {}

void PgDb::removeFromGroup(const QString&, const QString&) {}

bool PgDb::setValue(const QString&, const QString&, const QString&, bool) {
    return false;
}

void PgDb::defineVariable(const QString&, const QString&, int, int, const QString&) {}

void PgDb::undefineVariable(const QString&, const QString&) {}

bool PgDb::isDefinedVariable(const QString&, const QString&) const {
    return false;
}
