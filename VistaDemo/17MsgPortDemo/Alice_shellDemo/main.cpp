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
Okay. What does Alice do?

Basically Alice creates a MessageChannel to Bob.
Alice will receive InteractionCommands from the User (Press arrow keys and/or wasd).
These commands will then be sent (as event) to Bob via the MessageChannel.
Bob does something with these commands and sends a Message back to Alice.

*/

/*============================================================================*/
/* INCLUDES & DEFINES                                                         */
/*============================================================================*/
//This is the header file where we define the Window that is to be created.
#include "Alice.h"
#include <VistaKernel/VistaSystem.h>

/*============================================================================*/
/* main()                                                                     */
/*============================================================================*/
int main(int argc , char **argv)
{
	VistaSystem *pVistaSystem = new VistaSystem();
	pVistaSystem->IntroMsg();

	std::list<std::string> liSearchPaths;
	liSearchPaths.push_back("../configfiles/");
	liSearchPaths.push_back("./configfiles/");
	pVistaSystem->SetIniSearchPaths(liSearchPaths); 

	if(pVistaSystem->Init(argc, argv))
	{
		Alice* pAlice = new Alice(pVistaSystem->GetKeyboardSystemControl(),pVistaSystem->GetIniFile());
		
		pVistaSystem->Run();

		delete pAlice;
	}

	delete pVistaSystem;
  
	return 0;
};

