
#ifndef PREPOST_H
#define PREPOST_H


#include "plugins.h"
#include <qstringlist.h>

/**
 *
 * Yann Hodique
 **/
class PrePost : public PrePostPlugin {
    Q_OBJECT
    PLUGIN_OBJECT(PrePost)
public:

    typedef QMap<QString,QString> AliasMap;
    typedef QMap<QString,AliasMap> Map;

    PrePost(PluginManagerInterface* parent);

    ~PrePost();

    void modifiedInternalVariable(const QString& login, const QString& name, const QString& value);

    QString treatIncomingMessage(const QString& from, const QString& msg);
    QString treatOutgoingMessage(const QString& to, const QString& msg);

    void exportCommands();
    void incomingUser(const QString& login);
    void outgoingUser(const QString& login);
    void killedUser(const QString& login);
    void renamedUser(const QString&, const QString&);

private:
    Map alias;

    void aliasCmd(const QString& from, const QStringList&);
    void unaliasCmd(const QString& from, const QStringList&);
    void realiasCmd(const QString& from, const QStringList&);

    void loadAlias(const QString& login);
    void storeAlias(const QString& login);
    void addAlias(const QString& login, const QString& com, const QString& al);
    void delAlias(const QString& login, const QString& com);

    QString expandAliases(const QString& login, const QString& msg);
#ifdef _REDIRECT_
    QStringList splitCommand(const QString& msg);
#endif

    QMap<QString,QStringList> command_queue;
    QStringList away_cache;
};

#endif
