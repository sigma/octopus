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


#include <string>
#include <iostream>
#include <stdlib.h>
#include <getopt.h>

#include "PluginManager.h"

using namespace std;

int main(int argc, char *argv[]) {

    int port = 3000;

    PluginManager pm;

    int c;
    while (1) {
        int option_index = 0;
        static struct option long_options[] = {
                                                  {"port", 1, 0, 'p'
                                                  },
                                                  {"load", 1, 0, 'l'},
                                                  {"connect", 1, 0, 'c'},
                                                  {0, 0, 0, 0}
                                              };

        c = getopt_long (argc, argv, "p:l:c:", long_options, &option_index);
        if (c == -1)
            break;
        switch (c) {
        case 'p':
            cerr << "Setting port to " << argv[optind-1] << endl;
            port = atoi(argv[optind-1]);
            break;
        case 'l':
            cerr << "Loading " << argv[optind-1] << endl;
            pm.loadPluginFile(argv[optind-1]);
	    break;
        case 'c':
            cerr << "Loading " << argv[optind-1] << endl;
            pm.loadConnectionPluginFile(argv[optind-1]);
	    break;
        case '?':
            break;
        default:
            cerr << "?? Unrecognised character ??" << endl;
        }
    }

    if(pm.connectionPlugin()) {
	pm.connectionPlugin()->launch(port);
        cin >> c;
    }
    else {
	cerr << "No connection plugin set !" << endl;
	return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
