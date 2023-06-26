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

#ifndef _DECREASECALLBACK_H
#define _DECREASECALLBACK_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include <VistaAspects/VistaExplicitCallbackInterface.h>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class VrmlVisualizer;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * CDecreaseCallback
 * This Callback decreases the animation speed if the animation is running.
 * If the animation is not running the previous step is shown.
 */
class DecreaseCallback : public IVistaExplicitCallbackInterface {
 public:
  /**
   * Constructor
   * This one needs the instance of the CVrmlVisualizer
   */
  DecreaseCallback(VrmlVisualizer* pVrmlVisualizer);

  /**
   * Destructor
   * Nothing to here, because the m_pVrmlVisualizer is hold by the main.cpp
   */
  virtual ~DecreaseCallback();

  /**
   * Do
   * The Do method is invoced by the application.
   */
  bool Do();

 private:
  /**
   * Instance of CVrmlVisualizer
   */
  VrmlVisualizer* m_pVrmlVisualizer;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

/*============================================================================*/
/* END OF FILE                                                                */
/*============================================================================*/
#endif // _DECREASECALLBACK_H
