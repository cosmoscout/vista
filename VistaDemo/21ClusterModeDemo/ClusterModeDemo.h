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

#ifndef _CLUSTERMODEDEMO_H
#define _CLUSTERMODEDEMO_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaKernel/EventManager/VistaEventHandler.h>

#include <vector>
/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class VistaSystem;
class IVistaTextEntity;
class VistaSimpleTextOverlay;
class ComputationThread;
class IVistaClusterBarrier;
class IVistaClusterDataSync;
class VistaExternalMsgEvent;
class VistaMsg;
/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class ClusterModeDemo : public VistaEventHandler {
 public:
  ClusterModeDemo(int argc = 0, char* argv[] = NULL);
  virtual ~ClusterModeDemo();

  void Run();

  virtual void HandleEvent(VistaEvent* pEvent);

 private:
  void AddTextToOverlay(const std::string& sText);
  void PerformSomeComputation();

  void SetupExternalMessageEvent();
  void CheckThreadedComputation();
  void HandleExternalMessageEvent(VistaEvent* pEvent);

  void ComputeSomething();

  VistaSystem*                   m_pVistaSystem;
  std::vector<IVistaTextEntity*> m_vecTexts;
  VistaSimpleTextOverlay*        m_pTextOverlay;

  IVistaClusterBarrier*  m_pBarrier;
  IVistaClusterDataSync* m_pDataSync;

  ComputationThread*     m_pComputeThread;
  VistaExternalMsgEvent* m_pComputeFinishedEvent;
  VistaMsg*              m_pMessage;
};

#endif // _CLUSTERMODEDEMO_H
