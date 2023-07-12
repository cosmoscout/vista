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

#include "VistaDfnViewerSourceNode.h"
#include <VistaDataFlowNet/VdfnPort.h>

#include <VistaAspects/VistaPropertyAwareable.h>
#include <VistaBase/VistaExceptionBase.h>
#include <VistaKernel/DisplayManager/VistaDisplayManager.h>
#include <VistaKernel/DisplayManager/VistaDisplaySystem.h>
#include <VistaKernel/DisplayManager/VistaVirtualPlatform.h>

#include <VistaDataFlowNet/VdfnUtil.h>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/
class VistaDfnViewerSourceNode::DispObserver : public IVistaObserver {
 public:
  enum {
    MSG_NONE             = 0,
    MSG_POS_CHANGE       = 1,
    MSG_ORI_CHANGE       = 2,
    MSG_LEY_CHANGE       = 4,
    MSG_REY_CHANGE       = 8,
    MSG_FRAME_POS_CHANGE = 16,
    MSG_FRAME_ORI_CHANGE = 32
  };

  enum { TICKET_PROPS, TICKET_FRAME };

  DispObserver(VistaDfnViewerSourceNode* pParent)
      : IVistaObserver()
      , m_pParent(pParent)
      , m_nPropChange(~0)
      , m_pDispProps(NULL)
      , m_pPlatform(NULL) {
  }

  ~DispObserver() {
    ReleaseObserveable(m_pDispProps, TICKET_PROPS);
    ReleaseObserveable(m_pPlatform, TICKET_FRAME);
  }

  virtual bool ObserveableDeleteRequest(
      IVistaObserveable* pObserveable, int nTicket = IVistaObserveable::TICKET_NONE) {
    return true;
  }

  virtual void ObserveableDelete(
      IVistaObserveable* pObserveable, int nTicket = IVistaObserveable::TICKET_NONE) {
    ReleaseObserveable(pObserveable, nTicket);
  }

  virtual void ReleaseObserveable(
      IVistaObserveable* pObserveable, int nTicket = IVistaObserveable::TICKET_NONE) {
    switch (nTicket) {
    case TICKET_PROPS: {
      if (m_pDispProps) {
        m_pDispProps->DetachObserver(this);
        m_pDispProps         = NULL;
        m_pParent->m_pSystem = NULL;
      }
      break;
    }
    case TICKET_FRAME: {
      if (m_pPlatform) {
        m_pPlatform->DetachObserver(this);
        m_pPlatform = NULL;
      }
      break;
    }
    default:
      break;
    }
  }

  virtual void ObserverUpdate(IVistaObserveable* pObserveable, int msg, int ticket) {
    switch (ticket) {
    case TICKET_PROPS: {
      switch (msg) {
      case VistaDisplaySystem::VistaDisplaySystemProperties::MSG_LEFT_EYE_OFFSET_CHANGE: {
        m_nPropChange = m_nPropChange | MSG_LEY_CHANGE;
        break;
      }
      case VistaDisplaySystem::VistaDisplaySystemProperties::MSG_RIGHT_EYE_OFFSET_CHANGE: {
        m_nPropChange = m_nPropChange | MSG_REY_CHANGE;
        break;
      }
      case VistaDisplaySystem::VistaDisplaySystemProperties::MSG_VIEWER_ORIENTATION_CHANGE: {
        m_nPropChange = m_nPropChange | MSG_ORI_CHANGE;
        break;
      }
      case VistaDisplaySystem::VistaDisplaySystemProperties::MSG_VIEWER_POSITION_CHANGE: {
        m_nPropChange = m_nPropChange | MSG_POS_CHANGE;
        break;
      }
      default: {
        break;
      }
      }
      break;
    }
    case TICKET_FRAME: {
      switch (msg) {
      case VistaVirtualPlatform::MSG_TRANSLATION_CHANGE: {
        m_nPropChange = m_nPropChange | MSG_FRAME_POS_CHANGE;
        break;
      }
      case VistaVirtualPlatform::MSG_ROTATION_CHANGE: {
        m_nPropChange = m_nPropChange | MSG_FRAME_ORI_CHANGE;
        break;
      }
      default: {
        break;
      }
      }
      break;
    }
    default: {
      break;
    }
    }
  }

  virtual bool Observes(IVistaObserveable* pObserveable) {
    if (pObserveable == NULL)
      return false;
    if (dynamic_cast<IVistaObserveable*>(m_pDispProps) == pObserveable)
      return true;
    if (dynamic_cast<IVistaObserveable*>(m_pPlatform) == pObserveable)
      return true;

    return false;
  }

  virtual void Observe(
      IVistaObserveable* pObservable, int eTicket = IVistaObserveable::TICKET_NONE) {

    switch (eTicket) {
    case TICKET_PROPS: {
      if (m_pDispProps)
        m_pDispProps->DetachObserver(this);

      m_pDispProps = dynamic_cast<VistaDisplaySystem::VistaDisplaySystemProperties*>(pObservable);
      if (m_pDispProps)
        m_pDispProps->AttachObserver(this, eTicket);

      break;
    }
    case TICKET_FRAME: {
      if (m_pPlatform)
        m_pPlatform->DetachObserver(this);

      m_pPlatform = dynamic_cast<VistaVirtualPlatform*>(pObservable);
      if (m_pPlatform)
        m_pPlatform->AttachObserver(this, eTicket);

      break;
    }
    default:
      break;
    }
  }

  int GetPropsChangeFlag() const {
    return m_nPropChange;
  }

  int GetPropsChange() {
    int bRet      = m_nPropChange;
    m_nPropChange = MSG_NONE;
    return bRet;
  }

 private:
  VistaDfnViewerSourceNode*                         m_pParent;
  VistaDisplaySystem::VistaDisplaySystemProperties* m_pDispProps;
  VistaVirtualPlatform*                             m_pPlatform;

  int m_nPropChange;
};

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaDfnViewerSourceNode::VistaDfnViewerSourceNode(VistaDisplaySystem* pSys)
    : IVdfnNode()
    , m_pPosition(new TVdfnPort<VistaVector3D>)
    , m_pTranslation(new TVdfnPort<VistaVector3D>)
    , m_pOrientation(new TVdfnPort<VistaQuaternion>)
    , m_pRotation(new TVdfnPort<VistaQuaternion>)
    , m_pPlatformPosition(new TVdfnPort<VistaVector3D>)
    , m_pPlatformOrientation(new TVdfnPort<VistaQuaternion>)
    , m_pLeftEye(new TVdfnPort<VistaVector3D>)
    , m_pRightEye(new TVdfnPort<VistaVector3D>)
    , m_pSystem(pSys)
    , m_pObs(NULL)
    , m_nUpdateCount(0)

{
  RegisterOutPort("position", m_pPosition);
  RegisterOutPort("orientation", m_pOrientation);
  RegisterOutPort("translation", m_pTranslation);
  RegisterOutPort("rotation", m_pRotation);
  RegisterOutPort("platform_position", m_pPlatformPosition);
  RegisterOutPort("platform_orientation", m_pPlatformOrientation);
  RegisterOutPort("left_eye", m_pLeftEye);
  RegisterOutPort("right_eye", m_pRightEye);

  m_pObs = new DispObserver(this);
  m_pObs->Observe(m_pSystem->GetDisplaySystemProperties(), DispObserver::TICKET_PROPS);
  m_pObs->Observe(m_pSystem->GetReferenceFrame(), DispObserver::TICKET_FRAME);

  m_pSystem->GetDisplaySystemProperties()->GetViewerPosition(m_pPosition->GetValueRef());
  m_pPosition->IncUpdateCounter();

  m_pSystem->GetDisplaySystemProperties()->GetViewerOrientation(m_pOrientation->GetValueRef());
  m_pOrientation->IncUpdateCounter();

  m_pSystem->GetReferenceFrame()->GetTranslation(m_pPlatformPosition->GetValueRef());
  m_pPlatformPosition->IncUpdateCounter();

  m_pSystem->GetReferenceFrame()->GetRotation(m_pPlatformOrientation->GetValueRef());
  m_pPlatformOrientation->IncUpdateCounter();

  VistaVector3D& vL = m_pLeftEye->GetValueRef();
  m_pSystem->GetDisplaySystemProperties()->GetLeftEyeOffset(vL[0], vL[1], vL[2]);
  m_pLeftEye->IncUpdateCounter();

  VistaVector3D& vR = m_pRightEye->GetValueRef();
  m_pSystem->GetDisplaySystemProperties()->GetRightEyeOffset(vR[0], vR[1], vR[2]);
  m_pRightEye->IncUpdateCounter();
}

VistaDfnViewerSourceNode::~VistaDfnViewerSourceNode() {
  delete m_pObs;
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

VistaDisplaySystem* VistaDfnViewerSourceNode::GetDisplaySystem() const {
  return m_pSystem;
}

void VistaDfnViewerSourceNode::SetDisplaySystem(VistaDisplaySystem* pSys) {
  if (m_pSystem) {
    m_pObs->ReleaseObserveable(pSys->GetDisplaySystemProperties());
  }

  m_pSystem = pSys;

  if (m_pSystem) {
    m_pObs->Observe(m_pSystem->GetDisplaySystemProperties());
  }
}

bool VistaDfnViewerSourceNode::GetIsValid() const {
  return (m_pSystem != NULL);
}

bool VistaDfnViewerSourceNode::DoEvalNode() {
  int                                               nChg = m_pObs->GetPropsChange();
  VistaDisplaySystem::VistaDisplaySystemProperties* pProps =
      m_pSystem->GetDisplaySystemProperties();

  if (nChg & DispObserver::MSG_POS_CHANGE) {
    m_pPosition->SetValue(pProps->GetViewerPosition(), GetUpdateTimeStamp());
    if (pProps->GetLocalViewer()) {
      m_pTranslation->SetValue(m_pPosition->GetValueConstRef(), GetUpdateTimeStamp());
    } else {
      // world viewer		}
      VistaVector3D v3Trans(m_pSystem->GetReferenceFrame()->TransformPositionToFrame(
          m_pPosition->GetValueConstRef()));
      m_pTranslation->SetValue(v3Trans, GetUpdateTimeStamp());
    }
  }

  if (nChg & DispObserver::MSG_ORI_CHANGE) {
    m_pOrientation->SetValue(pProps->GetViewerOrientation(), GetUpdateTimeStamp());
    if (pProps->GetLocalViewer()) {
      m_pRotation->SetValue(m_pOrientation->GetValueConstRef(), GetUpdateTimeStamp());
    } else {
      VistaQuaternion q(m_pSystem->GetReferenceFrame()->TransformOrientationToFrame(
          m_pOrientation->GetValueConstRef()));
      m_pRotation->SetValue(q, GetUpdateTimeStamp());
    }
  }

  if (nChg & DispObserver::MSG_FRAME_POS_CHANGE) {
    m_pSystem->GetReferenceFrame()->GetTranslation(m_pPlatformPosition->GetValueRef());
    m_pPlatformPosition->IncUpdateCounter();
  }

  if (nChg & DispObserver::MSG_FRAME_ORI_CHANGE) {
    m_pSystem->GetReferenceFrame()->GetRotation(m_pPlatformOrientation->GetValueRef());
    m_pPlatformOrientation->IncUpdateCounter();
  }

  if (nChg & DispObserver::MSG_LEY_CHANGE) {
    VistaVector3D& vL = m_pLeftEye->GetValueRef();
    pProps->GetLeftEyeOffset(vL[0], vL[1], vL[2]);
    m_pLeftEye->IncUpdateCounter();
  }

  if (nChg & DispObserver::MSG_REY_CHANGE) {
    VistaVector3D& vR = m_pRightEye->GetValueRef();
    pProps->GetRightEyeOffset(vR[0], vR[1], vR[2]);
    m_pRightEye->IncUpdateCounter();
  }

  return true;
}

unsigned int VistaDfnViewerSourceNode::CalcUpdateNeededScore() const {
  if (m_pObs->GetPropsChangeFlag()) // use the const-version here, we need the flags later
  {
    return ++(m_nUpdateCount);
  }
  return m_nUpdateCount;
}

// #############################################################################

VistaDfnViewerSourceNodeCreate::VistaDfnViewerSourceNodeCreate(VistaDisplayManager* pMgr)
    : VdfnNodeFactory::IVdfnNodeCreator()
    , m_pMgr(pMgr) {
}

IVdfnNode* VistaDfnViewerSourceNodeCreate::CreateNode(const VistaPropertyList& oParams) const {
  try {
    const VistaPropertyList& subs = oParams.GetPropertyConstRef("param").GetPropertyListConstRef();

    if (subs.HasProperty("displaysystem")) {
      std::string strDispSysName;
      subs.GetValue("displaysystem", strDispSysName);
      VistaDisplaySystem* pSystem = m_pMgr->GetDisplaySystemByName(strDispSysName);
      if (pSystem) {
        return new VistaDfnViewerSourceNode(pSystem);
      }
    }
  } catch (VistaExceptionBase& x) { x.PrintException(); }

  return NULL;
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/
