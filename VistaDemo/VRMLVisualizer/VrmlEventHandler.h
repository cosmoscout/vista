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

#ifndef _VRMLEVENTHANDLER_H
#define _VRMLEVENTHANDLER_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include <VistaKernel/EventManager/VistaEventHandler.h>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class VrmlVisualizer;
class VistaSystemEvent;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * CVrmlEventHandler
 * The CVrmlEventHandler is registered on the PREGRAPHICS event
 * and invokes the applications Update() method.
 */
class VrmlEventHandler : public VistaEventHandler {
 public:
  /**
   * Constructor
   * Give the Constructor an instance of the app.
   */
  VrmlEventHandler(VrmlVisualizer* app);

  /**
   * Destructor
   * Nothing to do here.
   */
  virtual ~VrmlEventHandler();

  /**
   * HandleEvent
   * This method is inherited from VistaEventHandler and is
   * called by the VistaSystem on an PREGRAPHICS event.
   */
  void HandleEvent(VistaEvent* pEvent);

 private:
  /**
   * Pointer to the instance of the app.
   */
  VrmlVisualizer* m_pApp;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

/*============================================================================*/
/* END OF FILE                                                                */
/*============================================================================*/
#endif // _VRMLEVENTHANDLER_H
