//
//
// C++ Interface: $MODULE$
//
// Description:
//
//
// Author: Yann Hodique <hodique@lifl.fr>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef EXTENDEDPLUGIN_H
#define EXTENDEDPLUGIN_H

#include "PluginManagerInterface.h"

/**
   @author Yann Hodique
*/
class ExtendedPlugin : public PluginManagerInterface {
    Q_OBJECT
        
public:
    ExtendedPlugin(PluginManagerInterface* parent = 0) : PluginManagerInterface(parent,parent->dataDir()) {}

    virtual ~ExtendedPlugin() {}

    virtual ConnectionPlugin* connectionPlugin() const {
        return mng->connectionPlugin();
    }
    
    virtual DatabasePlugin* databasePlugin() const {
        return mng->databasePlugin();
    }
    
    virtual DefaultPlugin* defaultPlugin() const {
        return mng->defaultPlugin();
    }
    
    virtual LogPlugin* logPlugin() const {
        return mng->logPlugin();
    }
    
    virtual PrePostPlugin* prepostPlugin() const {
        return mng->prepostPlugin();
    }

};

#endif
