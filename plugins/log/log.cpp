//
//
// C++ Implementation: log
//
// Description:
//
//
// Author: Guillaume Denry <denryg@wanadoo.fr>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include <iostream>

#include "log.h"

#include <QDateTime>
#include <QTextStream>

PLUGIN_FACTORY(Log);

#define MAX_LINE_NUMBER 100

Log::Log(PluginManagerInterface* parent) : LogPlugin(parent) {
    // Create log directory if doesn't exist
    QDir dir(logDir());
    if (!dir.exists()) {
        if (!dir.mkdir(dir.absolutePath())) {
            octInfo("Could not create the log directory.\n");
            return;
        }
    }
}

Log::~Log() {}

void Log::exportCommands() {}

bool Log::write(const QString & section, const QString & msg) {
    // Elaborate the new msg (with timestamp)
    QString message("[" + QDate::currentDate().toString("dd/MM/yyyy-") + QTime::currentTime().toString("hh:mm:ss") + "] " + msg);

    // Write it
    QFile fout(logDir() + "/" + section);

    if (!fout.open(QIODevice::WriteOnly | QIODevice::Append)) {
        octInfo(QString("Could not write the %1 file.\n").arg(section));
        return false;
    }

    QTextStream streamOut(&fout);

    streamOut << message + "\n";

    fout.close();

    return true;
}

QString Log::logDir() {
    return manager()->dataDir() + "/logs";
}
