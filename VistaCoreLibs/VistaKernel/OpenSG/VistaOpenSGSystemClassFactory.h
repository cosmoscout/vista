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

#ifndef _VISTAOPENSGSGSYSTEMCLASSFACTORY_H
#define _VISTAOPENSGSGSYSTEMCLASSFACTORY_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <string>
#include <time.h>

#include <VistaKernel/VistaKernelConfig.h>
#include <VistaKernel/VistaSystemClassFactory.h>

#include <VistaKernel/EventManager/VistaSystemEvent.h>

#ifdef WIN32
// disable warnings from OpenSG
#pragma warning(push)
#pragma warning(disable : 4127)
#pragma warning(disable : 4189)
#pragma warning(disable : 4231)
#pragma warning(disable : 4267)
#endif

#include <OpenSG/OSGConfig.h>
#include <OpenSG/OSGRenderAction.h>

#ifdef WIN32
// disable warnings from OpenSG
#pragma warning(pop)
#endif

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class VistaSystemEvent;
class VistaOpenSGDisplayBridge;
class VistaSystem;
class VistaEventManager;
class VistaDriverMap;
class VistaWeightedAverageTimer;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
class VISTAKERNELAPI VistaOpenSGSystemClassFactory : public IVistaSystemClassFactory {
 public:
  VistaOpenSGSystemClassFactory(VistaSystem* pVistaSystem, bool bUseOpenSGThreads);

  ~VistaOpenSGSystemClassFactory();

  virtual std::vector<IVistaSystemClassFactory::Manager> GetInitOrder() const;

  virtual VistaGraphicsManager* CreateGraphicsManager();

  virtual IVistaGraphicsBridge* CreateGraphicsBridge();

  virtual IVistaNodeBridge* CreateNodeBridge();

  virtual VistaDisplayManager* CreateDisplayManager();

  virtual VistaInteractionManager* CreateInteractionManager();

  void Debug(std::ostream& out, bool bVerbose = true) const;

 private:
  VistaSystem*       m_pVistaSystem;
  OSG::RenderAction* m_pRenderAction;

  long m_lFCStoreOffset;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTAOPENSGSYSTEMCLASSFACTORY_H
