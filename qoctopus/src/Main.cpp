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

////////////////////////////////////////////////////////////////////////////////
//
// Octopus main file
//
////////////////////////////////////////////////////////////////////////////////


#include <iostream>
#include <getopt.h>
#include <signal.h>
#include <unistd.h>

#include <qapplication.h>

#include "PluginManager.h"
#include "ConnectionPlugin.h"
#include "DatabasePlugin.h"
#include "DefaultPlugin.h"
#include "PrePostPlugin.h"

using namespace std;

PluginManager* pm;

int signal_caught = 0;

static const int Signal[] = {
    SIGABRT, SIGALRM, SIGCHLD, SIGFPE,  SIGHUP,  SIGILL,  SIGINT,  SIGPIPE,
    SIGQUIT, SIGSEGV, SIGTERM, SIGTSTP, SIGTTIN, SIGTTOU, SIGUSR1, SIGUSR2
};

void exitServer() {
    signal_caught++;
    pm->shutdownCmd("Root","shutdown");
}

static void SigHandler(int Sig) {
    switch (Sig) {

        case SIGABRT :
        case SIGFPE  :
        case SIGILL  :
        case SIGINT  :
        case SIGQUIT :
        case SIGTERM : exitServer();
            break;
    }
}

static void initSignal(void) {

    int I, SignalNb;
    struct sigaction SigAction[1];

    SignalNb = 0;
    for (I = 0; I < (int)(sizeof(Signal)/sizeof(Signal[0])); I++) {
        if (Signal[I] >= SignalNb) SignalNb = Signal[I] + 1;
    }
    SigAction->sa_handler = SigHandler;
    sigemptyset(&SigAction->sa_mask);
    SigAction->sa_flags = SA_RESTART;

    for (I = 0; I < (int)(sizeof(Signal)/sizeof(Signal[0])); I++) {
        sigaction(Signal[I],SigAction,NULL);
    }
}

int main(int argc, char *argv[]) {

    QApplication app(argc, argv,QApplication::Tty);

    pm = new PluginManager;
    bool daemonize = false;
    int c;

    while (1) {
        int option_index = 0;
        static struct option long_options[] = {
            {"port", 1, 0, 'p'},
            {"load", 1, 0, 'l'},
            {"connect", 1, 0, 'c'},
            {"default", 1, 0, 'x'},
            {"database", 1, 0, 'd'},
            {"prepost", 1, 0, 't'},
            {"log", 1, 0, 'g'},
            {"directory", 1, 0, 'r'},
            {"daemon", 0, 0, 'h'},
            {0, 0, 0, 0}
        };

        c = getopt_long (argc, argv, "p:l:c:x:d:r:t:g:h", long_options, &option_index);
        if (c == -1)
            break;
        switch (c) {
            case 'p':
                cerr << "Setting port to " << argv[optind-1] << endl;
                pm->addConnectionPort(atoi(argv[optind-1]));
                break;
            case 'l':
                cerr << "Loading " << argv[optind-1] << endl;
                pm->addPluginFile(argv[optind-1]);
                break;
            case 'c':
                cerr << "Loading " << argv[optind-1] << endl;
                pm->setConnectionPluginFile(argv[optind-1]);
                break;
            case 'x':
                cerr << "Loading " << argv[optind-1] << endl;
                pm->setDefaultPluginFile(argv[optind-1]);
                break;
            case 'd':
                cerr << "Loading " << argv[optind-1] << endl;
                pm->setDatabasePluginFile(argv[optind-1]);
                break;
            case 't':
                cerr << "Loading " << argv[optind-1] << endl;
                pm->setPrePostPluginFile(argv[optind-1]);
                break;
            case 'g':
                cerr << "Loading " << argv[optind-1] << endl;
                pm->setLogPluginFile(argv[optind-1]);
                break;
            case 'r':
                cerr << "Setting directory to " << argv[optind-1] << endl;
                pm->setDataDir(argv[optind-1]);
                break;
            case 'h':
                daemonize = true;
                break;
            case '?':
                break;
            default:
                cerr << "?? Unrecognised character ??" << endl;
        }
    }

    if(!pm->boot()) {
        if(!pm->connectionPlugin())
            cerr << "No connection plugin set!" << endl;
        if(!pm->defaultPlugin())
            cerr << "No default plugin set!" << endl;
        if(!pm->databasePlugin())
            cerr << "No database plugin set!" << endl;
        if(!pm->prepostPlugin())
            cerr << "No prepost plugin set!" << endl;
        if(!pm->logPlugin())
            cerr << "No log plugin set!" << endl;
        return EXIT_FAILURE;
    }

    initSignal();

    if(daemonize) {
        daemon(1,1);
    }
    return app.exec();
}
