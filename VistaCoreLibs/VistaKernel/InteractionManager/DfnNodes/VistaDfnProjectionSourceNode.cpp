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

#include "VistaDfnProjectionSourceNode.h"
#include <VistaDataFlowNet/VdfnPort.h>

#include <VistaKernel/DisplayManager/VistaProjection.h>
#include <VistaKernel/EventManager/VistaEventManager.h>

#include <VistaAspects/VistaPropertyAwareable.h>
#include <VistaBase/VistaExceptionBase.h>
#include <VistaKernel/DisplayManager/VistaDisplayManager.h>

#include <VistaAspects/VistaObserver.h>

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/
class VistaDfnProjectionSourceNode::ProjectionObserver : public IVistaObserver {
 public:
  enum {
    MSG_NONE         = 0,
    MSG_MIDPOINT_CHG = 1,
    MSG_NORMAL_CHG   = 2,
    MSG_UP_CHG       = 4,
    MSG_EXTENTS_CHG  = 8,
    MSG_CLIPRG_CHG   = 16
  };

  ProjectionObserver(VistaDfnProjectionSourceNode* pParent)
      : IVistaObserver()
      , m_pParent(pParent)
      , m_pObs(NULL)
      , m_nVpChange(false) {
  }

  ~ProjectionObserver() {
    if (Observes(m_pObs))
      ReleaseObserveable(m_pObs);
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
    pObserveable->DetachObserver(this);
    m_pParent->m_pProjection = NULL;
  }

  virtual void ObserverUpdate(IVistaObserveable* pObserveable, int msg, int ticket) {
    switch (msg) {
    case VistaProjection::VistaProjectionProperties::MSG_CLIPPING_RANGE_CHANGE: {
      m_nVpChange = m_nVpChange | MSG_CLIPRG_CHG;
      break;
    }
    case VistaProjection::VistaProjectionProperties::MSG_PROJ_PLANE_MIDPOINT_CHANGE: {
      m_nVpChange = m_nVpChange | MSG_MIDPOINT_CHG;
      break;
    }
    case VistaProjection::VistaProjectionProperties::MSG_PROJ_PLANE_NORMAL_CHANGE: {
      m_nVpChange = m_nVpChange | MSG_NORMAL_CHG;
      break;
    }
    case VistaProjection::VistaProjectionProperties::MSG_PROJ_PLANE_UP_CHANGE: {
      m_nVpChange = m_nVpChange | MSG_UP_CHG;
      break;
    }
    case VistaProjection::VistaProjectionProperties::MSG_PROJ_PLANE_EXTENTS_CHANGE: {
      m_nVpChange = m_nVpChange | MSG_EXTENTS_CHG;
      break;
    }
    default:
      break;
    }
  }

  virtual bool Observes(IVistaObserveable* pObserveable) {
    return (m_pObs == pObserveable);
  }

  virtual void Observe(
      IVistaObserveable* pObservable, int eTicket = IVistaObserveable::TICKET_NONE) {
    if (m_pObs)
      m_pObs->DetachObserver(this);

    m_pObs = pObservable;

    if (m_pObs)
      m_pObs->AttachObserver(this, eTicket);
  }

  int GetPropsChangeFlag() const {
    return m_nVpChange;
  }

  int GetPropsChange() {
    int bRet    = m_nVpChange;
    m_nVpChange = false;
    return bRet;
  }

 private:
  VistaDfnProjectionSourceNode* m_pParent;
  int                           m_nVpChange;
  IVistaObserveable*            m_pObs;
};

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaDfnProjectionSourceNode::VistaDfnProjectionSourceNode(VistaProjection* pProjection)
    : IVdfnNode()
    , m_pMidpoint(new TVdfnPort<VistaVector3D>)
    , m_pNormal(new TVdfnPort<VistaVector3D>)
    , m_pUp(new TVdfnPort<VistaVector3D>)
    , m_pLeft(new TVdfnPort<double>)
    , m_pRight(new TVdfnPort<double>)
    , m_pTop(new TVdfnPort<double>)
    , m_pBottom(new TVdfnPort<double>)
    , m_pNear(new TVdfnPort<double>)
    , m_pFar(new TVdfnPort<double>)
    , m_pProjection(pProjection)
    , m_pUpd(new _sUpdate)
    , m_pObs(NULL) {
  RegisterOutPort("midpoint", m_pMidpoint);
  RegisterOutPort("normal", m_pNormal);
  RegisterOutPort("up", m_pUp);
  RegisterOutPort("left", m_pLeft);
  RegisterOutPort("right", m_pRight);
  RegisterOutPort("top", m_pTop);
  RegisterOutPort("bottom", m_pBottom);
  RegisterOutPort("near", m_pNear);
  RegisterOutPort("far", m_pFar);

  m_pObs = new ProjectionObserver(this);
  m_pObs->Observe(m_pProjection->GetProjectionProperties());

  VistaVector3D& vUp = m_pUp->GetValueRef();
  m_pProjection->GetProjectionProperties()->GetProjPlaneUp(vUp[0], vUp[1], vUp[2]);
  m_pUp->IncUpdateCounter();
  double& nLeft   = m_pLeft->GetValueRef();
  double& nRight  = m_pRight->GetValueRef();
  double& nTop    = m_pTop->GetValueRef();
  double& nBottom = m_pBottom->GetValueRef();

  m_pProjection->GetProjectionProperties()->GetProjPlaneExtents(nLeft, nRight, nBottom, nTop);

  m_pLeft->IncUpdateCounter();
  m_pRight->IncUpdateCounter();
  m_pTop->IncUpdateCounter();
  m_pBottom->IncUpdateCounter();
  VistaVector3D& vMp = m_pMidpoint->GetValueRef();
  m_pProjection->GetProjectionProperties()->GetProjPlaneMidpoint(vMp[0], vMp[1], vMp[2]);
  m_pMidpoint->IncUpdateCounter();
  VistaVector3D& vN = m_pNormal->GetValueRef();
  m_pProjection->GetProjectionProperties()->GetProjPlaneNormal(vN[0], vN[1], vN[2]);
  m_pNormal->IncUpdateCounter();
  double& nNear = m_pNear->GetValueRef();
  double& nFar  = m_pFar->GetValueRef();

  m_pProjection->GetProjectionProperties()->GetClippingRange(nNear, nFar);
  m_pNear->IncUpdateCounter();
  m_pFar->IncUpdateCounter();
}

VistaDfnProjectionSourceNode::~VistaDfnProjectionSourceNode() {
  delete m_pUpd;
  delete m_pObs;
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

bool VistaDfnProjectionSourceNode::GetIsValid() const {
  return (m_pProjection != NULL);
}

bool VistaDfnProjectionSourceNode::DoEvalNode() {
  int nMsk = m_pObs->GetPropsChange();

  if (nMsk & ProjectionObserver::MSG_UP_CHG) {
    VistaVector3D& vUp = m_pUp->GetValueRef();
    m_pProjection->GetProjectionProperties()->GetProjPlaneUp(vUp[0], vUp[1], vUp[2]);
    m_pUp->IncUpdateCounter();
  }

  if (nMsk & ProjectionObserver::MSG_EXTENTS_CHG) {
    double& nLeft   = m_pLeft->GetValueRef();
    double& nRight  = m_pRight->GetValueRef();
    double& nTop    = m_pTop->GetValueRef();
    double& nBottom = m_pBottom->GetValueRef();

    m_pProjection->GetProjectionProperties()->GetProjPlaneExtents(nLeft, nRight, nBottom, nTop);

    m_pLeft->IncUpdateCounter();
    m_pRight->IncUpdateCounter();
    m_pTop->IncUpdateCounter();
    m_pBottom->IncUpdateCounter();
  }

  if (nMsk & ProjectionObserver::MSG_MIDPOINT_CHG) {
    VistaVector3D& vMp = m_pMidpoint->GetValueRef();
    m_pProjection->GetProjectionProperties()->GetProjPlaneMidpoint(vMp[0], vMp[1], vMp[2]);
    m_pMidpoint->IncUpdateCounter();
  }

  if (nMsk & ProjectionObserver::MSG_NORMAL_CHG) {
    VistaVector3D& vN = m_pNormal->GetValueRef();
    m_pProjection->GetProjectionProperties()->GetProjPlaneNormal(vN[0], vN[1], vN[2]);
    m_pNormal->IncUpdateCounter();
  }

  if (nMsk & ProjectionObserver::MSG_CLIPRG_CHG) {
    double& nNear = m_pNear->GetValueRef();
    double& nFar  = m_pFar->GetValueRef();

    m_pProjection->GetProjectionProperties()->GetClippingRange(nNear, nFar);
    m_pNear->IncUpdateCounter();
    m_pFar->IncUpdateCounter();
  }

  return true;
}

unsigned int VistaDfnProjectionSourceNode::CalcUpdateNeededScore() const {
  if (m_pUpd->m_bNeedsUpdate ||
      m_pObs->GetPropsChangeFlag()) // use the const-version here, we need the flags later
  {
    _sUpdate* pUpd       = m_pUpd;
    pUpd->m_bNeedsUpdate = false; // one-timer
    return ++(pUpd->m_nUpdateCount);
  }
  return m_pUpd->m_nUpdateCount;
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/
