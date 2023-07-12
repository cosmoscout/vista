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

#ifndef _VISTAKERNELDFNNODECREATORSS_H
#define _VISTAKERNELDFNNODECREATORSS_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include <VistaKernel/VistaKernelConfig.h>

#include <VistaKernel/InteractionManager/DfnNodes/VistaDfnTextOverlayNode.h>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class VistaSystem;
class VistaDisplayManager;
class VistaEventManager;
class VistaInteractionManager;
class VistaClusterMode;
class VistaKeyboardSystemControl;
class VistaGraphicsManager;
class VistaSceneGraph;
/*============================================================================*/
/* UTILITY                                                                    */
/*============================================================================*/

namespace VistaKernelDfnNodeCreators {
bool VISTAKERNELAPI RegisterNodeCreates(VistaSystem* pVistaSystem);
}

/*============================================================================*/
/* NODE CREATES                                                               */
/*============================================================================*/

class VISTAKERNELAPI Vista3DMouseTransformNodeCreate : public VdfnNodeFactory::IVdfnNodeCreator {
 public:
  Vista3DMouseTransformNodeCreate(VistaDisplayManager* pSys);

  virtual IVdfnNode* CreateNode(const VistaPropertyList& oParams) const;

  VistaDisplayManager* m_pMgr;
};

class VISTAKERNELAPI VdfnClusterNodeInfoNodeCreate : public VdfnNodeFactory::IVdfnNodeCreator {
 public:
  VdfnClusterNodeInfoNodeCreate(VistaClusterMode* pDm);
  virtual IVdfnNode* CreateNode(const VistaPropertyList& oParams) const;
  VistaClusterMode*  m_pClusterMode;
};

class VISTAKERNELAPI VistaDfnDeviceDebugNodeCreate : public VdfnNodeFactory::IVdfnNodeCreator {
 public:
  VistaDfnDeviceDebugNodeCreate(VistaDisplayManager* pDisplayManager);
  virtual IVdfnNode*   CreateNode(const VistaPropertyList& oParams) const;
  VistaDisplayManager* m_pDisplayManager;
};

class VISTAKERNELAPI VdfnDumpHistoryNodeClusterCreate : public VdfnNodeFactory::IVdfnNodeCreator {
 public:
  VdfnDumpHistoryNodeClusterCreate(VistaClusterMode* pClusterMode);
  IVdfnNode*        CreateNode(const VistaPropertyList& oParams) const;
  VistaClusterMode* m_pClusterMode;
};

class VISTAKERNELAPI VistaDfnEventSourceNodeCreate : public VdfnNodeFactory::IVdfnNodeCreator {
 public:
  VistaDfnEventSourceNodeCreate(VistaEventManager* pEvMgr, VistaInteractionManager* pInMa);

  virtual IVdfnNode* CreateNode(const VistaPropertyList& oParams) const;

 private:
  VistaEventManager*       m_pEvMgr;
  VistaInteractionManager* m_pInMa;
};

class VISTAKERNELAPI VistaDfnNavigationNodeCreate : public VdfnNodeFactory::IVdfnNodeCreator {
 public:
  virtual IVdfnNode* CreateNode(const VistaPropertyList& oParams) const;

 private:
};

class VISTAKERNELAPI VistaDfnWindowSourceNodeCreate : public VdfnNodeFactory::IVdfnNodeCreator {
 public:
  VistaDfnWindowSourceNodeCreate(VistaDisplayManager* pMgr);
  virtual IVdfnNode* CreateNode(const VistaPropertyList& oParams) const;

 private:
  VistaDisplayManager* m_pMgr;
};

class VISTAKERNELAPI VistaDfnViewportSourceNodeCreate : public VdfnNodeFactory::IVdfnNodeCreator {
 public:
  VistaDfnViewportSourceNodeCreate(VistaDisplayManager* pMgr);
  virtual IVdfnNode* CreateNode(const VistaPropertyList& oParams) const;

 private:
  VistaDisplayManager* m_pMgr;
};

class VISTAKERNELAPI VistaDfnViewerSinkNodeCreate : public VdfnNodeFactory::IVdfnNodeCreator {
 public:
  VistaDfnViewerSinkNodeCreate(VistaDisplayManager* pMgr);
  virtual IVdfnNode* CreateNode(const VistaPropertyList& oParams) const;

 private:
  VistaDisplayManager* m_pMgr;
};

class VISTAKERNELAPI VistaDfnTrackballNodeCreate : public VdfnNodeFactory::IVdfnNodeCreator {
 public:
  VistaDfnTrackballNodeCreate();
  virtual IVdfnNode* CreateNode(const VistaPropertyList& oParams) const;
};

template <class T>
class VISTAKERNELAPI VistaDfnTextOverlayNodeCreate : public VdfnNodeFactory::IVdfnNodeCreator {
 public:
  VistaDfnTextOverlayNodeCreate(VistaDisplayManager* pDm)
      : m_pDm(pDm) {
  }

  virtual IVdfnNode* CreateNode(const VistaPropertyList& oParams) const {
    try {
      const VistaPropertyList& oSubs =
          oParams.GetPropertyConstRef("param").GetPropertyListConstRef();

      float nX = oSubs.GetValueOrDefault<float>("x_pos", 0.1f);
      float nY = oSubs.GetValueOrDefault<float>("y_pos", 0.1f);

      float nR = oSubs.GetValueOrDefault<float>("red", 1.0f);
      float nG = oSubs.GetValueOrDefault<float>("green", 1.0f);
      float nB = oSubs.GetValueOrDefault<float>("blue", 0.0f);

      int nSize = oSubs.GetValueOrDefault<int>("size", 20);

      std::string sIniText;
      oSubs.GetValue("value", sIniText);

      Vista2DText::Vista2DTextFontFamily eFamily = Vista2DText::SANS;

      std::string strFont;
      if (oSubs.GetValue("font", strFont)) {
        if (strFont.compare("TYPEWRITER") == 0)
          eFamily = Vista2DText::TYPEWRITER;
        else if (strFont.compare("SERIF") == 0)
          eFamily = Vista2DText::SERIF;
      }

      std::string  sViewport = oSubs.GetValueOrDefault<std::string>("viewport", "");
      Vista2DText* pText     = m_pDm->New2DText(sViewport);
      if (pText) {
        pText->Init(sIniText, nX, nY, int(255 * nR), int(255 * nG), int(255 * nB), nSize, eFamily);

        VistaDfnTextOverlayNode<T>* pSink = new VistaDfnTextOverlayNode<T>(pText);

        std::string sPostfix = oSubs.GetValueOrDefault<std::string>("postfix", "");
        std::string sPrefix  = oSubs.GetValueOrDefault<std::string>("prefix", "");

        pSink->SetPostfix(sPostfix);
        pSink->SetPrefix(sPrefix);

        return pSink;
      }
    } catch (VistaExceptionBase& x) { x.PrintException(); }

    return NULL;
  }

 private:
  VistaDisplayManager* m_pDm;
};

class VISTAKERNELAPI VistaDfnSystemTriggerControlNodeCreate
    : public VdfnNodeFactory::IVdfnNodeCreator {
 public:
  VistaDfnSystemTriggerControlNodeCreate(VistaKeyboardSystemControl* pCtrl);

  virtual IVdfnNode* CreateNode(const VistaPropertyList& oParams) const;

 private:
  VistaKeyboardSystemControl* m_pCtrl;
};

class VISTAKERNELAPI VistaDfnReferenceFrameTransformNodeCreate
    : public VdfnNodeFactory::IVdfnNodeCreator {
 public:
  VistaDfnReferenceFrameTransformNodeCreate(VistaDisplayManager* pDisplayManager);

  virtual IVdfnNode* CreateNode(const VistaPropertyList& oParams) const;

 private:
  VistaDisplayManager* m_pDisplayManager;
};

class VISTAKERNELAPI VistaDfnProjectionSourceNodeCreate : public VdfnNodeFactory::IVdfnNodeCreator {
 public:
  VistaDfnProjectionSourceNodeCreate(VistaDisplayManager* pMgr);
  virtual IVdfnNode* CreateNode(const VistaPropertyList& oParams) const;

 private:
  VistaDisplayManager* m_pMgr;
};

class VistaDfnFrameclockNodeCreate : public VdfnNodeFactory::IVdfnNodeCreator {
 public:
  class TimerNodeFrameclockGet;

  VistaDfnFrameclockNodeCreate(VistaClusterMode* pClusterAux);

  virtual IVdfnNode* CreateNode(const VistaPropertyList& oParams) const;

 private:
  VistaClusterMode* m_pClusterAux;
};

class VistaDfnCropViewportNodeCreate : public VdfnNodeFactory::IVdfnNodeCreator {
 public:
  VistaDfnCropViewportNodeCreate(VistaDisplayManager* pDisplayManager);

  virtual IVdfnNode* CreateNode(const VistaPropertyList& oParams) const;

 private:
  VistaDisplayManager* m_pDisplayManager;
};

class VistaDfnKeyCallbackNodeCreate : public VdfnNodeFactory::IVdfnNodeCreator {
 public:
  VistaDfnKeyCallbackNodeCreate(VistaKeyboardSystemControl* pKeyboard);

  virtual IVdfnNode* CreateNode(const VistaPropertyList& oParams) const;

 private:
  VistaKeyboardSystemControl* m_pKeyboard;
};

class VistaDfnKeyStateNodeCreate : public VdfnNodeFactory::IVdfnNodeCreator {
 public:
  VistaDfnKeyStateNodeCreate(VistaKeyboardSystemControl* pKeyboard);

  virtual IVdfnNode* CreateNode(const VistaPropertyList& oParams) const;

 private:
  VistaKeyboardSystemControl* m_pKeyboard;
};

class VistaDfnProximityWarningNodeCreate : public VdfnNodeFactory::IVdfnNodeCreator {
 public:
  VistaDfnProximityWarningNodeCreate(VistaSystem* pVistaSystem);

  virtual IVdfnNode* CreateNode(const VistaPropertyList& oParams) const;

 private:
  VistaSystem* m_pVistaSystem;
};

class VistaDfnFadeoutNodeCreate : public VdfnNodeFactory::IVdfnNodeCreator {
 public:
  VistaDfnFadeoutNodeCreate(VistaClusterMode* pClusterMode, VistaDisplayManager* pDisplayManager);

  virtual IVdfnNode* CreateNode(const VistaPropertyList& oParams) const;

 private:
  VistaClusterMode*    m_pClusterMode;
  VistaDisplayManager* m_pDisplayManager;
};

class VistaDfnSimpleTextNodeCreate : public VdfnNodeFactory::IVdfnNodeCreator {
 public:
  VistaDfnSimpleTextNodeCreate(VistaDisplayManager* pDisplayManager);

  virtual IVdfnNode* CreateNode(const VistaPropertyList& oParams) const;

 private:
  VistaDisplayManager* m_pDisplayManager;
};

class VistaDfnGeometryNodeCreate : public VdfnNodeFactory::IVdfnNodeCreator {
 public:
  VistaDfnGeometryNodeCreate(VistaSceneGraph* pSceneGraph);

  virtual IVdfnNode* CreateNode(const VistaPropertyList& oParams) const;

 private:
  VistaSceneGraph* m_pSceneGraph;
};

class VistaDfnPointingRayGeometryNodeCreate : public VdfnNodeFactory::IVdfnNodeCreator {
 public:
  VistaDfnPointingRayGeometryNodeCreate(VistaSceneGraph* pSceneGraph);

  virtual IVdfnNode* CreateNode(const VistaPropertyList& oParams) const;

 private:
  VistaSceneGraph* m_pSceneGraph;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTAKERNELDFNNODECREATORSS_H
