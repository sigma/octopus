/***************************************************************************
 *   Copyright (C) 2003 by Yann Hodique                                    *
 *   Yann.Hodique@lifl.fr                                                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include <iostream>

#include "testplugin.h"

using namespace std;

PLUGIN_FACTORY(TestPlugin)

TestPlugin::TestPlugin() : Plugin() {}


TestPlugin::~TestPlugin() {}

void TestPlugin::plop(const string& msg) {
    cout << msg << endl;
}

void TestPlugin::exportCommands() {
    registerCommand("plopi",0);
}

void TestPlugin::callCommand(int id, string& msg) {
    switch(id) {
    case 0:
        plop(msg);
        break;
    default:
        cerr << "Unknown command" << endl;
    }
}
