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

#ifndef _VISTASYSTEMCLASSFACTORY_H
#define _VISTASYSTEMCLASSFACTORY_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include <VistaKernel/VistaKernelConfig.h>

#include <ostream>
#include <string>
#include <vector>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class IVistaGraphicsBridge;
class IVistaNodeBridge;
class VistaDisplayManager;
class VistaInteractionManager;
class VistaGraphicsManager;
class VistaDriverMap;
class IVistaWindowingToolkit;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
class VISTAKERNELAPI IVistaSystemClassFactory {
 public:
  enum Manager { DISPLAY, GRAPHICS, INTERACTION };

  virtual ~IVistaSystemClassFactory() {
  }

  virtual std::vector<Manager> GetInitOrder() const = 0;

  virtual VistaGraphicsManager*    CreateGraphicsManager()    = 0;
  virtual IVistaGraphicsBridge*    CreateGraphicsBridge()     = 0;
  virtual IVistaNodeBridge*        CreateNodeBridge()         = 0;
  virtual VistaDisplayManager*     CreateDisplayManager()     = 0;
  virtual VistaInteractionManager* CreateInteractionManager() = 0;

  virtual void Debug(std::ostream& out, bool bVerbose = true) const = 0;

 protected:
  IVistaSystemClassFactory() {
  }
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTASYSTEMCLASSFACTORY_H
