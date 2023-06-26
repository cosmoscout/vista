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

#ifndef _TOGGLESHADOWCALLBACK_H
#define _TOGGLESHADOWCALLBACK_H

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

class ShadowDemo;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * CResetCallback
 * This Callback let the application parse the parameters again.
 */
class ToggleShadowCallback : public IVistaExplicitCallbackInterface {
 public:
  /**
   * Constructor
   * The CResetCallback needs an instance of ShadowDemo
   */
  ToggleShadowCallback(ShadowDemo* appl);

  /**
   * Destructor
   * Nothing todo here yet
   */
  virtual ~ToggleShadowCallback();

  /**
   * Do
   * Invoked by the application
   */
  bool Do();

 private:
  /**
   * The pointer to the instance of the application.
   */
  ShadowDemo* m_pShadowDemo;
};

#endif // _RESETCALLBACK_H
