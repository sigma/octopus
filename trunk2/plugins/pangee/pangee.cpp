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
#include "pangee.h"

PLUGIN_FACTORY(Pangee);

Pangee::Pangee(PluginManagerInterface* parent)
        : Plugin(parent) {}


Pangee::~Pangee() {}


void Pangee::exportCommands() {
}

void Pangee::incomingUser(const QString& /*login*/) {
}

void Pangee::killedUser(const QString& /*login*/) {
}

void Pangee::outgoingUser(const QString& /*login*/) {
}
