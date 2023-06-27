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

#if defined(VISTA_SYS_OPENSG)

#include "VistaOpenSGSystemClassFactory.h"

// Vista includes
#include <VistaKernel/Cluster/VistaClusterMode.h>
#include <VistaKernel/VistaSystem.h>

#include <VistaKernel/OpenSG/VistaOpenSGDisplayBridge.h>
#include <VistaKernel/OpenSG/VistaOpenSGGraphicsBridge.h>
#include <VistaKernel/OpenSG/VistaOpenSGNodeBridge.h>
#include <VistaKernel/OpenSG/VistaOpenSGThreadImp.h>

#include <VistaKernel/EventManager/VistaSystemEvent.h>

#include <VistaKernel/Stuff/VistaKernelProfiling.h>

#include <VistaKernel/DisplayManager/VistaDisplayEntity.h>
#include <VistaKernel/DisplayManager/VistaDisplayManager.h>
#include <VistaKernel/DisplayManager/VistaWindow.h>
#include <VistaKernel/GraphicsManager/VistaTransformNode.h>
#include <VistaKernel/InteractionManager/VistaInteractionManager.h>

#include <VistaBase/VistaExceptionBase.h>
#include <VistaBase/VistaStreamUtils.h>
#include <VistaBase/VistaTimer.h>
#include <VistaBase/VistaVectorMath.h>

#ifdef WIN32
// disable warnings from OpenSG
#pragma warning(push)
#pragma warning(disable : 4127)
#pragma warning(disable : 4189)
#pragma warning(disable : 4231)
#pragma warning(disable : 4267)
#endif

#include <OpenSG/OSGBaseFunctions.h>
// "name" attachments
#include <OpenSG/OSGConfig.h>
#include <OpenSG/OSGSimpleAttachments.h>

#ifdef WIN32
// disable warnings from OpenSG
#pragma warning(pop)
#endif

#include "VistaOpenSGImageAndTextureFactory.h"
#include <iostream>
using namespace std;

OSG_USING_NAMESPACE

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaOpenSGSystemClassFactory::VistaOpenSGSystemClassFactory(
    VistaSystem* pVistaSystem, bool bUseOpenSGThreads)
    : IVistaSystemClassFactory()
    , m_pVistaSystem(pVistaSystem) {
  osg::osgInit(0, NULL);
  // get size of FieldContainerStore right after init (prototypes are initialized now)
  m_lFCStoreOffset = (int)osg::FieldContainerFactory::the()->getFieldContainerStore()->size();

  if (bUseOpenSGThreads) {
    // register thread creation factory
    IVistaThreadImp::RegisterThreadImpFactory(new VistaOSGThreadImp::VistaOSGThreadImpFactory);
  }

  m_pRenderAction = osg::RenderAction::create();
  m_pRenderAction->setFrustumCulling(false);
  m_pRenderAction->setUseGLFinish(false);
  m_pRenderAction->setOcclusionCullingMode(osg::RenderAction::OcclusionHierarchicalMultiFrame);
  m_pRenderAction->setOcclusionCulling(false);
  m_pRenderAction->setVolumeDrawing(false);

  IVistaImageAndTextureCoreFactory::SetSingleton(
      new VistaOpenSGImageAndTextureCoreFactory(m_pRenderAction));
}

void VistaOpenSGSystemClassFactory::Debug(std::ostream& out, bool bVerbose) const {
  const std::vector<FieldContainerPtr>& fcs =
      *FieldContainerFactory::the()->getFieldContainerStore();
  int notNull       = 0;
  int suspectsCount = 0;
  for (int i = 0; i < (int)fcs.size(); ++i) {
    FieldContainerPtr fc = fcs[i];
    if (fc != NullFC) {
      ++notNull;
      if (fc.getRefCount() <= 0)
        ++suspectsCount;
      if (!bVerbose)
        continue;

      AttachmentContainerPtr ac = AttachmentContainerPtr::dcast(fc);
      if (ac == NullFC) {
        AttachmentPtr a = AttachmentPtr::dcast(fc);
        if (a != NullFC) {
          FieldContainerPtr parent = NullFC;
          if (a->getParents().size() > 0) {
            /** @todo make this compile again with OpenSG 1.8.0 and higher */
            parent = a->getParents().getValue(0);
          }
          ac = AttachmentContainerPtr::dcast(parent);
        }
      }
      const osg::Char8* name = osg::getName(ac);

      if (name != NULL) {
        out << ((fc.getRefCount() <= 0) ? "[*SUSPECT*] " : "")
            << ((i < m_lFCStoreOffset) ? "[prototype] " : "") << "Detected living FC "
            << fc->getTypeName() << " (" << name << ") " << fc.getCPtr() << " refcount "
            << fc.getRefCount() << "\n";
      } else {
        out << ((fc.getRefCount() <= 0) ? "[*SUSPECT*] " : "")
            << ((i < m_lFCStoreOffset) ? "[prototype] " : "") << "Detected living FC "
            << fc->getTypeName() << " refcount " << fc.getRefCount() << "\n";
      }
    }
  }

  out << notNull << " of " << fcs.size() << " (" << m_lFCStoreOffset
      << " prototypes) FCs are alive, " << suspectsCount << " are suspect (refcount 0)"
      << std::endl;
}

VistaOpenSGSystemClassFactory::~VistaOpenSGSystemClassFactory() {
  IVistaThreadImp::DeleteThreadImpFactory();

  delete m_pRenderAction;

  osg::Thread::getCurrentChangeList()->clearAll();

#ifdef DEBUG
//	Debug(std::cerr);
#endif

  osg::osgExit();
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

std::vector<IVistaSystemClassFactory::Manager> VistaOpenSGSystemClassFactory::GetInitOrder() const {
  std::vector<IVistaSystemClassFactory::Manager> vOrder;
  vOrder.clear();
  vOrder.push_back(IVistaSystemClassFactory::GRAPHICS);
  vOrder.push_back(IVistaSystemClassFactory::DISPLAY);
  vOrder.push_back(IVistaSystemClassFactory::INTERACTION);
  return vOrder;
}

IVistaGraphicsBridge* VistaOpenSGSystemClassFactory::CreateGraphicsBridge() {
  return new VistaOpenSGGraphicsBridge(m_pRenderAction, this);
}

IVistaNodeBridge* VistaOpenSGSystemClassFactory::CreateNodeBridge() {
  return new VistaOpenSGNodeBridge;
}

VistaGraphicsManager* VistaOpenSGSystemClassFactory::CreateGraphicsManager() {
  return new VistaGraphicsManager(
      m_pVistaSystem->GetEventManager(), m_pVistaSystem->GetFrameLoop());
}

VistaDisplayManager* VistaOpenSGSystemClassFactory::CreateDisplayManager() {
  // note: init order! gm should be there!
  VistaGraphicsManager* pGraphisManager = m_pVistaSystem->GetGraphicsManager();

  VistaSceneGraph* pSG       = pGraphisManager->GetSceneGraph();
  VistaGroupNode*  pRealRoot = pSG->GetRealRoot();
  osg::NodePtr     pOSGRoot = (dynamic_cast<VistaOpenSGNodeData*>(pRealRoot->GetData()))->GetNode();

  VistaOpenSGDisplayBridge* pDisplayBridge =
      new VistaOpenSGDisplayBridge(m_pRenderAction, pOSGRoot);

  VistaDisplayManager* pDisplayManager = new VistaDisplayManager(pDisplayBridge);

  // cross link bridge with manager
  pDisplayBridge->SetDisplayManager(pDisplayManager);

  return pDisplayManager;
}

/*============================================================================*/
/*                                                                            */
/*  NAME      :   Update                                                      */
/*                                                                            */
/*============================================================================*/

VistaInteractionManager* VistaOpenSGSystemClassFactory::CreateInteractionManager() {
  VistaInteractionManager* pRet = new VistaInteractionManager(m_pVistaSystem->GetEventManager());

  return pRet;
}

#endif // VISTA_SYS_OPENSG
