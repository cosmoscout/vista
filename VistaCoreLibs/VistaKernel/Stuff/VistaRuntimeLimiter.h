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

#ifndef _VISTARUNTIMELIMITER_H
#define _VISTARUNTIMELIMITER_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaBase/VistaBaseTypes.h>
#include <VistaBase/VistaTimer.h>

#include <VistaKernel/EventManager/VistaEventHandler.h>
#include <VistaKernel/VistaKernelConfig.h>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class VistaSystem;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * Small class that automatically terminates the application after
 * a certain time or number of frames. This can help for example
 * to automatically test if the application runs properly
 */
class VISTAKERNELAPI VistaRuntimeLimiter : public VistaEventHandler {
 public:
  /**
   * automatically registers itself als event manager for the system
   */
  VistaRuntimeLimiter(VistaSystem* pSystem);
  virtual ~VistaRuntimeLimiter();

  unsigned int GetFrameLimit() const;
  void         SetFrameLimit(const unsigned int& oValue);

  VistaType::microtime GetTimeLimit() const;
  void                 SetTimeLimit(const VistaType::microtime& oValue);

  virtual void HandleEvent(VistaEvent* pEvent);

 private:
  VistaSystem*         m_pSystem;
  unsigned int         m_nFrameLimit;
  VistaType::microtime m_nTimeLimit;
  VistaTimer           m_oTimer;
};

#endif //_VISTARUNTIMELIMITER_H
