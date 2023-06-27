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

#ifndef _VISTASTREAMMANAGEREXT_H
#define _VISTASTREAMMANAGEREXT_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include <VistaKernel/VistaKernelConfig.h>

#include <VistaBase/VistaStreamManager.h>
#include <VistaKernel/EventManager/VistaEventObserver.h>

#include <list>
#include <ostream>
#include <vector>
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

class VISTAKERNELAPI VistaKernelStreamInfoInterface : public VistaStreamManager::IInfoInterface {
 public:
  VistaKernelStreamInfoInterface(VistaSystem* pVistaSystem);
  virtual ~VistaKernelStreamInfoInterface();

  virtual VistaType::systemtime GetFrameClock() const;
  virtual std::string           GetNodeName() const;
  virtual unsigned int          GetFrameCount() const;
  virtual float                 GetFrameRate() const;

 private:
  VistaSystem* m_pVistaSystem;
};

class VISTAKERNELAPI VistaSystemEventLogger : public VistaEventObserver {
 public:
  VistaSystemEventLogger(VistaEventManager* pManager);
  virtual ~VistaSystemEventLogger();

  /**
   * Register for SystemEvents using the mask, with entries for SystemEvent VSE_XYZ
   * beeing 1 << VSE_XYZ (i.e. 2^VSE_XYZ), use ~0 to register for all events
   */
  void SetStreamEventMask(std::ostream& oStream, unsigned int iEventMask);
  void UnregisterStream(std::ostream& oStream);

  virtual void Notify(const VistaEvent* pEvent);

 protected:
  std::vector<std::list<std::ostream*>> m_vecLogStreams;
  VistaEventManager*                    m_pManager;
  bool                                  m_bRegistered;
};

#endif // _VISTASTREAMMANAGEREXT_H
