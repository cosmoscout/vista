/*============================================================================*/
/*                              ViSTA VR toolkit                              */
/*               Copyright (c) 1997-2016 RWTH Aachen University               */
/*============================================================================*/
/*                                  License                                   */
/*                                                                            */
/*  This program is free software: you can redistribute it and/or modify      */
/*  it under the terms of the GNU Lesser General Public License as published  */
/*  by the Free Software Foundation, either version 3 of the License, or      */
/*  (at your option) any later version.                                       */
/*                                                                            */
/*  This program is distributed in the hope that it will be useful,           */
/*  but WITHOUT ANY WARRANTY; without even the implied warranty of            */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             */
/*  GNU Lesser General Public License for more details.                       */
/*                                                                            */
/*  You should have received a copy of the GNU Lesser General Public License  */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.     */
/*============================================================================*/
/*                                Contributors                                */
/*                                                                            */
/*============================================================================*/


/*

What is Bob doing?

In short: Bob receives an event from Alice and reacts accordingly.

A bit more in depth:
We have a bit more complex structure here than with Alice.

We have the CommandHandler that awaits a VistaEvent telling him that a message/event from Alice
came in. Depending on the message the CommandHandler tells Bob what to do.

I for my part find that Bob looks like a blockhead this way. For the matter of abstraction I
accept this. However one could think about Merging CommandHandler and Bob, making Bob
follow Alice directly. But maybe Alice and Bob only had a hissy fight ...

 */

/*============================================================================*/
/* INCLUDES & DEFINES                                                         */
/*============================================================================*/
#include <VistaKernel/VistaSystem.h>

#include "Bob.h"
#include "CommandHandler.h"
#include <VistaBase/VistaExceptionBase.h>

/*============================================================================*/
/* main()                                                                     */
/*============================================================================*/
int main(int argc, char **argv)
{
	// Initialize our Vista System
	VistaSystem *pVistaSystem = new VistaSystem;
	pVistaSystem->IntroMsg();

	std::list<std::string> liSearchPaths;
	liSearchPaths.push_back("../configfiles/");
	liSearchPaths.push_back("./configfiles/");
	pVistaSystem->SetIniSearchPaths(liSearchPaths); 

	Bob *pBob = 0;

	if(pVistaSystem->Init(argc, argv))
	{
		try
		{
			//Create Bob, Create Scene
			pBob = new Bob( pVistaSystem );
		}

		catch(VistaExceptionBase &eException)
		{
			//Something went terribly wrong. Better stop.
			eException.PrintException();
			return 0;
		}


		// Create the CommandHandler, link it with Bob
		CommandHandler oCommandHandler( pVistaSystem->GetEventManager(), pBob);
		// Get Bob going
		pVistaSystem->Run();
	}

	delete pBob;
	delete pVistaSystem;

	return 0;
}

