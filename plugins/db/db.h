
#ifndef DB_H
#define DB_H

#include <qdom.h>
#include <qstringlist.h>

#include "plugins.h"

/**
 *
 * Yann Hodique
 **/

class Record {
public:
    Record(Plugin* p = 0, int m = 0, int t = 0, const QString&d = QString::null) :
      plug(p), mode(m), type(t), def(d) {}

    Plugin* plug;
    int mode;
    int type;
    QString def;
};

class Db : public DatabasePlugin {
    Q_OBJECT
    PLUGIN_OBJECT(Db)
public:

    Db(PluginManagerInterface* parent = 0);

    ~Db();

    void incomingUser(const QString& /*login*/);
    void outgoingUser(const QString& /*login*/);
    void killedUser(const QString& /*login*/);

    bool isValidLogin(const QString& login) const;
    int validLoginLength() const;

    bool isRegisteredUser(const QString&) const;
    bool authUser(const QString& login, const QString& pass) const;
    bool existGroup(const QString& group) const;
    bool inGroup(const QString& login, const QString& group);

    void exportCommands();

    const QString& canonicalName(const QString& login) const;

    void registerInternalVariable(Plugin*,const QString&,int mode = DEFAULT_MODE, int type = STRING, const QString& def = QString::null);

    typedef QMap< QString,Record > VarMap;

private:
    QDomDocument m_document;

    bool loadDatas();
    bool storeDatas();

    void registerUser(const QString& login, const QString& pass);
    void createUser(const QString& login);
    void deleteUser(const QString& login);
    void createGroup(const QString& group);
    void deleteGroup(const QString& group);
    void addToGroup(const QString& login, const QString& group);
    void removeFromGroup(const QString& login, const QString& group);

    void registerCmd(const QString&, const QStringList&);
    void addgroupCmd(const QString&, const QStringList&);
    void delgroupCmd(const QString&, const QStringList&);
    void setCmd(const QString&, const QStringList&);
    void defineCmd(const QString&, const QStringList&);
    void undefCmd(const QString&, const QStringList&);
    void getCmd(const QString&, const QStringList&);
    void killCmd(const QString&, const QStringList&);
    void renameCmd(const QString&, const QStringList&);
    void chpasswdCmd(const QString&, const QStringList&);
    void usersCmd(const QString&, const QStringList&);
    void fingerCmd(const QString&, const QStringList&);
    void groupsCmd(const QString&, const QStringList&);
    void groupListCmd(const QString&, const QStringList&);


    bool isDefinedVariable(const QString& login, const QString& label) const;
    void defineVariable(const QString& login, const QString& label, int mode = DEFAULT_MODE, int type = STRING, const QString& def = QString::null);
    void undefineVariable(const QString& login, const QString& label);
    bool setValue(const QString& login, const QString& label, const QString& value, bool notify = true);
    QString getValue(const QString& login, const QString& label) const;
    int getVariableMode(const QString& login, const QString& label) const;
    int getVariableType(const QString& login, const QString& label) const;

    QString pathModifier(int mode) const;
    QString infos(const QString& user, int mode = DEFAULT_MODE);
    QString formatInfos(const QString &infos);
    QStringList getVariableList(const QString &from);

    QList<int> m_modes;
    QStringList guests;
    VarMap internal_vars;
};

#endif
