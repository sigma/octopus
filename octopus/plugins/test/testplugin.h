/***************************************************************************
 *   Copyright (C) 2003 by Yann Hodique                                    *
 *   Yann.Hodique@lifl.fr                                                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef TESTPLUGIN_H
#define TESTPLUGIN_H


#include "Plugin.h"

class TestPlugin : public Plugin
{

public:
  TestPlugin();
  ~TestPlugin();
  
  void plop(const string&);
  void callCommand(int id, string& msg);

protected:
  void exportCommands();
};

#endif
