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

#ifndef _PREPOSTPLUGIN_H_
#define _PREPOSTPLUGIN_H_

#include "Plugin.h"

class PrePostPlugin : public Plugin {
    
    Q_OBJECT
    
public:
    /** 
     * Constructor.
     * 
     * @param parent Corresponding plugin manager.
     * 
     */
    PrePostPlugin(PluginManagerInterface* parent=0) : Plugin(parent) {}

    /** 
     * Destructor.
     * 
     */
    virtual ~PrePostPlugin() {}

    /** 
     * Preprocessing.
     * 
     * @param from Author of the message.
     * @param msg The original message.
     * 
     * @return Modified message.
     */    
    virtual QString treatIncomingMessage(const QString& from, const QString& msg) = 0;

    /** 
     * Postprocessing.
     * 
     * @param to Receiver of the message.
     * @param msg The original message.
     * 
     * @return Modified message.
     */    
    virtual QString treatOutgoingMessage(const QString& to, const QString& msg) = 0;
};

#endif
