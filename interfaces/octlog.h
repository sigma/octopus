/*
 *  File: octlog.h
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

#ifndef _OCTLOG_H_
#define _OCTLOG_H_

#include <string>
#include <list>
#include <ostream>
#include <sstream>

#include <QString>

namespace octopus {

    class Octstream : public std::ostringstream {

    public:
        Octstream &operator << (const QString& msg) {
            return (Octstream&)(*((std::ostringstream*)this) << std::string(msg.ascii()));
        }

        Octstream &operator << (const char* msg) {
            return (Octstream&)(*((std::ostringstream*)this) << std::string(msg));
        }

        Octstream &operator << (int i) {
            return (Octstream&)(*((std::ostringstream*)this) << i);
        }

    };

    class Octlog {
    public:
        //! get (and create if needed) the octlog singleton
        static Octlog &getInstance();

        //! destroy the octlog singleton
        static void destroy();

        //! constructor
        Octlog();

        //! destructor
        ~Octlog();

        //! Add a new stream into the octlog. If deleteIt is true, the stream will be deleted in the octlog destructor
        void addStream(std::ostream *stream, bool deleteIt = false);

        //! Remove a stream from the octlog
        void removeStream(std::ostream *stream);

        //! Display a string to all attached streams
        Octlog &operator << (const std::string &message);

    protected:
        typedef std::list<std::pair<std::ostream *, bool> > StreamList;
        StreamList streams;
        static Octlog *singleton;
    };

} // octopus

#endif
