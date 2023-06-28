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

#include "EventDemoAppl.h"
#include <VistaBase/VistaExceptionBase.h>

int main(int argc, char* argv[]) {
  try {
    // create a new instance of this demo here
    EventDemoAppl demoAppl(argc, argv);
    // start universe as an endless loop
    demoAppl.Run();
  } catch (VistaExceptionBase& eException) {
    eException.PrintException();
  } catch (std::exception& eException) {
    std::cerr << "Exception:" << eException.what() << std::endl;
  }
}
