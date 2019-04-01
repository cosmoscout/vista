/******************************************************************
*                                                                 *
*       File : spacemouseserver.cpp                               *
*                                                                 *
*                                                                 *
*       Contact : Ingo Assenmacher (ingo.assenmacher@imag.fr)     *
*                                                                 *
******************************************************************/


#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <signal.h>

#include "vrpn_Connection.h"
#include "vrpn_3DConnexion.h"

#include <iostream>


bool bDone = false;

void siginthandler( int sig )
{
	bDone = true;
}

int main (int argc, char **argv)
{

    signal( SIGINT, siginthandler );

    vrpn_Connection *connection = vrpn_create_server_connection(vrpn_DEFAULT_LISTEN_PORT_NO);
    vrpn_3DConnexion_Navigator *Navigator;

	//Open a 3Space tracker
	if ( (Navigator =
	     new vrpn_3DConnexion_Navigator("SpaceNav0", connection)) == NULL)
	{
	  fprintf(stderr,"Can't create new vrpn_3DConnexion_Navigator\n");
	  return -1;
	}
	else
	{
		std::cout << "Opened \"SpaceNav0\"" << std::endl;
	}

	// Loop forever calling the mainloop()s for all devices and the connection
	while (!bDone) {
		// Let all the buttons generate reports
		Navigator->mainloop();
		// Send and receive all messages
		connection->mainloop();
	}

	printf("leaving server... bye.\n");

	delete Navigator;
	delete connection;
}
