/*********************************************************************************
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

#ifndef _TELL_H_
#define _TELL_H_

#include "plugins.h"

class Tell : public Plugin {
    Q_OBJECT
    PLUGIN_OBJECT(Tell)
public:
    Tell(PluginManagerInterface* parent=0) : Plugin(parent) {}

    ~Tell() {}

    void tellCmd(const QString&, const QStringList&);
    void senddataCmd(const QString&, const QStringList&);

    void exportCommands();

};

#endif
