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

#ifndef _VISTAFRAMELOOP_H
#define _VISTAFRAMELOOP_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaKernel/VistaKernelConfig.h>

#include <VistaBase/VistaBaseTypes.h>

#include <list>
#include <ostream>
#include <string>
#include <vector>
/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class VistaSystem;
class VistaDisplayManager;
class VistaEventManager;
class VistaSystemEvent;
class VistaClusterMode;
class VistaWeightedAverageTimer;
class VistaWindowAverageTimer;
class Vista2DText;
/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTAKERNELAPI VistaFrameLoop {
 public:
  VistaFrameLoop();
  virtual ~VistaFrameLoop();

  virtual bool Init(VistaSystem* pVistaSystem);

  virtual void Run();
  virtual void Quit();
  virtual void FrameUpdate();

  float                GetFrameRate();
  int                  GetFrameCount();
  VistaType::microtime GetAverageLoopTime();

 protected:
  void EmitSystemEvent(const int iSystemEventId);

 protected:
  class DisplayUpdateCallback;
  DisplayUpdateCallback* m_pUpdateCallback;
  VistaDisplayManager*   m_pDisplayManager;
  VistaEventManager*     m_pEventManager;
  VistaSystemEvent*      m_pSystemEvent;
  VistaClusterMode*      m_pClusterMode;

  VistaWeightedAverageTimer* m_pAvgLoopTime;
  VistaWeightedAverageTimer* m_pFrameRate;
  int                        m_iFrameCount;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTAFRAMELOOP_H
