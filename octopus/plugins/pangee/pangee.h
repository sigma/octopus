//
//
// C++ Interface: h
//
// Description:
//
//
// Author: Yann Hodique <Yann.Hodique@lifl.fr>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef PANGEE_H
#define PANGEE_H

#include "plugins.h"

/**
 *@author Yann Hodique
 */
class Pangee : public Plugin {
    Q_OBJECT
    PLUGIN_OBJECT(Pangee)
public:
    Pangee(PluginManagerInterface* parent=0);

    ~Pangee();

    virtual void exportCommands();
    virtual void incomingUser(const QString& login);
    virtual void killedUser(const QString& login);
    virtual void outgoingUser(const QString& login);

};

#endif
