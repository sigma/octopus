/*
 *  File: octlog.cpp
 *  Created: Wednesday, April 28, 2004
 *  Copyright: blind assist authors
 *  Email:
 */

/************************************************************************
 *                                                                      *
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 *                                                                      *
 ************************************************************************/

#include <algorithm>

#include "octlog.h"

namespace octopus {

    Octlog *Octlog::singleton = NULL;

    Octlog &Octlog::getInstance() {
        if (singleton == NULL)
            singleton = new Octlog();
        return *singleton;
    }

    void Octlog::destroy() {
        if (singleton != NULL) {
            delete singleton;
            singleton = NULL;
        }
    }

    Octlog::Octlog() {}

    Octlog::~Octlog() {
        for (StreamList::iterator it = streams.begin(); it != streams.end(); ++it)
            if (it->second)
                delete it->first;
    }

    void Octlog::addStream(std::ostream *stream, bool deleteIt) {
        if (stream != NULL) {
            streams.remove(std::make_pair(stream, deleteIt));
            streams.push_back(std::make_pair(stream, deleteIt));
        }
    }

    void Octlog::removeStream(std::ostream *stream) {
        if (stream != NULL)
            for (StreamList::iterator it = streams.begin(); it != streams.end(); )
                if (it->first == stream)
                    it = streams.erase(it);
                else
                    ++it;
    }

    Octlog &Octlog::operator << (const std::string &message) {
        for (StreamList::iterator it = streams.begin(); it != streams.end(); ++it) {
            *(it->first) << message;
        }
        return *this;
    }
} // octopus
