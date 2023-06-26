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

#ifndef _VISTAEVENTOBSERVER_H
#define _VISTAEVENTOBSERVER_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaKernel/VistaKernelConfig.h>

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
 * VistaEventObserver - the mother of all observers ;-)
 */
class VISTAKERNELAPI VistaEventObserver {
  friend class VistaEventManager;

 private:
  bool m_bIsDef;

 public:
  VistaEventObserver() {
    m_bIsDef = false;
  }

  virtual ~VistaEventObserver() {
  }
  /**
   * Notify the observer of the given event.
   *
   * @param   VistaEvent * pEvent    (MUST NOT BE NULL!)
   * @RETURN  --
   */
  virtual void Notify(const VistaEvent* pEvent) = 0;
};

/*============================================================================*/
/* INLINE FUNCTIONS                                                           */
/*============================================================================*/
#endif // _VISTAEVENTOBSERVER_H
