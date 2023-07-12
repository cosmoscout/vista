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

/*============================================================================*/
/*  MAKROS AND DEFINES                                                        */
/*============================================================================*/

#include "VistaPipeConstructionKit.h"
#include "VistaCheckError.h"
#include "VistaNullError.h"
#include <VistaInterProcComm/DataLaVista/Base/VistaPipeComponent.h>
#include <VistaInterProcComm/DataLaVista/Base/VistaRegistration.h>

#include <cstdio>
#include <list>

using namespace std;

#if defined(WIN32)
#pragma warning(disable : 4996)
#endif

/*============================================================================*/
/* STATICS                                                                    */
/*============================================================================*/

/*============================================================================*/
/*  CONSTRUCTORS / DESTRUCTOR                                                 */
/*============================================================================*/

DLVistaPipeConstructionKit::DLVistaPipeConstructionKit() {
}

DLVistaPipeConstructionKit::~DLVistaPipeConstructionKit() {
}

/*============================================================================*/
/*  IMPLEMENTATION                                                            */
/*============================================================================*/

DLVistaCheckError DLVistaPipeConstructionKit::Join(
    IDLVistaPipeComponent* pIn, IDLVistaPipeComponent* pOut) {
  bool bRet = pIn->AttachOutputComponent(pOut);
  bRet      = bRet && pOut->AttachInputComponent(pIn);

  if (bRet)
    return DLVistaNullError();
  else {
    char cBuffer[128];
    sprintf(cBuffer, "Attaching %lx with %lx returned false.", long(pIn), long(pOut));
    return DLVistaCheckError("DLVistaPipeConstructionKit::Join() -- ERROR", cBuffer);
  }
}

DLVistaCheckError DLVistaPipeConstructionKit::LinearJoin(
    const list<IDLVistaPipeComponent*>& liComponents) {
  list<IDLVistaPipeComponent*>::const_iterator it;
  list<IDLVistaPipeComponent*>::const_iterator iprev;
  for (it = liComponents.begin(); it != liComponents.end();) {
    iprev = it++;
    if (it != liComponents.end()) {
      DLVistaCheckError err = Join(*iprev, *it);
      if (!err.IsNullError())
        return err;
    } else
      break;
  }

  return DLVistaNullError(); // DLVistaCheckError("Not-Implemented-Error", "The feature
                             // \"ExtendedJoin\" is not implemented, yet.\n");
}
