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

#ifndef _CPROCESS_CALLBACK_H
#define _CPROCESS_CALLBACK_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include <VistaAspects/VistaExplicitCallbackInterface.h>

class VistaGraphicsEvent;
class VistaEventManager;
class VistaEvent;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class ProcessCallback : public IVistaExplicitCallbackInterface {
 public:
  static const int NULL_EVENT     = 0;
  static const int GRAPHICS_EVENT = 1;
  static const int DEMO_EVENT     = 2;

  ProcessCallback(int iEventType, VistaEventManager* pEventManager);
  ~ProcessCallback();

  // Interface
  bool PrepareCallback();
  bool Do();
  bool PostCallback();

 private:
  VistaEventManager* m_pEventManager;
  int                m_iEventType;
};

#endif
