/*********************************************************************************
 *   Copyright (C) 2003 by Octopus Authors                                       *
 *   octopus-devel@nongnu.org                                                    *
 *                                                                               *
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

/**
 * @file   LogPlugin.h
 * @author Guillaume Denry
 * @date   Wed May 2 10:36:56 2003
 * 
 * @brief  Interface for a log plugin
 * 
 * 
 */

#ifndef _LOGPLUGIN_H_
#define _LOGPLUGIN_H_

#include "Plugin.h"

class LogPlugin : public Plugin {

    Q_OBJECT

public:
    /**
     * Constructor.
     * 
     * @param parent Associated plugin manager 
     * 
     */
    LogPlugin(PluginManagerInterface* parent=0) : Plugin(parent) {}

    /**
     * Destructor
     * 
     * 
     */
    virtual ~LogPlugin() {}

    /**
     * Write a log
     * 
     * @param section message section.
     * @param msg message to send.
     * 
     * @return true if the message was successfully sent.
     */
    virtual bool write(const QString & section, const QString & msg) = 0;    
};

#endif
