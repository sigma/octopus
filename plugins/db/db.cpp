//
//
// C++ Implementation: cpp
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
#include <iostream>

#include <qregexp.h>
#include <qfile.h>
#include <QTextStream>

#include "db.h"
#include "domutil.h"

#define MAX_LENGTH 8
#define SEED "fr"

PLUGIN_FACTORY(Db);

Db::Db(PluginManagerInterface* parent) : DatabasePlugin(parent) {
    bool exist = loadDatas();
    if(!exist) {
        QString def("<db/>");
        m_document.setContent(def);
    }

    m_modes
        << (PUBLIC | EXTERNAL | RESIDENT | LOCAL | UNENCRYPTED)
        << (PRIVATE | EXTERNAL | RESIDENT | LOCAL | UNENCRYPTED)
        << (INFO | EXTERNAL | RESIDENT | LOCAL | UNENCRYPTED)
        << (PUBLIC | INTERNAL | RESIDENT | LOCAL | UNENCRYPTED)
        << (PRIVATE | INTERNAL | RESIDENT | LOCAL | UNENCRYPTED)
        << (INFO | INTERNAL | RESIDENT | LOCAL | UNENCRYPTED)
        << (PUBLIC | EXTERNAL | TRANSIENT | LOCAL | UNENCRYPTED)
        << (PRIVATE | EXTERNAL | TRANSIENT | LOCAL | UNENCRYPTED)
        << (INFO | EXTERNAL | TRANSIENT | LOCAL | UNENCRYPTED)
        << (PUBLIC | INTERNAL | TRANSIENT | LOCAL | UNENCRYPTED)
        << (PRIVATE | INTERNAL | TRANSIENT | LOCAL | UNENCRYPTED)
        << (INFO | INTERNAL | TRANSIENT | LOCAL | UNENCRYPTED)
        << (PUBLIC | EXTERNAL | RESIDENT | GLOBAL | UNENCRYPTED)
        << (PRIVATE | EXTERNAL | RESIDENT | GLOBAL | UNENCRYPTED)
        << (INFO | EXTERNAL | RESIDENT | GLOBAL | UNENCRYPTED)
        << (PUBLIC | INTERNAL | RESIDENT | GLOBAL | UNENCRYPTED)
        << (PRIVATE | INTERNAL | RESIDENT | GLOBAL | UNENCRYPTED)
        << (INFO | INTERNAL | RESIDENT | GLOBAL | UNENCRYPTED)
        << (PUBLIC | EXTERNAL | TRANSIENT | GLOBAL | UNENCRYPTED)
        << (PRIVATE | EXTERNAL | TRANSIENT | GLOBAL | UNENCRYPTED)
        << (INFO | EXTERNAL | TRANSIENT | GLOBAL | UNENCRYPTED)
        << (PUBLIC | INTERNAL | TRANSIENT | GLOBAL | UNENCRYPTED)
        << (PRIVATE | INTERNAL | TRANSIENT | GLOBAL | UNENCRYPTED)
        << (INFO | INTERNAL | TRANSIENT | GLOBAL | UNENCRYPTED)
        << (PUBLIC | EXTERNAL | RESIDENT | LOCAL | ENCRYPTED)
        << (PRIVATE | EXTERNAL | RESIDENT | LOCAL | ENCRYPTED)
        << (INFO | EXTERNAL | RESIDENT | LOCAL | ENCRYPTED)
        << (PUBLIC | INTERNAL | RESIDENT | LOCAL | ENCRYPTED)
        << (PRIVATE | INTERNAL | RESIDENT | LOCAL | ENCRYPTED)
        << (INFO | INTERNAL | RESIDENT | LOCAL | ENCRYPTED)
        << (PUBLIC | EXTERNAL | TRANSIENT | LOCAL | ENCRYPTED)
        << (PRIVATE | EXTERNAL | TRANSIENT | LOCAL | ENCRYPTED)
        << (INFO | EXTERNAL | TRANSIENT | LOCAL | ENCRYPTED)
        << (PUBLIC | INTERNAL | TRANSIENT | LOCAL | ENCRYPTED)
        << (PRIVATE | INTERNAL | TRANSIENT | LOCAL | ENCRYPTED)
        << (INFO | INTERNAL | TRANSIENT | LOCAL | ENCRYPTED)
        << (PUBLIC | EXTERNAL | RESIDENT | GLOBAL | ENCRYPTED)
        << (PRIVATE | EXTERNAL | RESIDENT | GLOBAL | ENCRYPTED)
        << (INFO | EXTERNAL | RESIDENT | GLOBAL | ENCRYPTED)
        << (PUBLIC | INTERNAL | RESIDENT | GLOBAL | ENCRYPTED)
        << (PRIVATE | INTERNAL | RESIDENT | GLOBAL | ENCRYPTED)
        << (INFO | INTERNAL | RESIDENT | GLOBAL | ENCRYPTED)
        << (PUBLIC | EXTERNAL | TRANSIENT | GLOBAL | ENCRYPTED)
        << (PRIVATE | EXTERNAL | TRANSIENT | GLOBAL | ENCRYPTED)
        << (INFO | EXTERNAL | TRANSIENT | GLOBAL | ENCRYPTED)
        << (PUBLIC | INTERNAL | TRANSIENT | GLOBAL | ENCRYPTED)
        << (PRIVATE | INTERNAL | TRANSIENT | GLOBAL | ENCRYPTED)
        << (INFO | INTERNAL | TRANSIENT | GLOBAL | ENCRYPTED);

    registerInternalVariable(this,"name", PUBLIC | INTERNAL | RESIDENT | GLOBAL | UNENCRYPTED, STRING);
    registerInternalVariable(this,"email", PUBLIC | INTERNAL | RESIDENT | GLOBAL | UNENCRYPTED, STRING);
    registerInternalVariable(this,"password", PRIVATE | INTERNAL | RESIDENT | GLOBAL | ENCRYPTED, STRING);
    registerInternalVariable(this,"client", PUBLIC | INTERNAL | GLOBAL | TRANSIENT | UNENCRYPTED, STRING);
    registerInternalVariable(this,"bell", PRIVATE | INTERNAL | GLOBAL | TRANSIENT | UNENCRYPTED, BOOL, "on");

    if(!exist) {
        createUser("Root");
        setValue("Root","name","Yann Hodique");
        setValue("Root","email","hodique@lifl.fr");
        registerUser("Root","Root");
        createUser("Mtp");
        setValue("Mtp","name","Yann Hodique");
        setValue("Mtp","email","hodique@lifl.fr");
        registerUser("Mtp","Mtp");

        storeDatas();
    }
}

Db::~Db() {
    storeDatas();
}

bool Db::loadDatas() {
    QFile fin(manager()->dataDir() + "/database");
    if (!fin.open(QIODevice::ReadOnly)) {
        octInfo("Could not read config file\n");
        return false;
    }

    int errorLine, errorCol;
    QString errorMsg;
    if (!m_document.setContent(&fin, &errorMsg, &errorLine, &errorCol)) {
        octInfo(QString("This is not a valid config file.\nXML error in line %1, column %2:\n%3\n").arg(errorLine).arg(errorCol).arg(errorMsg));
        fin.close();
        return false;
    }
    if (m_document.documentElement().nodeName() !="db") {
        octInfo("This is not a valid config file.\n");
        fin.close();
        return false;
    }

    fin.close();
    return true;
}

bool Db::storeDatas() {
    QFile fout(manager()->dataDir() + "/database");
    if( !fout.open(QIODevice::WriteOnly) ) {
        octInfo("Could not write the config file.\n");
        return false;
    }

    QTextStream stream(&fout);
    stream << m_document.toString();
    fout.close();

    return true;
}

void Db::incomingUser(const QString& login) {
    if(!isRegisteredUser(login))
        createUser(login);
    setValue(login,"bell","on");
}

void Db::outgoingUser(const QString& login) {
    if(!isRegisteredUser(login))
        deleteUser(login);
    else {
        QDomElement child = DomUtil::elementByPath(m_document,"/users/" + login + "/public/internal/transient");
        child.parentNode().removeChild(child);
        child = DomUtil::elementByPath(m_document,"/users/" + login + "/public/external/transient");
        child.parentNode().removeChild(child);
        child = DomUtil::elementByPath(m_document,"/users/" + login + "/private/internal/transient");
        child.parentNode().removeChild(child);
        child = DomUtil::elementByPath(m_document,"/users/" + login + "/private/external/transient");
        child.parentNode().removeChild(child);
        child = DomUtil::elementByPath(m_document,"/users/" + login + "/info/internal/transient");
        child.parentNode().removeChild(child);
        child = DomUtil::elementByPath(m_document,"/users/" + login + "/info/external/transient");
        child.parentNode().removeChild(child);
    }
}

void Db::killedUser(const QString& login) {
    deleteUser(login);
}

bool Db::isValidLogin(const QString& login) const {
    QRegExp re("\\w+");
    return (re.exactMatch(login) && (login.length() <= MAX_LENGTH));
}

int Db::validLoginLength() const {
    return MAX_LENGTH;
}

void Db::registerUser(const QString& login, const QString& pass) {
    QStringList l = DomUtil::readListEntry(m_document,"/userlist/","user");
    if(l.indexOf(login) == -1) {
        l << login;
        guests.removeAll(login);
    }
    DomUtil::writeListEntry(m_document,"/userlist/","user",l);
    setValue(login,"password",pass);
}

bool Db::isRegisteredUser(const QString& login) const {
    QStringList l = DomUtil::readListEntry(m_document,"/userlist/","user");
    QRegExp re("^" + login + "$");
    re.setCaseSensitivity(Qt::CaseInsensitive);
    return l.filter(re).count();
}

void Db::createUser(const QString& login) {
    guests << login;
    for(VarMap::Iterator it = internal_vars.begin(); it != internal_vars.end(); ++it)
        defineVariable(login,it.key(),it.value().mode,it.value().type,it.value().def);
}

void Db::deleteUser(const QString& login) {
    QStringList l = DomUtil::readListEntry(m_document,"/userlist/","user");
    int index = l.indexOf(login);
    if(index != -1) {
        l.removeAt(index);
        DomUtil::writeListEntry(m_document,"/userlist/","user",l);
    } else {
        guests.removeAll(login);
    }
    QDomElement father = DomUtil::elementByPath(m_document,"/users");
    QDomElement child = DomUtil::elementByPath(m_document,"/users/" + login);
    father.removeChild(child);
}

bool Db::authUser(const QString& login, const QString& pass) const {
    return(getValue(login, "password") == QString(crypt(pass.toAscii(),SEED)));
}

bool Db::existGroup(const QString& group) const {
    QStringList l = DomUtil::readListEntry(m_document,"/grouplist/","group");
    return (l.indexOf(group) != -1 || (!group.endsWith("*") && (l.indexOf(group + "*") != -1)));
}

void Db::createGroup(const QString& group) {
    QStringList l1 = DomUtil::readListEntry(m_document,"/grouplist/","group");
    QStringList l2 = DomUtil::readListEntry(m_document,"/users/Root/groups","group");
    if(l1.indexOf(group) == -1) {
        l1 << group;
        l2 << group;
    }
    DomUtil::writeListEntry(m_document,"/grouplist/","group",l1);
    DomUtil::writeListEntry(m_document,"/users/Root/groups","group",l2);
}

void Db::deleteGroup(const QString& group) {
    QStringList l1 = DomUtil::readListEntry(m_document,"/grouplist/","group");
    QStringList l2 = DomUtil::readListEntry(m_document,"/users/Root/groups","group");
    l1.removeAll(group);
    l2.removeAll(group);
    DomUtil::writeListEntry(m_document,"/grouplist/","group",l1);
    DomUtil::writeListEntry(m_document,"/users/Root/groups","group",l2);
}

void Db::addToGroup(const QString& login, const QString& group) {
    createGroup(group);

    QStringList l = DomUtil::readListEntry(m_document,"/users/" + login + "/groups","group");
    if(l.indexOf(group) == -1)
        l << group;
    DomUtil::writeListEntry(m_document,"/users/" + login + "/groups","group",l);
}

void Db::removeFromGroup(const QString& login, const QString& group) {
    createGroup(group);

    QStringList l = DomUtil::readListEntry(m_document,"/users/" + login + "/groups","group");
    l.removeAll(group);
    DomUtil::writeListEntry(m_document,"/users/" + login + "/groups","group",l);
}

bool Db::inGroup(const QString& login, const QString& group) {
    createGroup(group); // so that Root is always member

    QStringList l = DomUtil::readListEntry(m_document,"/users/" + login + "/groups","group");
    return ((l.indexOf(group) != -1) || (l.indexOf(group + "*") != -1)); // lead group implies group
}

void Db::exportCommands() {
    registerCommand("register",&Db::registerCmd,"{<value>}","Register an account");
    registerCommand("addgroup",&Db::addgroupCmd,"{<login>} {<wordlist>}","Add a user to some groups");
    registerCommand("delgroup",&Db::delgroupCmd,"{<login>} {<wordlist>}","Remove a user from some groups");
    registerCommand("set",&Db::setCmd,"[{<login>}.]{<word>} {<sentence>}","Set a variable");
    registerCommand("define",&Db::defineCmd,"{<type>} [{<login>}.]{<access>}.{<mode>}.{<word>}","Define a variable");
    registerCommand("undef",&Db::undefCmd,"[{<login>}.]{<word>}","Undefine a variable");
    registerCommand("get",&Db::getCmd,"[{<login>}.]{<word>}","Get the value of a variable");
    registerCommand("kill",&Db::killCmd,"{<login>}","Definitively kill a user","System");
    registerCommand("rename",&Db::renameCmd,"{<login>} {<login>}","Rename a user","System");
    registerCommand("chpasswd",&Db::chpasswdCmd,"{<value>}","Change a password");
    registerCommand("users",&Db::usersCmd,QString::null,"Display the list of registered users");
    registerCommand("finger",&Db::fingerCmd,"[{<login>}]","Display infos on a user");
    registerCommand("groups",&Db::groupsCmd,"[{<login>}]","Display user's groups");
    registerCommand("grouplist",&Db::groupListCmd,QString::null,"Display existing groups");
}

const QString& Db::canonicalName(const QString& login) const {
    QString lower(login.toLower());
    // Research in guest
    for (QStringList::ConstIterator it = guests.begin(); it != guests.end(); ++it) {
        if ((*it).toLower() == lower)
            return *it;
    }

    // Research in registered
    QStringList users = DomUtil::readListEntry(m_document,"/userlist/","user");
    for (QStringList::Iterator it = users.begin(); it != users.end(); ++it) {
        if ((*it).toLower() == lower)
            return *it;
    }

    return login;
}

void Db::registerInternalVariable(Plugin* plug,const QString& label,int mode,int type, const QString& def) {
    QStringList l = DomUtil::readListEntry(m_document,"/userlist/","user");
    for(QStringList::Iterator it = l.begin(); it != l.end(); ++it) {
        defineVariable(*it,label,mode,type,def);
    }
    Record r(plug,mode,type,def);
    internal_vars.insert(label,r);
}

void Db::registerCmd(const QString& from, const QStringList& list) {
    if(isRegisteredUser(from)) {
        QString txt("You are already registered!");
        manager()->connectionPlugin()->serverSend(from,txt);
        return;
    }

    if(getValue(from,"name").isNull()
       || getValue(from,"email").isNull()) {
        QString txt("You did not fill name and email!");
        manager()->connectionPlugin()->serverSend(from,txt);
        return;
    }

    registerUser(from,list[0]);

    storeDatas();

    QString txt1(from + " is now registered");
    manager()->connectionPlugin()->serverBroadcastOthers(from,txt1);

    QString txt2("You are registered");
    manager()->connectionPlugin()->serverSend(from,txt2);
}

void Db::addgroupCmd(const QString& from, const QStringList& list) {
    QString login = canonicalName(list[0]);
    QStringList groups = list[1].split(",",QString::SkipEmptyParts);

    if(!isRegisteredUser(login)) {
        QString txt("User " + login + " is not a registered user");
        manager()->connectionPlugin()->serverSend(from,txt);
        return;
        }
    QStringList ok,denied,na;
    for(QStringList::Iterator it = groups.begin(); it != groups.end(); ++it) {
        QString group = *it;
        if(existGroup(group) || (from == "Root")) {
            QString leader = (group.endsWith("*"))?group:(group + "*");
            if(inGroup(from,leader)) {
                ok << group;
                addToGroup(login,group);
            } else {
                denied << group;
            }
        } else {
            na << group;
        }
    }
    if(ok.count()) {
        QString txt1(from + " puts " + login + " in group(s) " + ok.join(" "));
        manager()->connectionPlugin()->serverBroadcastOthers(login,txt1);
        QString txt2(from + " puts you in group(s) " + ok.join(" "));
        manager()->connectionPlugin()->serverSend(login,txt2);
    }
    if(denied.count()) {
        QString txt("addgroup : permission denied for group(s) " + denied.join(" "));
        manager()->connectionPlugin()->serverSend(from,txt);
    }
    if(na.count()) {
        QString txt("Inexistant group(s) " + na.join(" "));
            manager()->connectionPlugin()->serverSend(from,txt);
    }
}

void Db::delgroupCmd(const QString& from, const QStringList& list) {
    QString login = canonicalName(list[0]);
    QStringList groups = list[1].split(",",QString::SkipEmptyParts);

    if(!isRegisteredUser(login)) {
        QString txt("User " + login + " is not a registered user");
        manager()->connectionPlugin()->serverSend(from,txt);
        return;
    }
    QStringList ok,denied,na;
    for(QStringList::Iterator it = groups.begin(); it != groups.end(); ++it) {
        QString group = *it;
        if(existGroup(group)) {
            QString leader = (group.endsWith("*"))?group:(group + "*");
            if((from == "Root") || (inGroup(from,leader) && !inGroup(login,leader))) {
                ok << group;
                removeFromGroup(login,group);
            } else {
                denied << group;
            }
        } else {
            na << group;
        }
    }
    if(ok.count()) {
        QString txt1(from + " kicks " + login + " from group(s) " + ok.join(" "));
        manager()->connectionPlugin()->serverBroadcastOthers(login,txt1);
        QString txt2(from + " kicks you from group(s) " + ok.join(" "));
        manager()->connectionPlugin()->serverSend(login,txt2);
    }
    if(denied.count()) {
        QString txt("delgroup : permission denied for group(s) " + denied.join(" "));
        manager()->connectionPlugin()->serverSend(from,txt);
    }
    if(na.count()) {
        QString txt("Inexistant group(s) " + na.join(" "));
        manager()->connectionPlugin()->serverSend(from,txt);
    }
}

void Db::defineCmd(const QString& from, const QStringList& list) {
    QString stype(list[0]);
    QString login(canonicalName(list[1]));
    QString prop(list[4]);
    int mode = (list[2] == "public")?0:PRIVATE;
    if(list[3] == "transient")
        mode = mode | TRANSIENT;
    int type = (stype == "string")?STRING:((stype == "int")?INT:BOOL);

    if(login == "" || login == from) { // owner of the property
        if(!isDefinedVariable(from,prop) || (!(getVariableMode(from,prop) & GLOBAL) && !(getVariableMode(from,prop) & INTERNAL))) {
            defineVariable(from,prop,mode,type);
            QString txt("You define " + prop + " as " + list[2] + " and " + list[3]);
            manager()->connectionPlugin()->serverSend(from,txt);
        } else {
            QString txt("define : permission denied");
            manager()->connectionPlugin()->serverSend(from,txt);
        }
    } else {
        if(!isRegisteredUser(login)) {
            QString txt("Unregistered user");
                manager()->connectionPlugin()->serverSend(from,txt);
        } else if(((from == "Root") || (inGroup(from,"Set") && !inGroup(login,"Set"))) &&
                  (!isDefinedVariable(login,prop) || (!(getVariableMode(login,prop) & GLOBAL) && !(getVariableMode(login,prop) & INTERNAL)))) {
            defineVariable(login,prop,mode,type);
            QString txt("You define " + login + "." + prop + " as " + list[2] + " and " + list[3]);
                manager()->connectionPlugin()->serverSend(from,txt);
        } else {
            QString txt("define : permission denied");
            manager()->connectionPlugin()->serverSend(from,txt);
        }
    }
}

void Db::undefCmd(const QString& from, const QStringList& list) {
    QString login(canonicalName(list[0]));
    QString prop(list[1]);
    if(login == "" || login == from) { // owner of the property
        int mode = getVariableMode(from,prop);
        if (!(mode & GLOBAL) && !(mode & INTERNAL)) {
            undefineVariable(from,prop);
            QString txt("You undef " + prop);
            manager()->connectionPlugin()->serverSend(from,txt);
        } else {
            QString txt("undef : permission denied");
            manager()->connectionPlugin()->serverSend(from,txt);
        }
    } else {
        if(!isRegisteredUser(login)) {
            QString txt("Unregistered user");
            manager()->connectionPlugin()->serverSend(from,txt);
        } else if(((from == "Root") || (inGroup(from,"Set") && !inGroup(login,"Set"))) &&
                      !(getVariableMode(login,prop) & GLOBAL) && !(getVariableMode(login,prop) & INTERNAL)) {
            undefineVariable(login,prop);
            QString txt("You undef " + login + "." + prop);
            manager()->connectionPlugin()->serverSend(from,txt);
        } else {
            QString txt("undef : permission denied");
            manager()->connectionPlugin()->serverSend(from,txt);
        }
    }
}

void Db::setCmd(const QString& from, const QStringList& list) {
    QString login(canonicalName(list[0]));
    QString prop(list[1]);
    QString value(list[2]);
    if (login == "" && prop == "" && value == "") { // list of properties
        manager()->connectionPlugin()->serverSend(from, "Your variables :");
        manager()->connectionPlugin()->send(from, getVariableList(from).join("\n"));
        manager()->connectionPlugin()->serverSend(from, "End of variables");
    }
    else if(login == "" || login == from) { // owner of the property
        if(isDefinedVariable(from,prop)) {
            int mode = getVariableMode(from,prop);
            if(!(mode & INFO) || from == "Root") {
                if(setValue(from,prop,value)) {
                    if(!((mode & INTERNAL) && (mode & PRIVATE))) {
                        QString txt("You set " + prop + " to \"" + value + "\"");
                        manager()->connectionPlugin()->serverSend(from,txt);
                    }
                    if((!(mode & PRIVATE)) && (!(mode & TRANSIENT)) && !(mode & INTERNAL)) {
                        QString disp(from + "'s " + prop + " is now \"" + value + "\"");
                        manager()->connectionPlugin()->serverBroadcastOthers(from,disp);
                    }
                } else {
                    QString txt("Incorrect value type");
                    manager()->connectionPlugin()->serverSend(from,txt);
                }
            } else {
                QString txt("set : permission denied");
                manager()->connectionPlugin()->serverSend(from,txt);
            }
        } else {
            QString txt("Unknown variable \"" + prop + "\"");
            manager()->connectionPlugin()->serverSend(from,txt);
        }
    } else {
        if(!isRegisteredUser(login)) {
            QString txt("Unregistered user");
            manager()->connectionPlugin()->serverSend(from,txt);
        } else if((from == "Root") || (inGroup(from,"Set") && !inGroup(login,"Set"))) {
            if(isDefinedVariable(login,prop)) {
                int mode = getVariableMode(login,prop);
                if((from == "Root") || (mode & PUBLIC)) {
                    if(setValue(login,prop,value)) {
                        QString txt("You set " + login + "." + prop + " to \"" + value + "\"");
                        manager()->connectionPlugin()->serverSend(from,txt);
                        if(!(mode & TRANSIENT) && !(mode & PRIVATE) && !(mode & INTERNAL)) {
                            QString disp(login + "'s " + prop + " is now \"" + value + "\"");
                            manager()->connectionPlugin()->broadcastOthers(from,disp);
                        }
                    } else {
                        QString txt("Incorrect value type");
                        manager()->connectionPlugin()->serverSend(from,txt);
                    }
                } else {
                    QString txt("set : permission denied");
                    manager()->connectionPlugin()->serverSend(from,txt);
                }
            } else {
                QString txt("Unknown variable \"" + prop + "\"");
                manager()->connectionPlugin()->serverSend(from,txt);
            }
        } else {
            QString txt("set : permission denied");
            manager()->connectionPlugin()->serverSend(from,txt);
        }
    }
}

void Db::getCmd(const QString& from, const QStringList& list) {
    QString login(canonicalName(list[0]));
    QString prop(list[1]);
    if (login == "" && prop == "") { // list of properties
        manager()->connectionPlugin()->serverSend(from, "Your variables :");
        manager()->connectionPlugin()->send(from, getVariableList(from).join("\n"));
        manager()->connectionPlugin()->serverSend(from, "End of variables");
    } else if(login == "" || login == from) { // owner of the property
        if(isDefinedVariable(from,prop)) {
            int mode = getVariableMode(from,prop);
            if(!(mode & ENCRYPTED)) {
                QString txt(prop + "'s value is \"" + getValue(from,prop) + "\"");
                manager()->connectionPlugin()->serverSend(from,txt);
            } else {
                QString txt("get : permission denied");
                manager()->connectionPlugin()->serverSend(from,txt);
            }
        } else {
            QString txt("Unknown variable \"" + prop + "\"");
            manager()->connectionPlugin()->serverSend(from,txt);
        }
    } else {
        if(!isRegisteredUser(login)) {
            QString txt("Unregistered user");
            manager()->connectionPlugin()->serverSend(from,txt);
        } else {
            if(isDefinedVariable(login,prop)) {
                int mode = getVariableMode(login,prop);
                if((from == "Root") || (!(mode & PRIVATE) && !(mode & ENCRYPTED))) {
                    QString txt(login + "." + prop + "'s value is \"" + getValue(login,prop) + "\"");
                    manager()->connectionPlugin()->serverSend(from,txt);
                } else {
                    QString txt("get : permission denied");
                    manager()->connectionPlugin()->serverSend(from,txt);
                }
            } else {
                QString txt("Unknown variable \"" + prop + "\"");
                manager()->connectionPlugin()->serverSend(from,txt);
            }
        }
    }
}

void Db::killCmd(const QString& from, const QStringList& list) {
    QString login(canonicalName(list[0]));
    if(manager()->databasePlugin()->isRegisteredUser(login)) {
        manager()->killed(login);
        QString txt1("You kill user " + login);
        manager()->connectionPlugin()->serverSend(from,txt1);
        QString txt2(from + " killed user " + login);
        manager()->connectionPlugin()->serverBroadcastOthers(from,txt2);
    } else {
        QString txt(login + " is not registered!");
        manager()->connectionPlugin()->serverSend(from,txt);
    }
}

void Db::renameCmd(const QString& from, const QStringList& list) {
    QString login(canonicalName(list[0]));
    if(manager()->databasePlugin()->isRegisteredUser(login)) {
        QDomElement child = DomUtil::elementByPath(m_document,"/users/" + login);
        child.setTagName(list[1]);
        QStringList l = DomUtil::readListEntry(m_document,"/userlist/","user");
        l.removeAll(login);
        l << list[1];
        DomUtil::writeListEntry(m_document,"/userlist/","user",l);
        manager()->renamed(login,list[1]);
        QString txt2(login + " is now known as " + list[1]);
        manager()->connectionPlugin()->serverBroadcast(txt2);
    } else {
        QString txt(login + " is not registered!");
        manager()->connectionPlugin()->serverSend(from,txt);
    }
}

void Db::chpasswdCmd(const QString& from, const QStringList& list) {
    setValue(from, "password", list[0]);
    QString txt("You changed your password");
    manager()->connectionPlugin()->serverSend(from,txt);
}

void Db::usersCmd(const QString& from, const QStringList&) {
    QString txt(" Login           Full Name                   EMail address\n"
                "-------- --------------------------- -------------------------------");
    QStringList l = DomUtil::readListEntry(m_document,"/userlist/","user");
    int count = 0;
    for(QStringList::Iterator it = l.begin(); it != l.end(); ++it,++count) {
        QString name = getValue(*it,"name");
        QString email = getValue(*it,"email");
        txt += QString("\n%1 %2 %3").arg(*it,-8).arg(name.left(27),-27).arg(email.left(31),-31);
    }
    manager()->connectionPlugin()->send(from, txt);

    txt = QString("There are %1 registered users").arg(count);
    manager()->connectionPlugin()->serverSend(from,txt);
}

void Db::fingerCmd(const QString& from, const QStringList& list) {
    QString login = canonicalName(list[0]);
    QString txt;
    if(login == "" || login == from) {
        QString txt = "login : " + from + "\n"
            + infos(from, PUBLIC | RESIDENT | INTERNAL | UNENCRYPTED)
            + "== Public\n"
            + infos(from, PUBLIC | EXTERNAL)
            + "== Info\n"
            + infos(from, INFO | UNENCRYPTED)
            + "== Intern\n"
            + infos(from, PUBLIC | INTERNAL | TRANSIENT | UNENCRYPTED)
            + "== Private\n"
            + infos(from, PRIVATE | UNENCRYPTED);
        txt = formatInfos(txt);
        manager()->connectionPlugin()->send(from, txt);
        manager()->connectionPlugin()->serverSend(from, "End of Finger");
    } else if(isRegisteredUser(login)) {
        QString txt = "login : " + login + "\n"
            + infos(login, PUBLIC | RESIDENT | INTERNAL | UNENCRYPTED)
            + "== Public\n"
            + infos(login, PUBLIC | EXTERNAL)
            + "== Info\n"
            + infos(login, INFO | UNENCRYPTED)
            + "== Intern\n"
            + infos(login, PUBLIC | INTERNAL | TRANSIENT | UNENCRYPTED) ;
        txt = formatInfos(txt);
        manager()->connectionPlugin()->send(from, txt);
        manager()->connectionPlugin()->serverSend(from, "End of Finger");
    } else {
        QString txt("User " + login + " does not exist!");
        manager()->connectionPlugin()->serverSend(from,txt);
    }
}

void Db::groupsCmd(const QString& from, const QStringList& list) {
    QString login = canonicalName(list[0]);
    QString txt;
    if(login == "") {
        QStringList l = DomUtil::readListEntry(m_document,"/users/" + from + "/groups","group");
        if(l.count()) {
            txt = "You are in groups ";
            txt += l.join(",");
        } else
            txt = "You are in no group";
    } else {
        QStringList l = DomUtil::readListEntry(m_document,"/users/" + login + "/groups","group");
        if(l.count()) {
            txt = login + " is in groups ";
            txt += l.join(",");
        } else
            txt = login + " is in no group";
    }
    manager()->connectionPlugin()->serverSend(from,txt);
}

void Db::groupListCmd(const QString& from, const QStringList&) {
    QStringList list = DomUtil::readListEntry(m_document,"/grouplist/","group");
    manager()->connectionPlugin()->serverSend(from, "Existing groups:");
    manager()->connectionPlugin()->send(from, list.join("\n"));
    manager()->connectionPlugin()->serverSend(from, "End of groups");
}

bool Db::isDefinedVariable(const QString& login, const QString& label) const {
    DomUtil::PairList list = DomUtil::readPairListEntry(m_document,"/users/" + login + "/variables", "variable", "name", "infos");
    DomUtil::PairList::Iterator it = list.find(label);

    return (it != list.end());
}

void Db::defineVariable(const QString& login, const QString& label, int mode, int type, const QString& def) {
    if(!isDefinedVariable(login,label)) {
        QString default_value(def);
        if(def.isNull())
            default_value = (type == STRING)?QString::null:((type == INT)?QString::number(0):"false");

        if(!(mode & TRANSIENT)) {
            QString path("/users/" + login + pathModifier(mode) + label);
            DomUtil::writeEntry(m_document,path,default_value);
        }

        DomUtil::PairList list = DomUtil::readPairListEntry(m_document,"/users/" + login + "/variables", "variable", "name", "infos");

        list[label] = QString::number(mode) + ":" + QString::number(type);
        DomUtil::writePairListEntry(m_document,"/users/" + login + "/variables", "variable", "name", "infos", list);
    }
}

void Db::undefineVariable(const QString& login, const QString& label) {
    DomUtil::PairList list = DomUtil::readPairListEntry(m_document,"/users/" + login + "/variables", "variable", "name", "infos");
    DomUtil::PairList::Iterator it = list.find(label);

    if(it != list.end()) {
        QStringList l = it.value().split(":",QString::SkipEmptyParts);
        QString path("/users/" + login + pathModifier(l[0].toInt()) + label);
        QDomElement elem = DomUtil::elementByPath(m_document,path);
        elem.parentNode().removeChild(elem);

        list.erase(it);
    }
}

bool Db::setValue(const QString& login, const QString& label, const QString& value, bool notify) {
    int mode = getVariableMode(login,label);
    int type = getVariableType(login,label);
    QString path("/users/" + login + pathModifier(mode) + label);

    QRegExp re;
    switch(type) {
        case STRING:
            re.setPattern(".*");
            break;
        case INT:
            re.setPattern("\\d+");
            break;
        case BOOL:
            re.setPattern("(on|true|1)|(off|false|0)");
            break;
        default:
            return false;
    }

    if(re.exactMatch(value)) {
        QString v(value);
        if(type == BOOL)
            v = (re.cap(1) == "")?"0":"1";
        DomUtil::writeEntry(m_document,path,(mode & ENCRYPTED)?QString(crypt(v.toAscii(),SEED)):v);
        if((mode & INTERNAL) && notify)
            ((*(internal_vars.find(label))).plug)->modifiedInternalVariable(login,label,v);
        return true;
    }
    return false;
}

QString Db::getValue(const QString& login, const QString& label) const {
    QString path("/users/" + login + pathModifier(getVariableMode(login,label)) + label);

    return DomUtil::readEntry(m_document,path,"");
}

int Db::getVariableMode(const QString& login, const QString& label) const {
    DomUtil::PairList list = DomUtil::readPairListEntry(m_document,"/users/" + login + "/variables", "variable", "name", "infos");
    DomUtil::PairList::Iterator it = list.find(label);
    QStringList l = it.value().split(":",QString::SkipEmptyParts);
    return (l[0].toInt());
}

int Db::getVariableType(const QString& login, const QString& label) const {
    DomUtil::PairList list = DomUtil::readPairListEntry(m_document,"/users/" + login + "/variables", "variable", "name", "infos");
    DomUtil::PairList::Iterator it = list.find(label);
    QStringList l = it.value().split(":",QString::SkipEmptyParts);
    return (l[1].toInt());
}

QString Db::pathModifier(int mode) const {
    QString path;
    if(mode & INFO)
        path += "/hidden";
    else if(mode & PRIVATE)
        path += "/private";
    else
        path += "/public";

    if(mode & INTERNAL)
        path += "/internal";
    else
        path += "/external";

    if(mode & TRANSIENT)
        path += "/transient";
    else
        path += "/resident";

    if(mode & GLOBAL)
        path += "/global";
    else
        path += "/local";

    if(mode & ENCRYPTED)
        path += "/encrypted/";
    else
        path += "/clear/";
    return path;
}

QString Db::infos(const QString& user, int mode) {
    QStringList txt;
    for(int i = 0; i<m_modes.count(); i++)
        if((m_modes[i] & mode) == mode) {
            QDomElement node = DomUtil::elementByPath(m_document,"/users/" + user + pathModifier(m_modes[i]));
            QDomNodeList children = node.childNodes();
            for(int i = 0; i < children.count(); i++) {
                QDomNode n = children.item(i);
                txt << n.nodeName() + " : " + n.firstChild().nodeValue();
            }
        }
    txt.sort();
    return (txt.join("\n") + "\n");

}

QString Db::formatInfos(const QString &infos) {
    QString txt;
    QStringList list;
    QRegExp re("([^\\:]+)\\:(.*)");
    int maxLength = 0;
    list = infos.split('\n',QString::SkipEmptyParts);
    // compute max length
    for (QStringList::Iterator it = list.begin(); it != list.end(); ++it)
        if (re.exactMatch(*it) && (re.cap(1).length() > maxLength))
            maxLength = re.cap(1).simplified().length();
    // make the text
    for (QStringList::Iterator it = list.begin(); it != list.end(); ++it) {
        if (txt != "")
            txt += "\n";
        if (re.exactMatch(*it))
            txt += QString("%1 : %2").arg(re.cap(1).simplified(), -maxLength).arg(re.cap(2).simplified());
        else
            txt += *it;
    }
    return txt;
}

QStringList Db::getVariableList(const QString &from) {
    QStringList name_list;
    QStringList value_list;
    int max = 0;

    // Construction
    for (int i = 0; i<m_modes.count(); i++) {
        if ((m_modes[i] & UNENCRYPTED) == UNENCRYPTED) {
            QDomElement node = DomUtil::elementByPath(m_document,"/users/" + from + pathModifier(m_modes[i]));
            QDomNodeList children = node.childNodes();
            for (int i = 0; i < children.count(); i++) {
                QDomNode n = children.item(i);
                name_list << n.nodeName();
                value_list << n.firstChild().nodeValue();
                if (n.nodeName().length() > max)
                    max = n.nodeName().length();
            }
        }
    }

    // Identation
    QString fill;
    for (int i = 0; i < name_list.count(); i++) {
        fill.fill(' ', max - name_list[i].length());
        name_list[i] += fill + " = " + value_list[i];
    }

    return name_list;
}
