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

#ifndef _VISTAEVENTHANDLER_H
#define _VISTAEVENTHANDLER_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
//#include <cassert>
#include <VistaKernel/VistaKernelConfig.h>

#include <string>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class VistaEvent;
class VistaEventManager;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
/**
 * VistaEventHandler - the mother of all event handlers ;-)
 */
class VISTAKERNELAPI VistaEventHandler {
 public:
  virtual ~VistaEventHandler();

  virtual void HandleEvent(VistaEvent* pEvent) = 0; // overwrite this to handle the given event

  virtual bool GetIsEnabled() const;
  virtual void SetIsEnabled(bool bEnabled);

  std::string GetHandlerToken() const;
  void        SetHandlerToken(const std::string& strToken);

 protected:
  VistaEventHandler();

 private:
  bool        m_bIsEnabled;
  std::string m_strHandlerToken;
};

/*============================================================================*/
/* INLINE FUNCTIONS                                                           */
/*============================================================================*/

#endif //_VISTAEVENTHANDLER_H
