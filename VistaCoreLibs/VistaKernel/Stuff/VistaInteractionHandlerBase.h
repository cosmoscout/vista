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

#ifndef _VISTAINTERACTIONHANDLERBASE_H
#define _VISTAINTERACTIONHANDLERBASE_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaKernel/VistaKernelConfig.h>

#include <VistaKernel/EventManager/VistaEventHandler.h>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class VistaEventManager;
class VistaInteractionEvent;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTAKERNELAPI IVistaInteractionHandlerBase : public VistaEventHandler {
 public:
  virtual ~IVistaInteractionHandlerBase();

  void HandleEvent(VistaEvent* pEvent);

 protected:
  IVistaInteractionHandlerBase(VistaEventManager* pEvMgr, unsigned int nRoleId, bool bNeedsTime);

  virtual bool HandleContextChange(VistaInteractionEvent*) = 0;
  virtual bool HandleGraphUpdate(VistaInteractionEvent*)   = 0;

  virtual bool HandleTimeUpdate(double dTs, double dLastTs);

 private:
  VistaEventManager* m_pEvMgr;
  double             m_dTs;
  bool               m_bNeedsTime;
  unsigned int       m_nRoleId;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTAINTERACTIONHANDLERBASE_H
